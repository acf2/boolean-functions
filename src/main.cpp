#include "bfunc.hpp"
#include <iostream>

#include <bitset>
using namespace Boolean;

int main() {
	size_t comb = initial_combination(5, 2);
	std::cout << std::bitset<5>(comb) << std::endl;
	while (!is_last_combination(comb)) {
		comb = next_combination(comb);
		std::cout << std::bitset<5>(comb) << std::endl;
	}
}
