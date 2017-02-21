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
		size_t bitsize = sizeof(T) << 3, zeros = 0;

		for (size_t step = bitsize >> 1; step != 0; step >>= 1) {
			if ((number << bitsize - step) == 0) {
				zeros += step;
				number >>= step;
			}
		}

		return zeros;
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
	};

	Function to_function(std::string str);
}

#endif //__BOOLEAN_FUNCTION_HPP
