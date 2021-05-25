#pragma once
#include <bitset>
namespace font {
	std::bitset<36> loadChar(uint8_t c); //36 bits instead of 35 for 5x7 font because the rightmost bit indicates if the character is a control char
};