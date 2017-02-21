#ifndef __BOOLEAN_FUNCTION_HPP
#define __BOOLEAN_FUNCTION_HPP

#include <utility>
#include <algorithm>
#include <string>

#include <stdexcept>
#include <cassert>

namespace Boolean {
	typedef unsigned long Base;
	const size_t base_bitsize = sizeof(Base) << 3;

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
			for (size_t i = 0; i < bitsize / mask_size; ++i)
				mask = (mask << mask_size) | ((static_cast<T>(1) << (mask_size >> 1)) - 1);
			result = (result & mask) + ((result & ~mask) >> (mask_size >> 1));
		}

		return result;
	}

	class Function {
		private:
			Base* body;
			size_t arguments;
			size_t size;

		public:
			friend void swap(Function& one, Function& another) {
				using std::swap;

				swap(one.body, another.body);
				swap(one.arguments, another.arguments);
				swap(one.size, another.size);
			}

			Function();
			Function(Function const& another);
			Function(Function&& another);
			~Function();

			Function(size_t arguments);

			Function& operator=(Function const& another);
			Function& operator=(Function&& another);
			
			void set_value(size_t argument_tuple, bool value) {
				assert(argument_tuple / base_bitsize < size);
				if (value != 0) {
					body[argument_tuple / base_bitsize] |= static_cast<Base>(1) << (argument_tuple % base_bitsize);
				} else {
					body[argument_tuple / base_bitsize] &= ~(static_cast<Base>(1) << argument_tuple % base_bitsize);
				}
			}
			bool operator()(size_t argument_tuple) {
				return (body[argument_tuple / base_bitsize] & (static_cast<Base>(1) << argument_tuple % base_bitsize)) != 0;
			}

			size_t weight() const;
			size_t bitsize() const {
				return 1 << arguments;
			}
	};

	Function to_function(std::string str);
}

#endif //__BOOLEAN_FUNCTION_HPP
