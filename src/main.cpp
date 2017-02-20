#include "bfunc.hpp"
#include <iostream>

using Boolean::Function;
using Boolean::to_function;

int main() {
	Function F = to_function("11010010");

	for (int i = 0; i < 8; ++i)
		std::cout << "F(" << i << ") = " << F(i) << std::endl;
}
