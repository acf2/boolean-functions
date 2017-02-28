#include "bfunc.hpp"
#include <iostream>

using namespace Boolean;

int main() {
	Function F;
	for (size_t i = 2; i < 8; ++i) {
		F = Function(i, Function::Generation::Randomly);
		std::cout << i << ": " << to_formula(F) << std::endl << std::endl;
	}
}
