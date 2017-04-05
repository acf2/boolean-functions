#include "bfunc.hpp"

namespace Boolean {
	size_t initial_combination(size_t cardinality, size_t elements) {
		return ((static_cast<size_t>(1) << elements) - 1) << (cardinality - elements);
	}

	size_t next_combination(size_t combination) {
		//XXX: b, c?! Really?!
		size_t b, c;
		b = (combination + 1) & combination;
		c = Boolean::weight((b - 1) ^ combination) - 2;
		return (((((combination + 1) ^ combination) << 1) + 1) << c) ^ b; 
	}

	bool is_last_combination(size_t combination) {
		return ((combination + 1) & combination) == 0;
	}
}
