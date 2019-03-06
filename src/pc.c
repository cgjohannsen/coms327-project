#include <stdlib.h>
#include <ncurses.h>

#include "pc.h"
#include "dungeon.h"
#include "heap.h"

int pc_init(dungeon_t *d)
{
  d->pc.pc = calloc(1, sizeof(pc_t));
  d->pc.npc = NULL;
  d->pc.isAlive = 1;
  d->pc.speed = 10;
  d->pc.move_time = 1000/(d->pc.speed);
  d->pc.symbol = '@';

  d->pc.x = d->rooms[0].x;
  d->pc.y = d->rooms[0].y;
  d->characters[d->pc.y][d->pc.x] = &d->pc;
  
  return 0;
}

void pc_delete(pc_t *pc)
{
  if(pc){ free(pc); }
}

int pc_move(dungeon_t *d, int c, heap_t *h)
{
  switch(c) {
  case (int) '7':
  case (int) 'y':
    if(d->hardness[d->pc.y-1][d->pc.x-1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x-1;
      d->pc.y = d->pc.y-1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '8':
  case (int) 'k':
    if(d->hardness[d->pc.y-1][d->pc.x  ] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x  ;
      d->pc.y = d->pc.y-1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '9':
  case (int) 'u':
    if(d->hardness[d->pc.y-1][d->pc.x+1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x+1;
      d->pc.y = d->pc.y-1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '6':
  case (int) 'l':
    if(d->hardness[d->pc.y  ][d->pc.x+1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x+1;
      d->pc.y = d->pc.y  ;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '3':
  case (int) 'n':
    if(d->hardness[d->pc.y+1][d->pc.x+1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x+1;
      d->pc.y = d->pc.y+1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '2':
  case (int) 'j':
    if(d->hardness[d->pc.y+1][d->pc.x] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x  ;
      d->pc.y = d->pc.y+1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '1':
  case (int) 'b':
    if(d->hardness[d->pc.y+1][d->pc.x-1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x-1;
      d->pc.y = d->pc.y+1;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '4':
  case (int) 'h':
    if(d->hardness[d->pc.y  ][d->pc.x-1] == 0) {
      d->characters[d->pc.y][d->pc.x] = NULL;
      d->pc.x = d->pc.x-1;
      d->pc.y = d->pc.y  ;
      d->characters[d->pc.y][d->pc.x] = &d->pc;
    } else {
      d->message = "There's a wall in the way!";
    }
    break;
  case (int) '>':
    if(d->map[d->pc.y][d->pc.x] == ter_stair_up) {
      clear();
      clear_dungeon(d);
      gen_dungeon(d);
      d->pc.y = d->rooms[0].y;
      d->pc.x = d->rooms[0].x;
      place_characters(d, h);
    } else {
      d->message = "There is no set of upstairs here!";
    }
    break;
  case (int) '<':
    if(d->map[d->pc.y][d->pc.x] == ter_stair_down) {
      clear();
      clear_dungeon(d);
      gen_dungeon(d);
      d->pc.y = d->rooms[0].y;
      d->pc.x = d->rooms[0].x;
      place_characters(d, h);
    } else {
      d->message = "There is no set of downstairs here!";
    }
    break;
  case (int) 'm':
    break;
  case KEY_UP:
    break;
  case KEY_DOWN:
    break;
  case 27: // Esacpe
    break;
  case (int) 'Q':
    return 1;
    break;
  }

  return 0;
}
