#include "bfunc.hpp"
#include <iostream>

#include <bitset>
using namespace Boolean;

int main() {
	Function f(5, Function::Generation::Randomly);
	Spectrum s(f);
	for (auto i: s) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
}
