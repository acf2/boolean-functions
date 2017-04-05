#include "bfunc.hpp"

namespace Boolean {
		std::unordered_map<char, std::string> const Symbols::digits = {{'0',"\u2080"}, {'1', "\u2081"}, {'2', "\u2082"}, {'3', "\u2083"}, {'4', "\u2084"}, {'5', "\u2085"}, {'6', "\u2086"}, {'7', "\u2087"}, {'8', "\u2088"}, {'9', "\u2089"}}; 
		std::string const Symbols::sign = "\u2295";
		std::string const Symbols::variable = "x";
		std::string const Symbols::separator = " ";
}
