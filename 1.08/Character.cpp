#include <cstddef>
#include <cmath>

#include <float.h>

#include "Character.h"
#include "Dungeon.h"

int *Character::next_pos(int x0, int y0, int x1, int y1)
{
	int *pos = (int*)malloc(2 * sizeof(int));

	if(x1 > x0){ pos[0] = x0++; }
	else if(x1 < x0){ pos[0] = x0--; }
	else{ pos[0] = x0; }	

	if(y1 > y0){ pos[1] = y0++; }
	else if(y1 < y0){ pos[1] = y0--; }
	else{ pos[1] = y0; }

	return pos;
}

int Character::can_see(Dungeon &d, Character &viewer, Character &viewee)
{
	int x0 = viewer.x, y0 = viewer.y;
	int x1 = viewee.x, y1 = viewee.y;

	int *p;
	while(x0 != x1 && y0 != y1){
		p = next_pos(x0, y0, x1, y1);
		x0 = p[0];
		y0 = p[1];
		if(d.map[y0][x0] != Dungeon::ter_floor && 
		   d.map[y0][x0] != Dungeon::ter_corridor) {
			return 0;
		}
	}

	return 1;
}