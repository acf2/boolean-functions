#include "bfunc.hpp"
#include <iostream>

using namespace Boolean;

int main() {
	Function F;
	for (size_t i = 2; i < 32; ++i) {
		F = Function(i, Function::Generation::Randomly);
		std::cout << i << ": " << static_cast<double>(F.weight()) / (static_cast<size_t>(1) << i) << std::endl;
	}
}
