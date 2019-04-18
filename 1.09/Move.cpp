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
	c.move(d);
	
	if(d.characters[c.next_y][c.next_x] && 
	!(c.next_x == c.x && c.next_y == c.y) &&
		d.characters[c.next_y][c.next_x]->symbol == '@'){
		combat(d, c, d.player);	
	} else if(d.characters[c.next_y][c.next_x] && 
			!(c.next_x == c.x && c.next_y == c.y)){
		uint8_t tr, tc, temp_row = 0, temp_col = 0;
		for(tr = c.next_y-1; tr < c.next_y+2; tr++){
			for(tc = c.next_x-1; tc < c.next_x+2; tc++){
				if(!d.characters[tr][tc] && d.hardness[tr][tc] == 0){
					temp_row = tr;
					temp_col = tc;
				}
			}
		}
		if(temp_row != 0 && temp_col != 0){
			d.characters[temp_row][temp_col] = d.characters[c.next_y][c.next_x];
			d.characters[temp_row][temp_col]->x = temp_col;
			d.characters[temp_row][temp_col]->y = temp_row;
			d.characters[c.y][c.x] = NULL;
    		c.x = c.next_x;
    		c.y = c.next_y;
    		d.characters[c.y][c.x] = &c;
		} else {
			Character *temp = d.characters[c.next_y][c.next_x];
			d.characters[c.next_y][c.next_x] = d.characters[c.y][c.x];
			d.characters[c.y][c.x] = temp;
			temp->x = c.x;
			temp->y = c.y;
			c.x = c.next_x;
    		c.y = c.next_y;
		}
	} else {
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
	} else if(t == 2){
		return t;
	} else if(t == 0) {
		if(d.hardness[d.player.next_y][d.player.next_x] == 0){
			if(d.characters[d.player.next_y][d.player.next_x] && 
			  !(d.player.next_x == d.player.x && d.player.next_y == d.player.y)){
				if((combat(d, d.player, *d.characters[d.player.next_y][d.player.next_x])) == 10)
					return 10;
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
	uint32_t damage = attacker.attack();

	defender.hitpoints -= damage;

	if(defender.hitpoints < 1){
		char buffer[81];
		sprintf(buffer, "%s slays %s", attacker.name.c_str(), 
			defender.name.c_str());
		d.message = buffer;

		if(!defender.is_pc){
			if(((NPC*)d.characters[defender.y][defender.x])->abilities & NPC_BOSS){
				return 10;
			}
			d.characters[defender.y][defender.x]->isAlive = 0;
			d.characters[defender.y][defender.x] = NULL;
			delete(d.characters[defender.y][defender.x]);
			d.nummon--;
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