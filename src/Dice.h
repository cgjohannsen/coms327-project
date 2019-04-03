#ifndef DICE_H
# define DICE_H

class Dice {

private:

	int base, num, sides;
	char str[22];

public:
	
	int init(int &, int &, int &);
	int roll();
	char *c_str();

};

#endif