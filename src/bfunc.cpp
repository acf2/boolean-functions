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
		std::unordered_map<char, std::string> const digits = {{'0',"\u2080"}, {'1', "\u2081"}, {'2', "\u2082"}, {'3', "\u2083"}, {'4', "\u2084"}, {'5', "\u2085"}, {'6', "\u2086"}, {'7', "\u2087"}, {'8', "\u2088"}, {'9', "\u2089"}}; 
		std::string const sign = "\u2295";
		std::string const variable = "x";
		std::string const separator = " ";

		auto construct_monomial = [&digits, &sign, &variable](Function const& mobius, size_t index) -> std::string {
			std::string monomial;
			std::string buffer;
			if (index == 0) return std::string("1");
			for (size_t i = 0; i < mobius.get_arguments(); ++i) {
				if ((index & static_cast<size_t>(1) << (mobius.get_arguments() - i - 1)) == 0)
					continue;
				monomial += variable;
				buffer = std::to_string(i + 1);
				for (auto ch: buffer)
					monomial += digits.at(ch);
			}
			return monomial;
		};

		std::string result;

		for (size_t i = 0; i < mobius.bitsize(); ++i) {
			if (mobius(i) == 0) continue;
			if (!result.empty())
				result += separator + sign + separator;
			result += construct_monomial(mobius, i);
		}
		return (result.empty() ? "0" : result);
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

