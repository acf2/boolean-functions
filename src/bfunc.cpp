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

	Function::Function(size_t arguments) : arguments(arguments) {
		size = (1 << arguments) / base_bitsize + ((1 << arguments) % base_bitsize == 0 ? 0 : 1);
		body = new Base[size];
		std::fill_n(body, size, 0);
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
}
