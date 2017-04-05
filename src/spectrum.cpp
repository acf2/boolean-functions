#include "bfunc.hpp"

namespace Boolean {
	Spectrum::Spectrum(Function const& func) : std::vector<int>(func.bitsize()), arguments(func.get_arguments()) {
		using std::swap;

		for (size_t i = 0; i < func.bitsize(); ++i) {
			(*this)[i] = (func(i) != 0 ? -1 : 1);
		}

		int sum, diff;
		for (size_t segment_size = 1; segment_size < (*this).size(); segment_size <<= 1) {
			for (size_t segment = 0; segment < (*this).size() / (segment_size * 2); ++segment) {
				for (size_t cell = 0; cell < segment_size; ++cell) {
					sum = (*this)[segment_size * segment * 2 + cell]
						+ (*this)[segment_size * segment * 2 + segment_size + cell];
					diff = (*this)[segment_size * segment * 2 + cell]
						 - (*this)[segment_size * segment * 2 + segment_size + cell];
					(*this)[segment_size * segment * 2 + cell] = sum;
					(*this)[segment_size * segment * 2 + segment_size + cell] = diff;
				}
			}
		}
	}
}
