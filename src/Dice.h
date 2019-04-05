#ifndef DICE_H
# define DICE_H

# include <cstdint>

class Dice {

private:

	uint32_t base, num, sides;
	char str[22];

public:
	
	int init(const uint32_t &, const uint32_t &, const uint32_t &);
	uint32_t roll();
	char *c_str();

};

#endif