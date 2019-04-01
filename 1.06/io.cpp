#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <ncurses.h>

#include "io.h"
#include "dungeon.h"

io::io(){}

int io::display(int cmd, dungeon &d)
{
  switch(cmd){
    case DISPLAY_ALL_CMD:
      display_all(d);
      break;
    case DISPLAY_MAP_CMD:
      display_map(d);
      break;
    case DISPLAY_MONSTERS_CMD:
      display_monsters(d);
      break;
    case DISPLAY_TELEPORT_CMD:
      display_teleport(d);
      break;
  }

  return 0;
}

int io::display_all(dungeon &d)
{
	uint8_t r, c;

	mvprintw(0, 0, d.message.c_str());
	for(r = 1; r < DUNGEON_Y+1; r++) {
		for(c = 0; c < DUNGEON_X; c++) {
      if(d.characters[r-1][c]){ mvaddch(r, c, d.characters[r-1][c]->symbol); }
      else{
      switch(d.map[r-1][c]) {
        case dungeon::ter_wall:
        case dungeon::ter_unknown:
        case dungeon::ter_immutable:
          mvaddch(r, c, ' ');
          break;
        case dungeon::ter_floor:
          mvaddch(r, c, '.');
          break;
        case dungeon::ter_corridor:
          mvaddch(r, c, '#');
          break;
        case dungeon::ter_stair_up:
          mvaddch(r, c, '>');
          break;
        case dungeon::ter_stair_down:
          mvaddch(r, c, '<');
          break;
        }
      }
		}      
	}

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
         abs(dis_x), dir_x.c_str(), abs(dis_y), dir_y.c_str());
      }
    for(; r < DUNGEON_Y+1; r++) {
      mvprintw(r, 25, "                                ");
    }
    cmd = getch();
  } while(cmd != 27); // Escape key
  
  return 0;
}

int io::display_teleport(dungeon &d)
{
  uint8_t cmd = ' ', tx = d.player.x, ty = d.player.y;
  srand(time(NULL));

  mvprintw(0, 0, d.message.c_str());
  do{
    display_all(d);
    mvaddch(ty+1, tx, '*');

    cmd = getch();
    switch(cmd){
      case 'r':
        do{
          ty = (rand() % (DUNGEON_Y-1)) + 1;
          tx = (rand() % (DUNGEON_X-1)) + 1;
        }while(d.hardness[ty][tx]);
        break;
      case '7':
      case 'y':
        if(tx-1 > 0 && ty-1 > 0){
          tx--;
          ty--;
        }
        break;
      case '8':
      case 'k':
        if(ty-1 > 0){
          ty--;
        }
        break;
      case '9':
      case 'u':
        if(tx+1 < DUNGEON_X && ty-1 > 0){
          tx++;
          ty--;
        }
        break;
      case '6':
      case 'l':
        if(tx+1 < DUNGEON_X){
          tx++;
        }
        break;
      case '3':
      case 'n':
        if(tx+1 < DUNGEON_X && ty+1 < DUNGEON_Y){
          tx++;
          ty++;
        }
        break;
      case '2':
      case 'j':
        if(ty+1 < DUNGEON_Y){
          ty++;
        }
        break;
      case '1':
      case 'b':
        if(tx-1 > 0 && ty+1 < DUNGEON_Y){
          tx--;
          ty++;
        }
        break;
      case '4':
      case 'h':
        if(tx-1 > 0){
          tx--;
        }
        break;
    }

  }while(cmd != 't' && cmd != 'r');

  d.characters[d.player.y][d.player.x] = NULL;
  d.characters[ty][tx] = &(d.player);
  d.player.y = ty;
  d.player.x = tx;

  return 0;
}