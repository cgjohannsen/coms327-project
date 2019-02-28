#include <time.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "dungeon.h"
#include "character.h"

#define DUNGEON_Y 21
#define DUNGEON_X 80

static int (*moves[16])(character_t *c, dungeon_t *d) = {
  character_stupid,
  character_smart,
  character_tele,
  character_smart_tele,
  character_tunnel,
  character_tunnel_smart,
  character_tunnel_tele,
  character_tunnel_tele_smart,
  character_erratic,
  character_erratic_smart,
  character_erratic_tele,
  character_erratic_tele_smart,
  character_erratic_tunnel,
  character_erratic_tunnel_smart,
  character_erratic_tunnel_tele,
  character_erratic_tunnel_tele_smart
};

int character_init_pc(character_t *c, dungeon_t *d)
{ 
  c->isPC      = 1;
  c->isAlive   = 1;
  c->speed     = 15;
  c->x         = d->rooms[0].x;
  c->y         = d->rooms[0].y;
  c->symbol    = '@';
  c->move_time = 1000/(c->speed);
  c->id        = 0;
  
  return 0;
}

int character_init_monster(character_t *c, int n, character_t monsters[n],
			   dungeon_t *d, int index)
{
  char symbols[] = "0123456789abcdef";
  srand(index + time(NULL));

  // c->characteristics = rand() % 16;
  int r = rand() % 3;
  if(r == 0){
    c->characteristics = 0;
  }else if(r == 1){
    c->characteristics = 3; 
  }else if(r == 2){
    c->characteristics = 7;
  }
    
  c->isPC      = 0;
  c->isAlive   = 1;
  c->id        = index+1;
  c->speed     = (rand() % 16) + 5; 
  c->symbol    = symbols[c->characteristics];
  c->move_time = 1000/(c->speed);

  int invalid = 1;
  do{
    room_t room = d->rooms[rand()%d->num_rooms];
    c->x = room.x + rand()%room.width;
    c->y = room.y + rand()%room.height;

    for(r = 0; r < n; r++) {
      if(!(monsters[r].x == c->x && monsters[r].y == c->y)){ invalid = 0; }
    }
    
  }while(invalid);

  return 0;
}

int character_render(dungeon_t *d, character_t *pc,
		     int n, character_t monsters[n])
{
  int i;
  
  // Render PC
  d->char_arr[pc->y][pc->x] = '@';

  // Render monsters
  for(i = 0; i < n; i++){
    if(monsters[i].isAlive) {
      d->char_arr[monsters[i].y][monsters[i].x] = monsters[i].symbol;
    }
  }

  return 0;
}

int character_move(character_t *c, dungeon_t *d, character_t *pc,
		   int n, character_t monsters[n])
{
  int i;
  moves[c->characteristics](c, d);

  if(c->x == pc->x && c->y == pc->y && c->id != pc->id)
    { pc->isAlive = 0; }
  
  for(i = 0; i < n; i++) {
    if(monsters[i].x == c->x && monsters[i].y == c->y &&
       monsters[i].id != c->id)
      { monsters[i].isAlive = 0; }
  }
  
  
  return 0;
}

int character_stupid(character_t *c, dungeon_t *d)
{
  srand(time(NULL));
  uint8_t temp_x = c->x;
  uint8_t temp_y = c->y;
    
  do{
    c->x = temp_x;
    c->y = temp_y;
    if(rand() % 2){
      if(rand() % 2){
	c->y = (c->y)+1;
	c->x = (c->x)  ;
      }else{
        c->y = (c->y)-1;
	c->x = (c->x)  ;
      }
    }else{
      if(rand() % 2){
        c->y = (c->y)  ;
	c->x = (c->x)+1;
      }else{
        c->y = (c->y)  ;
	c->x = (c->x)-1;
      }
    }
  }while(d->hardness[c->y][c->x] != 0);

  return 0;
}

int character_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_tele(character_t *c, dungeon_t *d)
{
  return 0;
}

// Implement for 1.04
int character_smart_tele(character_t *c, dungeon_t *d)
{
  int up = d->floor_dis[(c->y)-1][(c->x)  ].cost,
    down = d->floor_dis[(c->y)+1][(c->x)  ].cost,
    left = d->floor_dis[(c->y)  ][(c->x)-1].cost,
   right = d->floor_dis[(c->y)  ][(c->x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    c->y = (c->y)-1;
    c->x = (c->x)  ;
  }
  if(right <= up && right <= down && right <= left) {
    c->y = (c->y)  ;
    c->x = (c->x)+1;
  }
  if(left <= up && left <= down && left <= right) {
    c->y = (c->y)  ;
    c->x = (c->x)-1;
  }
  if(down <= up && down <= left && down <= right) {
    c->y = (c->y)+1;
    c->x = (c->x)  ;
  }
  
  return 0;
}

int character_tunnel(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_tunnel_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_tunnel_tele(character_t *c, dungeon_t *d)
{
  return 0;
}

// Implement for 1.04
int character_tunnel_tele_smart(character_t *c, dungeon_t *d)
{
  int up = d->all_dis[(c->y)-1][(c->x)  ].cost,
    down = d->all_dis[(c->y)+1][(c->x)  ].cost,
    left = d->all_dis[(c->y)  ][(c->x)-1].cost,
   right = d->all_dis[(c->y)  ][(c->x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    if(d->hardness[(c->y)-1][(c->x)  ] < 86){
      d->hardness[(c->y)-1][(c->x)  ] = 0;
      c->y = (c->y)-1;
      c->x = (c->x)  ;
    } else {
      d->hardness[(c->y)-1][(c->x)  ] -= 85;
    }
    
  } else if(right <= up && right <= down && right <= left) {
    if(d->hardness[(c->y)  ][(c->x)+1] < 86){
      d->hardness[(c->y)  ][(c->x)+1] = 0;
      c->y = (c->y)  ;
      c->x = (c->x)+1;
    } else {
      d->hardness[(c->y)  ][(c->x)+1] -= 85;
    }
    
  } else if (left <= up && left <= down && left <= right) {
    if(d->hardness[(c->y)  ][(c->x)-1] < 86){
      d->hardness[(c->y)  ][(c->x)-1] = 0;
      c->y = (c->y)  ;
      c->x = (c->x)-1;
    } else {
      d->hardness[(c->y)  ][(c->x)-1] -= 85;
    }
    
  } else if (down <= up && down <= left && down <= right) {
    if(d->hardness[(c->y)+1][(c->x)  ] < 86){
      d->hardness[(c->y)+1][(c->x)  ] = 0;
      c->y = (c->y)+1;
      c->x = (c->x)  ;
    } else {
      d->hardness[(c->y)+1][(c->x)  ] -= 85;
    }
  }
  
  return 0;
}

int character_erratic(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tele(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tele_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tunnel(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tunnel_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tunnel_tele(character_t *c, dungeon_t *d)
{
  return 0;
}

int character_erratic_tunnel_tele_smart(character_t *c, dungeon_t *d)
{
  return 0;
}

