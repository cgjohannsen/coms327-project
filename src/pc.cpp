#include <cstdlib>
#include <cstdint>

#include <unistd.h>
#include <ncurses.h>

#include "pc.h"
#include "dungeon.h"
#include "heap.h"

pc::pc() {}

int pc::init(uint8_t x_in, uint8_t y_in)
{
  this->is_pc = 1;
  this->isAlive = 1;
  this->speed = 10;
  this->move_time = 1000/(this->speed);
  this->symbol = '@';

  this->x = x_in;
  this->y = y_in;
  this->next_x = x_in;
  this->next_y = y_in;

  return 0;
}

int pc::pc_move(dungeon &d, int c, heap_t *h)
{
  switch(c) {
  case (int) '7':
  case (int) 'y':
    if(d.hardness[this->y-1][this->x-1] == 0) {
      d.characters[this->y][this->x] = NULL;
      this->x = this->x-1;
      this->y = this->y-1;
      d.characters[this->y][this->x] = this;
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '8':
  case (int) 'k':
    if(d.hardness[this->y-1][this->x  ] == 0) {
      if(d.characters[this->y-1][this->x  ]) {
        d.characters[this->y-1][this->x  ]->isAlive = 0;
        d.nummon--;
        d.characters[this->y-1][this->x  ] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x  ;
        this->y = this->y-1;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '9':
  case (int) 'u':
    if(d.hardness[this->y-1][this->x+1] == 0) {
      if(d.characters[this->y-1][this->x+1]) {
        d.characters[this->y-1][this->x+1]->isAlive = 0; 
        d.nummon--;
        d.characters[this->y-1][this->x+1] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x+1;
        this->y = this->y-1;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '6':
  case (int) 'l':
    if(d.hardness[this->y  ][this->x+1] == 0) {
      if(d.characters[this->y  ][this->x+1]) {
        d.characters[this->y  ][this->x+1]->isAlive = 0;
        d.nummon--;
        d.characters[this->y  ][this->x+1] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x+1;
        this->y = this->y  ;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '3':
  case (int) 'n':
    if(d.hardness[this->y+1][this->x+1] == 0) {
      if(d.characters[this->y+1][this->x+1]) {
        d.characters[this->y+1][this->x+1]->isAlive = 0;
        d.nummon--;
        d.characters[this->y+1][this->x+1] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x+1;
        this->y = this->y+1;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '2':
  case (int) 'j':
    if(d.hardness[this->y+1][this->x  ] == 0) {
      if(d.characters[this->y+1][this->x  ]) {
        d.characters[this->y+1][this->x  ]->isAlive = 0;
        d.nummon--;
        d.characters[this->y+1][this->x  ] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x  ;
        this->y = this->y+1;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '1':
  case (int) 'b':
    if(d.hardness[this->y+1][this->x-1] == 0) {
      if(d.characters[this->y+1][this->x-1]) {
        d.characters[this->y+1][this->x-1]->isAlive = 0;
        d.nummon--;
        d.characters[this->y+1][this->x-1] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x  ;
        this->y = this->y+1;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '4':
  case (int) 'h':
    if(d.hardness[this->y  ][this->x-1] == 0) {
      if(d.characters[this->y  ][this->x-1]) {
        d.characters[this->y  ][this->x-1]->isAlive = 0;
        d.nummon--;
        d.characters[this->y  ][this->x-1] = NULL;
      } else {
        d.characters[this->y][this->x] = NULL;
        this->x = this->x-1;
        this->y = this->y  ;
        d.characters[this->y][this->x] = this;
      }
    } else {
      d.message = "There's a wall in the way!";
    }
    break;
  case (int) '>':
    if(d.map[this->y][this->x] == dungeon::ter_stair_up) {
      clear();
      d.clear_dungeon();
      d.gen_dungeon();
      this->y = d.rooms[0].y;
      this->x = d.rooms[0].x;
      d.place_characters(h);
    } else {
      d.message = "There is no set of upstairs here!";
    }
    break;
  case (int) '<':
    if(d.map[this->y][this->x] == dungeon::ter_stair_down) {
      clear();
      d.clear_dungeon();
      d.gen_dungeon();
      this->y = d.rooms[0].y;
      this->x = d.rooms[0].x;
      d.place_characters(h);
    } else {
      d.message = "There is no set of downstairs here!";
    }
    break;
  case (int) '5':
  case (int) '.':
  case (int) ' ':
    d.message = "The PC bides his (or her) time...";
    break;
  case (int) 'Q':
    return 1;
    break;
  }

  return 0;
}