#include <cstdint>

#include <ncurses.h>

#include "io.h"

io::io(){}

int io::display_monsters(dungeon &d)
{
  uint32_t cmd = (int) 'm';
  int r, c, i = 0;

  character *monsters[d.nummon];
  
  // Collect list of monsters
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d.characters[r][c] &&
        d.characters[r][c]->symbol != '@'){
        monsters[i] = d.characters[r][c];
        i++;
      }
    }
  }

  i = 0;
  do{
    //clear();
    mvprintw(1, 25, "--------------------------------");
    mvprintw(2, 25, "|         Monster List         |");
    mvprintw(3, 25, "--------------------------------");
    mvprintw(4, 25, "       Live Monsters = %d       ", d.nummon);
    if(d.nummon > 16) {
      switch(cmd) {
      case KEY_UP:
  if(i > 0)
  { i -= 16; }
  break;
      case KEY_DOWN:
  if(d.nummon - i >= 16)
  { i += 16; }
  break;
      }
    }
    
    for(r = 5; r < DUNGEON_Y+1 && i+(r-5) < d.nummon; r++) {
      std::string dir_x, dir_y;
  
      int dis_x = d.player.x - monsters[i+(r-5)]->x;
      if(dis_x > 0) {
        dir_x = "west";
      } else {
        dir_x = "east";
      }
  
      int dis_y = d.player.y - monsters[i+(r-5)]->y;
      if(dis_y > 0) {
        dir_y = "north";
      } else {
        dir_y = "south";
      }
  
      mvprintw(r, 25, "  %c, %d %s and %d %s      ", monsters[i+(r-5)]->symbol,
         dis_x, dir_x.c_str(), dis_y, dir_y.c_str());
      }
    for(; r < DUNGEON_Y+1; r++) {
      mvprintw(r, 25, "                                ");
    }
    cmd = getch();
  } while(cmd != 27); // Escape key
  
  return 0;
}

int io::display_map(dungeon &d)
{
	uint8_t r, c;

	mvprintw(0, 0, d.message.c_str());
	for(r = 1; r < DUNGEON_Y+1; r++) {
		for(c = 0; c < DUNGEON_X; c++) {
			mvaddch(r, c, d.output[r-1][c]);
		}      
	}

	return 0;
}