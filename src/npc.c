#include <stdint.h>
#include <stdio.h>
#include <ncurses.h>

#include "npc.h"
#include "dungeon.h"
#include "character.h"

static int (*moves[16])(dungeon_t *d, character_t *c) = {
  npc_move00,
  npc_move01,
  npc_move02,
  npc_move03,
  npc_move04,
  npc_move05,
  npc_move06,
  npc_move07,
  npc_move08,
  npc_move09,
  npc_move10,
  npc_move11,
  npc_move12,
  npc_move13,
  npc_move14,
  npc_move15
};

int npc_init(dungeon_t *d, int seed)
{
  character_t *n;
  n = malloc(sizeof(character_t));
  n->npc = malloc(sizeof(npc_t));
  
  
  char symbols[] = "0123456789abcdef";
  srand(time(NULL) + seed);

  // (n->npc)->characteristics = rand() % 16;

  int r = rand()%3;
  if(r == 0){ (n->npc)->characteristics = 0; }
  else if(r == 1){ (n->npc)->characteristics = 3; }
  else { (n->npc)->characteristics = 7; }
  
   n->pc = NULL;
   n->isAlive = 1;
   n->speed = (rand() % 16) + 5;
   n->move_time = 1000/(n->speed);
   n->symbol = symbols[(n->npc)->characteristics];

  do{
    room_t room = d->rooms[rand()%d->num_rooms];
    n->x = room.x + rand()%room.width;
    n->y = room.y + rand()%room.height;    
  }while(d->characters[n->y][n->x]);

  n->next_x = n->x;
  n->next_y = n->y;

  d->characters[n->y][n->x] = n;
  
  return 0;
}

void npc_delete(npc_t *npc)
{
  if(npc){ free(npc); }
}

int display_monsters(dungeon_t  *d, char m)
{
  uint32_t cmd = m;
  int r, c, i = 0;

  character_t *monsters[SCREEN_HEIGHT];

  for(r = 0; r < SCREEN_HEIGHT; r++) {
    monsters[r] = NULL;
  }
  
  // Collect list of monsters
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d->characters[r][c] &&
	 d->characters[r][c]->symbol != '@'){
	monsters[i] = d->characters[r][c];
	i++;
      }
    }
  }

  i = 0;
  do{
    i = 0;
    clear();
    if(d->nummon > 20) {
      switch(cmd) {
      case KEY_UP:
	if(i > 0) { i--; }
	break;
      case KEY_DOWN:
	if(d->nummon - i < SCREEN_HEIGHT) { i++; }
	break;
      }
    }

    
    for(r = 1; r < SCREEN_HEIGHT+1 && monsters[i]; r++, i++) {
      if(monsters[i]) {
	printw(" %d ", i);
	char *dir_x, *dir_y;
	
	int dis_x = d->pc.x - monsters[i]->x;
        if(dis_x > 0) {
	  dir_x = "west";
	} else {
	  dir_x = "east";
	}
	
	int dis_y = d->pc.y - monsters[i]->y;
        if(dis_y > 0) {
	  dir_y = "north";
	} else {
	  dir_y = "south";
	}
	
	mvprintw(r, 0, "%c, %d %s and %d %s", monsters[i]->symbol,
		 dis_x, dir_x, dis_y, dir_y);
      }
    }
    cmd = getch();
  } while(cmd != 27); // Escape key
  
  return 0;
}

int npc_move(dungeon_t *d, character_t *c)
{ 
  moves[(c->npc)->characteristics](d, c);

  if(d->characters[c->next_y][c->next_x] &&
     !(c->next_x == c->x && c->next_y == c->y)) {
    if(d->characters[c->next_y][c->next_x]->symbol != '@') {
       d->characters[c->next_y][c->next_x]->isAlive = 0;
       d->nummon--;

       // DEBUG
       //printf("Bloody murder!\n");
       //printf("Muderer: %p\nMurderee: %p\nLocation: %d, %d\n",
       //d->characters[c->y][c->x],
       //d->characters[c->next_y][c->next_x],
       //c->next_x, c->next_y);
       
       d->characters[c->next_y][c->next_x] = NULL;
    } else {
      d->pc.isAlive = 0;
    }
  } else {
    d->characters[c->y][c->x] = NULL;
    c->x = c->next_x;
    c->y = c->next_y;
    d->characters[c->y][c->x] = c;
  }
    
  return 0;
}

/* b0000 => Erratic                                  */
int npc_move00(dungeon_t *d, character_t *c)
{
  srand(time(NULL));
  uint8_t temp_x = c->x;
  uint8_t temp_y = c->y;
    
  do{
    c->next_x = temp_x;
    c->next_y = temp_y;
    if(rand() % 2){
      if(rand() % 2){
	c->next_y = (c->next_y)+1;
	c->next_x = (c->next_x)  ;
      }else{
        c->next_y = (c->next_y)-1;
	c->next_x = (c->next_x)  ;
      }
    }else{
      if(rand() % 2){
        c->next_y = (c->next_y)  ;
	c->next_x = (c->next_x)+1;
      }else{
        c->next_y = (c->next_y)  ;
	c->next_x = (c->next_x)-1;
      }
    }
  }while(d->hardness[c->next_y][c->next_x] != 0);

  return 0;
}

/* b0001 => Telepathic                                */
int npc_move01(dungeon_t *d, character_t *c)
{
  return 0;
}


/* b0010 => Smart                                     */
int npc_move02(dungeon_t *d, character_t *c)
{
  return 0;
}


/* b0011 => Smart + Telepathic                        */
int npc_move03(dungeon_t *d, character_t *c)
{
  int up = d->pc_cost_floor[(c->y)-1][(c->x)  ].cost,
    down = d->pc_cost_floor[(c->y)+1][(c->x)  ].cost,
    left = d->pc_cost_floor[(c->y)  ][(c->x)-1].cost,
   right = d->pc_cost_floor[(c->y)  ][(c->x)+1].cost;
  
       if(up <= down && up <= left && up <= right) {
    c->next_y = (c->y)-1;
    c->next_x = (c->x)  ;
  }
  else if(right <= up && right <= down && right <= left) {
    c->next_y = (c->y)  ;
    c->next_x = (c->x)+1;
  }
  else if(left <= up && left <= down && left <= right) {
    c->next_y = (c->y)  ;
    c->next_x = (c->x)-1;
  }
  else if(down <= up && down <= left && down <= right) {
    c->next_y = (c->y)+1;
    c->next_x = (c->x)  ;
  }
  
  return 0;
}

/* b0100 => Tunneling                                 */
int npc_move04(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b0101 => Tunneling + Telepathic                    */
int npc_move05(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b0110 => Tunneling + Smart                         */
int npc_move06(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b0111 => Tunneling + Smart + Telepathic            */
int npc_move07(dungeon_t *d, character_t *c)
{
  int up = d->pc_cost_all[(c->y)-1][(c->x)  ].cost,
    down = d->pc_cost_all[(c->y)+1][(c->x)  ].cost,
    left = d->pc_cost_all[(c->y)  ][(c->x)-1].cost,
   right = d->pc_cost_all[(c->y)  ][(c->x)+1].cost;
  
  if(up <= down && up <= left && up <= right) {
    if(d->hardness[(c->y)-1][(c->x)  ] < 86) {
       d->hardness[(c->y)-1][(c->x)  ] = 0;
      if(   d->map[(c->y)-1][(c->x)  ] == ter_wall) {
	    d->map[(c->y)-1][(c->x)  ] = ter_corridor;
      }
      c->next_y = (c->y)-1;
      c->next_x = (c->x)  ;
    } else {
      d->hardness[(c->y)-1][(c->x)  ] -= 85;
    }
  } else if(right <= up && right <= down && right <= left) {
    if(d->hardness[(c->y)  ][(c->x)+1] < 86){
       d->hardness[(c->y)  ][(c->x)+1] = 0;
      if(   d->map[(c->y)  ][(c->x)+1] == ter_wall) {
	    d->map[(c->y)  ][(c->x)+1] = ter_corridor;
      }
      c->next_y = (c->y)  ;
      c->next_x = (c->x)+1;
    } else {
      d->hardness[(c->y)  ][(c->x)+1] -= 85;
    }
  } else if (left <= up && left <= down && left <= right) {
    if( d->hardness[(c->y)  ][(c->x)-1] < 86){
        d->hardness[(c->y)  ][(c->x)-1] = 0;
      if(    d->map[(c->y)  ][(c->x)-1] == ter_wall) {
	     d->map[(c->y)  ][(c->x)-1] = ter_corridor;
          }
      c->next_y = (c->y)  ;
      c->next_x = (c->x)-1;
    } else {
      d->hardness[(c->y)  ][(c->x)-1] -= 85;
    }
  } else if (down <= up && down <= left && down <= right) {
    if( d->hardness[(c->y)+1][(c->x)  ] < 86){
        d->hardness[(c->y)+1][(c->x)  ] = 0;
      if(    d->map[(c->y)+1][(c->x)  ] == ter_wall) {
	     d->map[(c->y)+1][(c->x)  ] = ter_corridor;
          }
      c->next_y = (c->y)+1;
      c->next_x = (c->x)  ;
    } else {
      d->hardness[(c->y)+1][(c->x)  ] -= 85;
    }
  }
  
  return 0;
}

/* b1000 => Erratic                                   */
int npc_move08(dungeon_t *d, character_t *c)
{
  return npc_move00(d, c);
}

/* b1001 => Erratic + Telepathic                      */
int npc_move09(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1010 => Erratic + Smart                           */
int npc_move10(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1011 => Erratic + Smart + Telepathic              */
int npc_move11(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1100 => Erratic + Tunneling                       */
int npc_move12(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1101 => Erratic + Tunneling + Telepathic          */
int npc_move13(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1110 => Erratic + Tunneling + Smart               */
int npc_move14(dungeon_t *d, character_t *c)
{
  return 0;
}

/* b1111 => Erratic + Tunneling + Smart + Telepathic  */
int npc_move15(dungeon_t *d, character_t *c)
{
  return 0;
}
