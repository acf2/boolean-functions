#include "bfunc.hpp"

namespace Boolean {
	Function::Function() : body(new Base[1]), arguments(0), size(1) { }

	Function::Function(Function const& another) : arguments(another.arguments), size(another.size) {
		body = new Base[size];
		std::copy_n(another.body, size, body);
	}

	Function::Function(Function&& another) {
		swap(*this, another);
	}

	Function::~Function() {
		delete[] body;
	}

	Function& Function::operator=(Function const& another) {
		Function temp(another);
		swap(*this, temp);
		return *this;
	}

	Function& Function::operator=(Function&& another) {
		swap(*this, another);
		return *this;
	}

	bool Function::operator==(Function const& another) const {
		if (this->size != another.size) return 0;
		if ((1 << arguments) % base_bitsize == 0) {
			for (size_t i = 0; i < this->size; ++i) {
				if (this->body[i] != another.body[i]) return 0;
			}
		} else {
			for (size_t i = 0; i < this->size - 1; ++i) {
				if (this->body[i] != another.body[i]) return 0;
			}
			if ((this->body[this->size] & (1 << arguments) - 1) != (another.body[another.size] & (1 << arguments) - 1)) return 0;
		}
		return 1;
	}

	bool Function::operator!=(Function const& another) const {
		return !(*this == another);
	}

	Function::Function(size_t arguments, Generation method) : arguments(arguments) {
		size = (static_cast<size_t>(1) << arguments) / base_bitsize + ((static_cast<size_t>(1) << arguments) % base_bitsize == 0 ? 0 : 1);
		body = new Base[size];

		switch (method) {
			case Generation::Zeros:
				std::fill_n(body, size, 0);
				break;
			case Generation::Randomly:
				std::random_device gen;
				std::uniform_int_distribution<Base> dist(0, ~static_cast<Base>(0));
				std::generate_n(body, size, [&dist, &gen]() -> Base {
					return dist(gen);
				});
				break;
		}
	}

	size_t Function::weight() const {
		size_t result = 0;
		if ((static_cast<size_t>(1) << arguments) % base_bitsize == 0) {
			std::for_each(body, std::next(body, size), [&result](Base const& base) -> void {
				result += Boolean::weight(base);
			});
		} else {
			std::for_each(body, std::next(body, size-1), [&result](Base const& base) -> void {
				result += Boolean::weight(base);
			});
			result += Boolean::weight(body[size-1] & ((static_cast<Base>(1) << (static_cast<size_t>(1) << arguments) % base_bitsize) - 1));
		}
		return result;
	}

	size_t Function::degree() const {
		size_t result = 0;
		size_t temp;
		if ((static_cast<size_t>(1) << arguments) % base_bitsize == 0) {
			for (size_t base = 0; base < size; ++base) {
				if (body[base] == 0) continue;
				for (size_t bit = 0; bit < base_bitsize; ++bit) {
					if ((body[base] & (static_cast<Base>(1) << bit)) == 0)
						continue;
					temp = Boolean::weight(base * base_bitsize + bit);
					if (temp > result) result = temp;
				}
			}
		} else {
			for (size_t base = 0; base < size-1; ++base) {
				if (body[base] == 0) continue;
				for (size_t bit = 0; bit < base_bitsize; ++bit) {
					if ((body[base] & (static_cast<Base>(1) << bit)) == 0)
						continue;
					temp = Boolean::weight(base * base_bitsize + bit);
					if (temp > result) result = temp;
				}
			}
			if (body[size-1] == 0) return result;
			for (size_t bit = 0; bit < (static_cast<size_t>(1) << arguments) % base_bitsize; ++bit) {
				if ((body[size-1] & (static_cast<Base>(1) << bit)) == 0)
					continue;
				temp = Boolean::weight((size-1) * base_bitsize + bit);
				if (temp > result) result = temp;
			}
		}
		return result;
	}

	Function Function::mobius() const {
		Function result(*this);

		// Для каждой базы отдельно - до маски размера base_bitsize/2
		Base mask = 0, temp;
		for (size_t bits = 1; bits < std::min(base_bitsize, static_cast<size_t>(1) << result.arguments); bits <<= 1) {
			for (size_t i = 0; i < result.size; ++i) {
				// TODO: Тоже генерацию масок во время компиляции хотелось бы
				for (Base mask = (1 << bits) - 1; mask != 0; mask <<= bits * 2) {
					temp = result.body[i] & mask;
					result.body[i] ^= temp << bits;
				}
			}
		}

		assert((result.size & result.size - 1) == 0); //aka: exists k (result.size == power(2, k))
		// Для масок с размерами, кратными размерам баз
		for (size_t bases = 1; bases < result.size; bases <<= 1) {
			// Цикл по отрезкам: источник-назначение
			for (size_t segment = 0; segment < result.size / (bases * 2); ++segment) {
				for (size_t base = 0; base < bases; ++base) {
					result.body[segment * bases * 2 + bases + base] ^= result.body[segment * bases * 2 + base];
				}
			}
		}

		return result;
	}

	unsigned long long Function::nonlinearity() const {
		using std::vector;

		vector<int> wh = wh_transform(*this);
		unsigned long long result = (wh[0] < 0 ? -wh[0] : wh[0]), temp;
		for (size_t i = 1; i < wh.size(); ++i) {
			temp = static_cast<unsigned long long>(wh[i] < 0 ? -wh[i] : wh[i]);
			if (result < temp) result = temp;
		}
		return (static_cast<long long>(1) << this->arguments - 1) - result / 2;
	}

	Function to_function(std::string str) {
		if ((str.size() & (str.size() - 1)) != 0)
			throw std::domain_error("lenght is not power of 2");
		size_t args = count_tail_zeros(str.size());

		Function result(args);
		for (size_t i = 0; i < str.size(); ++i) {
			result.set_value(i, (str[i] == '0' ? 0 : 1));
		}
		return result;
	}

	std::string to_formula(Function mobius) {
		auto construct_monomial = [](Function const& mobius, size_t index) -> std::string {
			std::string monomial;
			std::string buffer;
			if (index == 0) return std::string("1");
			for (size_t i = 0; i < mobius.get_arguments(); ++i) {
				if ((index & static_cast<size_t>(1) << (mobius.get_arguments() - i - 1)) == 0)
					continue;
				monomial += Symbols::variable;
				buffer = std::to_string(i + 1);
				for (auto ch: buffer)
					monomial += Symbols::digits.at(ch);
			}
			return monomial;
		};

		std::string result;

		for (size_t i = 0; i < mobius.bitsize(); ++i) {
			if (mobius(i) == 0) continue;
			if (!result.empty())
				result += Symbols::separator + Symbols::sign + Symbols::separator;
			result += construct_monomial(mobius, i);
		}
		return (result.empty() ? "0" : result);
	}

	std::string to_formula(AffineFunction affine_func) {
		auto construct_monomial = [](AffineFunction af, size_t index) -> std::string {
			std::string monomial;
			std::string buffer;
			monomial += Symbols::variable;
			buffer = std::to_string(af.arguments - index);
			for (auto ch: buffer)
				monomial += Symbols::digits.at(ch);
			return monomial;
		};

		std::string result;

		for (size_t i = 0; i < affine_func.arguments; ++i) {
			if ((affine_func.coefficient & (1 << i)) == 0) continue;
			if (!result.empty())
				result += Symbols::separator + Symbols::sign + Symbols::separator;
			result += construct_monomial(affine_func, i);
		}
		if (affine_func.constant != 0) {
			if (!result.empty())
				result += Symbols::separator + Symbols::sign + Symbols::separator;
			result += "1";
		}

		return (result.empty() ? "0" : result);
	}

	std::vector<int> wh_transform(Function const& func) {
		using std::swap;
		std::vector<int> result(func.bitsize());

		for (size_t i = 0; i < func.bitsize(); ++i) {
			result[i] = (func(i) != 0 ? -1 : 1);
		}

		int sum, diff;
		for (size_t segment_size = 1; segment_size < result.size(); segment_size <<= 1) {
			for (size_t segment = 0; segment < result.size() / (segment_size * 2); ++segment) {
				for (size_t cell = 0; cell < segment_size; ++cell) {
					sum = result[segment_size * segment * 2 + cell]
						+ result[segment_size * segment * 2 + segment_size + cell];
					diff = result[segment_size * segment * 2 + cell]
						 - result[segment_size * segment * 2 + segment_size + cell];
					result[segment_size * segment * 2 + cell] = sum;
					result[segment_size * segment * 2 + segment_size + cell] = diff;
				}
			}
		}

		return result;
	}

	AffineFunction best_affine_approximation(Function const& func) {
		using std::vector;

		vector<int> wh = wh_transform(func);
		AffineFunction result;
		result.arguments = func.get_arguments();
		unsigned long long biggest = (wh[0] < 0 ? -wh[0] : wh[0]), temp;
		size_t index = 0;
		for (size_t i = 1; i < wh.size(); ++i) {
			temp = static_cast<unsigned long long>(wh[i] < 0 ? -wh[i] : wh[i]);
			if (biggest < temp) {
				biggest = temp;
				index = i;
			}
		}
		result.coefficient = index;
		if (wh[index] > 0) result.constant = 0;
		else if (wh[index] < 0) result.constant = 1;
		else assert(0); //Something is wrong

		return result;
	}

	std::ostream& operator<<(std::ostream& os, Function const& func) {
		for (size_t i = 0; i < func.size - 1; ++i)
			for (size_t bit = 0; bit < base_bitsize; ++bit)
				os << ((func.body[i] & static_cast<Base>(1) << bit) == 0 ? '0' : '1');
		for (size_t bit = 0; bit < (1 << func.arguments) % base_bitsize; ++bit)
			os << ((func.body[func.size-1] & static_cast<Base>(1) << bit) == 0 ? '0' : '1');
		return os;
	}
}

