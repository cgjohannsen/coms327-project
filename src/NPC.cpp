#include <cstdint>
#include <cstdio>

#include <unistd.h>
#include <ncurses.h>

#include "NPC.h"
#include "Dungeon.h"
#include "Character.h"
#include "Pathfinder.h"

NPC::NPC(int x, int y, int seed)
{ 
  char symbols[] = "0123456789abcdef";
  srand(time(NULL) + (seed*17));

  // (n->NPC)->characteristics = rand() % 16;

  //*****************************************
  int r = rand()%3;
  if(r == 0){ this->characteristics = 1; }
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
  this->last_seen_x = x;
  this->last_seen_y = y;
}

NPC::~NPC()
{
  
}

int NPC::move(Dungeon &d)
{ 

  switch(this->characteristics){
    case 0:
      move00(d);
      break;
    case 1:
      move01(d);
      break;
    case 2:
    case 3:
      move03(d);
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      move07(d);
      break;
    case 8:
      move08(d);
      break;
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
int NPC::move00(Dungeon &d)
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
int NPC::move01(Dungeon &d)
{
  //int *p = this->next_pos(this->x, this->y, d.player.x, d.player.y);

  int next_x_t, next_y_t;

  if(this->x < d.player.x){ next_x_t = (this->x)+1; }
  else if(this->x < d.player.x){ next_x_t = (this->x)-1; }
  else{ next_x_t = this->x; }

  if(this->y < d.player.y){ next_y_t = (this->y)+1; }
  else if(this->y < d.player.y){ next_y_t = (this->y)-1; }
  else{ next_y_t = this->y; }

  //if(d.hardness[next_y_t][next_x_t] == 0){
    this->next_x = next_x_t;
    this->next_y = next_y_t;
  //}

  return 0;
}


/* b0010 => Smart                                     */
int NPC::move02(Dungeon &d)
{
  return 0;
}


/* b0011 => Smart + Telepathic                        */
int NPC::move03(Dungeon &d)
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
int NPC::move04(Dungeon &d)
{
  return 0;
}

/* b0101 => Tunneling + Telepathic                    */
int NPC::move05(Dungeon &d)
{
  return 0;
}

/* b0110 => Tunneling + Smart                         */
int NPC::move06(Dungeon &)
{
  return 0;
}

/* b0111 => Tunneling + Smart + Telepathic            */
int NPC::move07(Dungeon &d)
{
  int up = d.all_pathfinder.path[(this->y)-1][(this->x)  ].cost,
    down = d.all_pathfinder.path[(this->y)+1][(this->x)  ].cost,
    left = d.all_pathfinder.path[(this->y)  ][(this->x)-1].cost,
   right = d.all_pathfinder.path[(this->y)  ][(this->x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    if(d.hardness[(this->y)-1][(this->x)  ] < 86) {
       d.hardness[(this->y)-1][(this->x)  ] = 0;
      if(   d.map[(this->y)-1][(this->x)  ] == Dungeon::ter_wall) {
	    d.map[(this->y)-1][(this->x)  ] = Dungeon::ter_corridor;
      }
      this->next_y = (this->y)-1;
      this->next_x = (this->x)  ;
    } else {
      d.hardness[(this->y)-1][(this->x)  ] -= 85;
    }
  } else if(right <= up && right <= down && right <= left) {
    if(d.hardness[(this->y)  ][(this->x)+1] < 86){
       d.hardness[(this->y)  ][(this->x)+1] = 0;
      if(   d.map[(this->y)  ][(this->x)+1] == Dungeon::ter_wall) {
	    d.map[(this->y)  ][(this->x)+1] = Dungeon::ter_corridor;
      }
      this->next_y = (this->y)  ;
      this->next_x = (this->x)+1;
    } else {
      d.hardness[(this->y)  ][(this->x)+1] -= 85;
    }
  } else if (left <= up && left <= down && left <= right) {
    if( d.hardness[(this->y)  ][(this->x)-1] < 86){
        d.hardness[(this->y)  ][(this->x)-1] = 0;
      if(    d.map[(this->y)  ][(this->x)-1] == Dungeon::ter_wall) {
	     d.map[(this->y)  ][(this->x)-1] = Dungeon::ter_corridor;
          }
      this->next_y = (this->y)  ;
      this->next_x = (this->x)-1;
    } else {
      d.hardness[(this->y)  ][(this->x)-1] -= 85;
    }
  } else if (down <= up && down <= left && down <= right) {
    if( d.hardness[(this->y)+1][(this->x)  ] < 86){
        d.hardness[(this->y)+1][(this->x)  ] = 0;
      if(    d.map[(this->y)+1][(this->x)  ] == Dungeon::ter_wall) {
	     d.map[(this->y)+1][(this->x)  ] = Dungeon::ter_corridor;
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
int NPC::move08(Dungeon &d)
{
  return NPC::move00(d);
}

/* b1001 => Erratic + Telepathic                      */
int NPC::move09(Dungeon &d)
{
  return 0;
}

/* b1010 => Erratic + Smart                           */
int NPC::move10(Dungeon &d)
{
  return 0;
}

/* b1011 => Erratic + Smart + Telepathic              */
int NPC::move11(Dungeon &d)
{
  return 0;
}

/* b1100 => Erratic + Tunneling                       */
int NPC::move12(Dungeon &d)
{
  return 0;
}

/* b1101 => Erratic + Tunneling + Telepathic          */
int NPC::move13(Dungeon &d)
{
  return 0;
}

/* b1110 => Erratic + Tunneling + Smart               */
int NPC::move14(Dungeon &d)
{
  return 0;
}

/* b1111 => Erratic + Tunneling + Smart + Telepathic  */
int NPC::move15(Dungeon &d)
{
  return 0;
}


