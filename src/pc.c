#include <stdlib.h>
#include <ncurses.h>

#include "pc.h"
#include "dungeon.h"

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

int pc_move(dungeon_t *d, int c)
{
  switch(c) {
  case 7:
  case (int) 'y':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x-1;
    d->pc.y = d->pc.y-1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 8:
  case (int) 'k':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x  ;
    d->pc.y = d->pc.y-1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 9:
  case (int) 'u':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x+1;
    d->pc.y = d->pc.y-1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 6:
  case (int) 'l':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x+1;
    d->pc.y = d->pc.y  ;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 3:
  case (int) 'n':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x+1;
    d->pc.y = d->pc.y+1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 2:
  case (int) 'j':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x  ;
    d->pc.y = d->pc.y+1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 1:
  case (int) 'b':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x-1;
    d->pc.y = d->pc.y+1;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case 4:
  case (int) 'h':
    d->characters[d->pc.y][d->pc.y] = NULL;
    d->pc.x = d->pc.x-1;
    d->pc.y = d->pc.y  ;
    d->characters[d->pc.y][d->pc.y] = &d->pc;
    break;
  case (int) '>':
    break;
  case (int) '<':
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
