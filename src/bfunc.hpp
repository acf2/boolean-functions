#ifndef __BOOLEAN_FUNCTION_HPP
#define __BOOLEAN_FUNCTION_HPP

#include <utility>
#include <algorithm>
#include <string>
#include <iostream>
#include <unordered_map>

#include <stdexcept>
#include <cassert>

namespace Boolean {
	template<typename T>
	size_t count_tail_zeros(T number) {
		assert(number != 0);
		size_t const bitsize = sizeof(T) << 3;
		size_t zeros = 0;

		for (size_t step = bitsize >> 1; step != 0; step >>= 1) {
			if ((number << bitsize - step) == 0) {
				zeros += step;
				number >>= step;
			}
		}

		return zeros;
	}

	template<typename T>
	size_t weight(T vector) {
		size_t const bitsize = sizeof(T) << 3;

		T mask = static_cast<T>(0);
		T result = vector;
		for (size_t mask_size = 2; mask_size <= bitsize; mask_size <<= 1) {
			mask = static_cast<T>(0);
			//TODO: сделать генерацию масок во время компиляции
			for (size_t i = 0; i < bitsize / mask_size; ++i)
				mask = (mask << mask_size) | ((static_cast<T>(1) << (mask_size >> 1)) - 1);
			result = (result & mask) + ((result & ~mask) >> (mask_size >> 1));
		}

		return result;
	}

	struct AffineFunction {
		size_t coefficient;
		size_t arguments;
		bool constant;
	};

	// Combinatorics
	size_t initial_combination(size_t cardinality, size_t elements);
	size_t next_combination(size_t combination);
	bool is_last_combination(size_t combination);

	typedef unsigned long Base;
	const size_t base_bitsize = sizeof(Base) << 3;

	class Function {
		private:
			Base* body;
			size_t arguments;
			size_t size;

		public:
			friend void swap(Function& one, Function& another) noexcept {
				using std::swap;

				swap(one.body, another.body);
				swap(one.arguments, another.arguments);
				swap(one.size, another.size);
			}

			Function();
			Function(Function const& another);
			Function(Function&& another);
			~Function();

			enum class Generation {Zeros, Randomly};
			Function(size_t arguments, Generation method = Generation::Zeros);

			Function& operator=(Function const& another);
			Function& operator=(Function&& another);

			bool operator==(Function const& another) const;
			bool operator!=(Function const& another) const;
			
			void set_value(size_t argument_tuple, bool value) {
				assert(argument_tuple / base_bitsize < size);
				if (value != 0) {
					body[argument_tuple / base_bitsize] |= static_cast<Base>(1) << (argument_tuple % base_bitsize);
				} else {
					body[argument_tuple / base_bitsize] &= ~(static_cast<Base>(1) << argument_tuple % base_bitsize);
				}
			}
			bool operator()(size_t argument_tuple) const {
				return (body[argument_tuple / base_bitsize] & (static_cast<Base>(1) << argument_tuple % base_bitsize)) != 0;
			}

			size_t get_arguments() const {
				return arguments;
			}
			size_t bitsize() const {
				return static_cast<size_t>(1) << arguments;
			}
			size_t weight() const;
			size_t degree() const;
			Function mobius() const;

			friend std::ostream& operator<<(std::ostream& os, Function const& func);
	};

	class Spectrum : public std::vector<int> {
		protected:
			size_t arguments;

		public:
			Spectrum(Function const& func);
			size_t get_arguments() const {
				return arguments;
			}
	};

	class Symbols {
		public:
			static std::unordered_map<char, std::string> const digits;
			static std::string const sign;
			static std::string const variable;
			static std::string const separator;
	};

	Function to_function(std::string str);
	std::string to_formula(Function mobius);
	std::string to_formula(AffineFunction affine_func);
	unsigned long long nonlinearity(Spectrum const& wh);
	//size_t correlation_immunity_order(size_t boundary = sizeof(size_t) << 3) const;
	AffineFunction best_affine_approximation(Spectrum const& wh);
}

#endif //__BOOLEAN_FUNCTION_HPP
