#include <cstdint>
#include <cstdio>

#include <unistd.h>
#include <ncurses.h>

#include "NPC.h"
#include "Dungeon.h"
#include "Pathfinder.h"
#include "MonsterTemplate.h"

NPC::NPC(int pos_x, int pos_y, int seed)
{ 
  char symbols[] = "0123456789abcdef";
  srand(time(NULL) + (seed*17));

  // (n->NPC)->characteristics = rand() % 16;

  //*****************************************
  int r = rand()%3;
  if(r == 0){ abilities = 1; }
  else if(r == 1){ abilities = 3; }
  else { abilities = 7; }
  //*****************************************


  is_pc = 0;
  isAlive = 1;
  speed = (rand() % 16) + 5;
  move_time = 1000/(speed);
  symbol = symbols[abilities];
  hitpoints = 10;
  attack_damage = 10;
  x = pos_x;
  y = pos_y;
  next_x = pos_x;
  next_y = pos_y;
  last_seen_x = pos_x;
  last_seen_y = pos_y;
}


NPC::NPC(MonsterTemplate &temp, int pos_x, int pos_y)
{
  is_pc = 0;
  isAlive = 1;
  x = pos_x;
  y = pos_y;
  next_x = pos_x;
  next_y = pos_y;
  last_seen_x = pos_x;
  last_seen_y = pos_y;

  speed = temp.speed.roll();
  move_time = 1000/(speed);
  symbol = temp.symbol;
  color = temp.color.at(0);
  abilities = temp.abilities;
  hitpoints = temp.hitpoints.roll();
  attack_damage = temp.attack_damage.roll();
  symbol = temp.symbol;
  template_index = temp.index;
}


int NPC::move(Dungeon &d)
{ 
  switch(abilities){
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

  if(d.characters[next_y][next_x] &&
     !(next_x == x && next_y == y)) {
    if(d.characters[next_y][next_x]->symbol != '@') {
      d.characters[next_y][next_x]->isAlive = 0;
      d.nummon--;
       
      if(d.monster_templates
        .at(d.characters[next_y][next_x]->template_index).unique){
        d.monster_templates
        .at(d.characters[next_y][next_x]->template_index).isValid = false;
      }

      d.characters[next_y][next_x] = NULL;
    } else {
      d.player.isAlive = 0;
    }
  } else {
    d.characters[y][x] = NULL;
    x = next_x;
    y = next_y;
    d.characters[y][x] = this;
  }
    
  return 0;
}

/* b0000 => Erratic                                  */
int NPC::move00(Dungeon &d)
{
  srand(time(NULL));
  uint8_t temp_x = x;
  uint8_t temp_y = y;
    
  do{
    next_x = temp_x;
    next_y = temp_y;
    if(rand() % 2){
      if(rand() % 2){
        next_y = (next_y)+1;
        next_x = (next_x)  ;
      }else{
        next_y = (next_y)-1;
        next_x = (next_x)  ;
      }
    }else{
      if(rand() % 2){
        next_y = (next_y)  ;
        next_x = (next_x)+1;
      }else{
        next_y = (next_y)  ;
        next_x = (next_x)-1;
      }
    }
  }while(d.hardness[next_y][next_x] != 0);

  return 0;
}

/* b0001 => Telepathic                                */
int NPC::move01(Dungeon &d)
{
  if(x < d.player.x){ next_x = (x)+1; }
  else if(x < d.player.x){ next_x = (x)-1; }
  else{ next_x = x; }

  if(y < d.player.y){ next_y = (y)+1; }
  else if(y < d.player.y){ next_y = (y)-1; }
  else{ next_y = y; }

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
  int up = d.floor_pathfinder.path[(y)-1][(x)  ].cost,
    down = d.floor_pathfinder.path[(y)+1][(x)  ].cost,
    left = d.floor_pathfinder.path[(y)  ][(x)-1].cost,
   right = d.floor_pathfinder.path[(y)  ][(x)+1].cost;
  
       if(up <= down && up <= left && up <= right) {
    next_y = (y)-1;
    next_x = (x)  ;
  }
  else if(right <= up && right <= down && right <= left) {
    next_y = (y)  ;
    next_x = (x)+1;
  }
  else if(left <= up && left <= down && left <= right) {
    next_y = (y)  ;
    next_x = (x)-1;
  }
  else if(down <= up && down <= left && down <= right) {
    next_y = (y)+1;
    next_x = (x)  ;
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
  int up = d.all_pathfinder.path[(y)-1][(x)  ].cost,
    down = d.all_pathfinder.path[(y)+1][(x)  ].cost,
    left = d.all_pathfinder.path[(y)  ][(x)-1].cost,
   right = d.all_pathfinder.path[(y)  ][(x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    if(d.hardness[(y)-1][(x)  ] < 86) {
       d.hardness[(y)-1][(x)  ] = 0;
      if(   d.map[(y)-1][(x)  ] == Dungeon::ter_wall) {
	    d.map[(y)-1][(x)  ] = Dungeon::ter_corridor;
      }
      next_y = (y)-1;
      next_x = (x)  ;
    } else {
      d.hardness[(y)-1][(x)  ] -= 85;
    }
  } else if(right <= up && right <= down && right <= left) {
    if(d.hardness[(y)  ][(x)+1] < 86){
       d.hardness[(y)  ][(x)+1] = 0;
      if(   d.map[(y)  ][(x)+1] == Dungeon::ter_wall) {
	    d.map[(y)  ][(x)+1] = Dungeon::ter_corridor;
      }
      next_y = (y)  ;
      next_x = (x)+1;
    } else {
      d.hardness[(y)  ][(x)+1] -= 85;
    }
  } else if (left <= up && left <= down && left <= right) {
    if( d.hardness[(y)  ][(x)-1] < 86){
        d.hardness[(y)  ][(x)-1] = 0;
      if(    d.map[(y)  ][(x)-1] == Dungeon::ter_wall) {
	     d.map[(y)  ][(x)-1] = Dungeon::ter_corridor;
          }
      next_y = (y)  ;
      next_x = (x)-1;
    } else {
      d.hardness[(y)  ][(x)-1] -= 85;
    }
  } else if (down <= up && down <= left && down <= right) {
    if( d.hardness[(y)+1][(x)  ] < 86){
        d.hardness[(y)+1][(x)  ] = 0;
      if(    d.map[(y)+1][(x)  ] == Dungeon::ter_wall) {
	     d.map[(y)+1][(x)  ] = Dungeon::ter_corridor;
          }
      next_y = (y)+1;
      next_x = (x)  ;
    } else {
      d.hardness[(y)+1][(x)  ] -= 85;
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


