#include <string>
#include <cstdint>
#include <ncurses.h>

#include "Move.h"

#include "Dungeon.h"
#include "Character.h"
#include "PC.h"
#include "NPC.h"

int move_npc(Dungeon &d, NPC &c)
{
	// TODO

	c.move(d);
	
	if(d.characters[c.next_y][c.next_x] && 
		!(c.next_x == c.x && c.next_y == c.y) &&
		d.characters[c.next_y][c.next_x]->symbol == '@'){
		combat(d, c, d.player);	
	} else {
		if(d.characters[c.next_y][c.next_x] && 
		!(c.next_x == c.x && c.next_y == c.y)){
			/*
			int r[] = {c.next_y-1, c.next_y+1, c.next_y};
			int c[] = {c.next_x-1, c.next_x+1, c.next_x};
			int rand_r, rand_c;
			do{
				rand_r = rand()%3;
				rand_c = rand()%3;
			}while((rand_r == 2 && rand_c == 2) || 
				d.hardness[rand_r][rand_c] != 0 || 
				d.characters[rand_r][rand_c]);
			Character *temp = d.characters[rand_r][rand_c];
			d.characters[rand_r][rand_c] = 
			*/
		}
		d.characters[c.y][c.x] = NULL;
    	c.x = c.next_x;
    	c.y = c.next_y;
    	d.characters[c.y][c.x] = &c;
	}

	return 0;
}

int move_pc(Dungeon &d, heap_t *h, int cmd)
{
	int t = d.player.move(d, cmd);

	if(t == 1) {
		clear();
    	d.clear();
    	d.generate();
    	d.player.y = d.rooms[0].y;
    	d.player.x = d.rooms[0].x;
    	d.place_objects();
    	d.place_characters(h);
    	return t;
	} else if(t == 0) {
		if(d.hardness[d.player.next_y][d.player.next_x] == 0){
			if(d.characters[d.player.next_y][d.player.next_x] && 
			  !(d.player.next_x == d.player.x && d.player.next_y == d.player.y)){
				combat(d, d.player, *d.characters[d.player.next_y][d.player.next_x]);
			} else {
				d.characters[d.player.y][d.player.x] = NULL;
    			d.player.x = d.player.next_x;
    			d.player.y = d.player.next_y;
    			d.characters[d.player.y][d.player.x] = &d.player;

    			if(d.objects[d.player.y][d.player.x]) {
    				d.player.pickup(d);
    			}
			}
		} else {
			d.player.next_y = d.player.y;
			d.player.next_x = d.player.x;
			d.message = "There's a wall in the way!";
		}
	} else if(t < 0) {
		return t;
	}

	return 0;
}

int combat(Dungeon &d, Character &attacker, Character &defender)
{
	uint32_t damage = attacker.attack_damage.roll();

	defender.hitpoints -= damage;

	if(defender.hitpoints < 1){
		char buffer[81];
		sprintf(buffer, "%s slays %s", attacker.name.c_str(), 
			defender.name.c_str());
		d.message = buffer;

		if(!defender.is_pc){
			delete(&defender);
		} else {
			d.player.isAlive = 0;
		}

	} else {
		char buffer[81];
		sprintf(buffer, "%s deals %d damage to %s", 
			attacker.name.c_str(), damage, defender.name.c_str());
		d.message = buffer;
	}
	
	return 0;
}