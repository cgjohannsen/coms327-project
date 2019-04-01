#include <cstdint>
#include <cstdio>

#include <unistd.h>
#include <ncurses.h>

#include "npc.h"
#include "dungeon.h"
#include "character.h"
#include "pathfinder.h"

/*
# define NPC_SMART   0x00000001 // = b00000001
# define NPC_TELE    0x00000002 // = b00000010
# define NPC_TUNNEL  0x00000004 // = b00000100
# define NPC_ERRATIC 0x00000008 // = b00001000

# define has_characterstic(character, bit) \
     (character.characterstics & NPC_##bit)
*/

/*
static int (*moves[])(dungeon &) = {
  npc::npc_move00,
  npc::npc_move01,
  npc::npc_move02,
  npc::npc_move03,
  npc::npc_move04,
  npc::npc_move05,
  npc::npc_move06,
  npc::npc_move07,
  npc::npc_move08,
  npc::npc_move09,
  npc::npc_move10,
  npc::npc_move11,
  npc::npc_move12,
  npc::npc_move13,
  npc::npc_move14,
  npc::npc_move15
};
*/

npc::npc(int x, int y, int seed)
{ 
  char symbols[] = "0123456789abcdef";
  srand(time(NULL) + (seed*17));

  // (n->npc)->characteristics = rand() % 16;

  //*****************************************
  int r = rand()%3;
  if(r == 0){ this->characteristics = 0; }
  else if(r == 1){ this->characteristics = 3; }
  else { this->characteristics = 7; }
  //*****************************************

  this->is_pc = 0;
  this->isAlive = 1;
  this->speed = (rand() % 16) + 5;
  this->move_time = 1000/(this->speed);
  this->symbol = symbols[this->characteristics];
  this->x = x;
  this->y = y;
  this->next_x = x;
  this->next_y = y;
}

npc::~npc()
{
  
}

int npc::npc_move(dungeon &d)
{ 

  switch(this->characteristics){
    case 0:
      npc_move00(d);
      break;
    case 1:
    case 2:
    case 3:
      npc_move03(d);
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      npc_move07(d);
      break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      break;
  };

  if(d.characters[this->next_y][this->next_x] &&
     !(this->next_x == this->x && this->next_y == this->y)) {
    if(d.characters[this->next_y][this->next_x]->symbol != '@') {
       d.characters[this->next_y][this->next_x]->isAlive = 0;
       d.nummon--;
       d.characters[this->next_y][this->next_x] = NULL;
    } else {
      d.player.isAlive = 0;
    }
  } else {
    d.characters[this->y][this->x] = NULL;
    this->x = this->next_x;
    this->y = this->next_y;
    d.characters[this->y][this->x] = this;
  }
    
  return 0;
}

/* b0000 => Erratic                                  */
int npc::npc_move00(dungeon &d)
{
  srand(time(NULL));
  uint8_t temp_x = this->x;
  uint8_t temp_y = this->y;
    
  do{
    this->next_x = temp_x;
    this->next_y = temp_y;
    if(rand() % 2){
      if(rand() % 2){
        this->next_y = (this->next_y)+1;
        this->next_x = (this->next_x)  ;
      }else{
        this->next_y = (this->next_y)-1;
        this->next_x = (this->next_x)  ;
      }
    }else{
      if(rand() % 2){
        this->next_y = (this->next_y)  ;
        this->next_x = (this->next_x)+1;
      }else{
        this->next_y = (this->next_y)  ;
        this->next_x = (this->next_x)-1;
      }
    }
  }while(d.hardness[this->next_y][this->next_x] != 0);

  return 0;
}

/* b0001 => Telepathic                                */
int npc::npc_move01(dungeon &d)
{
  return 0;
}


/* b0010 => Smart                                     */
int npc::npc_move02(dungeon &d)
{
  return 0;
}


/* b0011 => Smart + Telepathic                        */
int npc::npc_move03(dungeon &d)
{
  int up = d.floor_pathfinder.path[(this->y)-1][(this->x)  ].cost,
    down = d.floor_pathfinder.path[(this->y)+1][(this->x)  ].cost,
    left = d.floor_pathfinder.path[(this->y)  ][(this->x)-1].cost,
   right = d.floor_pathfinder.path[(this->y)  ][(this->x)+1].cost;
  
       if(up <= down && up <= left && up <= right) {
    this->next_y = (this->y)-1;
    this->next_x = (this->x)  ;
  }
  else if(right <= up && right <= down && right <= left) {
    this->next_y = (this->y)  ;
    this->next_x = (this->x)+1;
  }
  else if(left <= up && left <= down && left <= right) {
    this->next_y = (this->y)  ;
    this->next_x = (this->x)-1;
  }
  else if(down <= up && down <= left && down <= right) {
    this->next_y = (this->y)+1;
    this->next_x = (this->x)  ;
  }
  
  return 0;
}

/* b0100 => Tunneling                                 */
int npc::npc_move04(dungeon &d)
{
  return 0;
}

/* b0101 => Tunneling + Telepathic                    */
int npc::npc_move05(dungeon &d)
{
  return 0;
}

/* b0110 => Tunneling + Smart                         */
int npc::npc_move06(dungeon &)
{
  return 0;
}

/* b0111 => Tunneling + Smart + Telepathic            */
int npc::npc_move07(dungeon &d)
{
  int up = d.all_pathfinder.path[(this->y)-1][(this->x)  ].cost,
    down = d.all_pathfinder.path[(this->y)+1][(this->x)  ].cost,
    left = d.all_pathfinder.path[(this->y)  ][(this->x)-1].cost,
   right = d.all_pathfinder.path[(this->y)  ][(this->x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    if(d.hardness[(this->y)-1][(this->x)  ] < 86) {
       d.hardness[(this->y)-1][(this->x)  ] = 0;
      if(   d.map[(this->y)-1][(this->x)  ] == dungeon::ter_wall) {
	    d.map[(this->y)-1][(this->x)  ] = dungeon::ter_corridor;
      }
      this->next_y = (this->y)-1;
      this->next_x = (this->x)  ;
    } else {
      d.hardness[(this->y)-1][(this->x)  ] -= 85;
    }
  } else if(right <= up && right <= down && right <= left) {
    if(d.hardness[(this->y)  ][(this->x)+1] < 86){
       d.hardness[(this->y)  ][(this->x)+1] = 0;
      if(   d.map[(this->y)  ][(this->x)+1] == dungeon::ter_wall) {
	    d.map[(this->y)  ][(this->x)+1] = dungeon::ter_corridor;
      }
      this->next_y = (this->y)  ;
      this->next_x = (this->x)+1;
    } else {
      d.hardness[(this->y)  ][(this->x)+1] -= 85;
    }
  } else if (left <= up && left <= down && left <= right) {
    if( d.hardness[(this->y)  ][(this->x)-1] < 86){
        d.hardness[(this->y)  ][(this->x)-1] = 0;
      if(    d.map[(this->y)  ][(this->x)-1] == dungeon::ter_wall) {
	     d.map[(this->y)  ][(this->x)-1] = dungeon::ter_corridor;
          }
      this->next_y = (this->y)  ;
      this->next_x = (this->x)-1;
    } else {
      d.hardness[(this->y)  ][(this->x)-1] -= 85;
    }
  } else if (down <= up && down <= left && down <= right) {
    if( d.hardness[(this->y)+1][(this->x)  ] < 86){
        d.hardness[(this->y)+1][(this->x)  ] = 0;
      if(    d.map[(this->y)+1][(this->x)  ] == dungeon::ter_wall) {
	     d.map[(this->y)+1][(this->x)  ] = dungeon::ter_corridor;
          }
      this->next_y = (this->y)+1;
      this->next_x = (this->x)  ;
    } else {
      d.hardness[(this->y)+1][(this->x)  ] -= 85;
    }
  }
  
  return 0;
}

/* b1000 => Erratic                                   */
int npc::npc_move08(dungeon &d)
{
  return npc::npc_move00(d);
}

/* b1001 => Erratic + Telepathic                      */
int npc::npc_move09(dungeon &d)
{
  return 0;
}

/* b1010 => Erratic + Smart                           */
int npc::npc_move10(dungeon &d)
{
  return 0;
}

/* b1011 => Erratic + Smart + Telepathic              */
int npc::npc_move11(dungeon &d)
{
  return 0;
}

/* b1100 => Erratic + Tunneling                       */
int npc::npc_move12(dungeon &d)
{
  return 0;
}

/* b1101 => Erratic + Tunneling + Telepathic          */
int npc::npc_move13(dungeon &d)
{
  return 0;
}

/* b1110 => Erratic + Tunneling + Smart               */
int npc::npc_move14(dungeon &d)
{
  return 0;
}

/* b1111 => Erratic + Tunneling + Smart + Telepathic  */
int npc::npc_move15(dungeon &d)
{
  return 0;
}


