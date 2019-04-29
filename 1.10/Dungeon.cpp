#include <ctime>
#include <climits>
#include <string>
#include <cmath>

#include <endian.h>
#include <ncurses.h>

#include "Dungeon.h"
#include "NPC.h"

/* ----------------------------- */
/*      Dungeon Generation       */
/* ----------------------------- */

Dungeon::Dungeon()
{
  uint16_t r, c;
  
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(r == 0 || r == DUNGEON_Y-1 ||
         c == 0 || c == DUNGEON_X-1) {
          map[r][c] = ter_immutable;
          seen[r][c] = ter_immutable;
          hardness[r][c] = 255;
      }
    }
  }

  message = "";
}

Dungeon::~Dungeon()
{
  if(rooms) {
    free(rooms);
  }
  if(u_stairs) {
    free(u_stairs);
  }
  if(d_stairs) {
    free(d_stairs);
  }
}

int Dungeon::clear()
{
  if(rooms) {
    free(rooms);
  }
  if(u_stairs) {
    free(u_stairs);
  }
  if(d_stairs) {
    free(d_stairs);
  }
  
  return 0;
}

int Dungeon::place_rooms()
{
  /* Initialize some things */
  uint16_t i, r, c, isValid;

  num_rooms = (rand() % (MAX_ROOMS-MIN_ROOMS)) + MIN_ROOMS;
  rooms = (room*)malloc(sizeof(room) * num_rooms);

  /* Begin creating and placing rooms */
  for(i = 0; i < num_rooms; i++){
    do{
      /* Calculate coordinate for room */
      rooms[i].y = (rand() % (DUNGEON_Y-ROOM_MIN_Y)) + 1;
      rooms[i].x = (rand() % (DUNGEON_X-ROOM_MIN_X)) + 1;
    
      /* Calculate width and height for room */
      rooms[i].width = (rand() % (ROOM_MAX_X-ROOM_MIN_X)) + ROOM_MIN_X;
      rooms[i].height = (rand() % (ROOM_MAX_Y-ROOM_MIN_Y)) + ROOM_MIN_Y;

      isValid = 1;

      for(r = rooms[i].y-1; r < rooms[i].y+rooms[i].height+1; r++){
        for(c = rooms[i].x-1; c < rooms[i].x+rooms[i].width+1; c++){
          if(hardness[r][c] == 0 || hardness[r][c] == 255){ isValid = 0; }
        }
      }
    }while(!isValid);

    /* Place room */
    for(r = rooms[i].y; r < rooms[i].y+rooms[i].height; r++){
    for(c = rooms[i].x; c < rooms[i].x+rooms[i].width; c++){
	     map[r][c] = ter_floor;
	     hardness[r][c] = 0;
      }
    }
  }

  return 0;
}

int Dungeon::place_corridors()
{
  uint16_t i, r, c, tr, tc, room1, room2;

  for(i = 0; i < num_rooms; i++){
    room1 = i;
    room2 = (i + 1) % num_rooms;

    r = rooms[room1].y + (rand()%rooms[room1].height);
    c = rooms[room1].x + (rand()%rooms[room1].width);
    tr = rooms[room2].y + (rand()%rooms[room2].height);
    tc = rooms[room2].x + (rand()%rooms[room2].width);

    do{
      if(r < tr){ r++; }
      else if(r > tr){ r--; }
      else if(c < tc){ c++; }
      else if(c > tc){ c--; }
      
      if(hardness[r][c] != 255 && map[r][c] != ter_floor){
        map[r][c] = ter_corridor;
        hardness[r][c] = 0;
      }
    }while( r != tr || c != tc);
  }

  return 0;
}

int Dungeon::place_stairs()
{
  uint16_t i, rand_row, rand_col, r_index;

  num_up = (rand() % MAX_UP) + MIN_UP;
  num_down = (rand() % MAX_DOWN) + MIN_DOWN;
  u_stairs = (stair*)malloc(sizeof(stair) * num_up);
  d_stairs = (stair*)malloc(sizeof(stair) *  num_down);
  
  for(i = 0; i < num_up; i++){
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    u_stairs[i].x = rand_col;
    u_stairs[i].y = rand_row;
    u_stairs[i].dir = 'u';
    map[rand_row][rand_col] = ter_stair_up;
    r_index++;
  }

  for(i = 0; i < num_down; i++){
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    d_stairs[i].x = rand_col;
    d_stairs[i].y = rand_row;
    d_stairs[i].dir = 'd';
    map[rand_row][rand_col] = ter_stair_down;
    r_index++;
  }

  return 0;
}

int Dungeon::place_characters(heap_t *h)
{
  uint32_t i, r, c, x, y;
  int rrty;
  MonsterTemplate *temp;
  
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      characters[r][c] = NULL;
    }
  }

  player.init(rooms[0].x, rooms[0].y);
  this->characters[player.y][player.x] = &player;
  heap_insert(h, (Character*)&player);

  if(!nummon){ nummon = (rand() % (MAX_MONSTERS - 1)) + 1; }

  for(i = 0; i < nummon; i++) {
    // Randomly select monster template (check for rarity)
    do{
      rrty = rand() % 100;
      uint16_t index = rand() % monster_templates.size();
      temp = &monster_templates.at(index);
    } while(rrty >= temp->rarity || !temp->isValid || 
      (temp->unique && temp->num_generated != 0));

    do{
      room room_t = this->rooms[rand()%this->num_rooms];
      x = room_t.x + rand()%room_t.width;
      y = room_t.y + rand()%room_t.height;    
    }while(this->characters[y][x] ||
       abs(this->player.y - y) < 3 ||
       abs(this->player.x - x) < 3);

    this->characters[y][x] = new NPC(*temp, x, y);
    temp->num_generated++;
    heap_insert(h, this->characters[y][x]);
  }

  for (auto j = monster_templates.begin(); j != monster_templates.end(); ++j) {
    j->num_generated = 0;
  }

  return 0;
}

int Dungeon::place_objects()
{
  int i, r, c, x, y, rrty;
  ObjectTemplate *temp;

  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      objects[r][c] = NULL;
    }
  }

  for(i = 0; i < (rand() % (MAX_OBJECTS-10)) + 10; i++) {
    // Randomly select object template (check for rarity)
    do{
      rrty = rand() % 100;
      uint16_t index = rand() % object_templates.size();
      temp = &object_templates.at(index);
    } while(rrty >= temp->rarity || !temp->isValid || 
      (temp->unique && temp->num_generated != 0));

    do{
      room room = this->rooms[rand()%this->num_rooms];
      x = room.x + rand()%room.width;
      y = room.y + rand()%room.height;    
    }while(this->objects[y][x]);

    this->objects[y][x] = new Object(*temp, x, y);
    temp->num_generated++;
  }

  for (auto j = object_templates.begin(); j != object_templates.end(); ++j) 
    j->num_generated = 0;

  return 0;
}

int Dungeon::generate()
{
  int r, c;
  for(r = 1; r < DUNGEON_Y-1; r++) {
    for(c = 1; c < DUNGEON_X-1; c++) {
      hardness[r][c] = rand() % 254 + 1;
      map[r][c] = ter_wall;
      seen[r][c] = ter_unknown;
    }
  }
  
  place_rooms();
  place_corridors();
  place_stairs();
 
  return 0;
}

int Dungeon::same_room(Character &c1, Character &c2)
{
  int i;

  for(i = 0; i < num_rooms; i++){
    if((c1.x >= rooms[i].x) && (c1.y >= rooms[i].y) &&
       (c1.x - rooms[i].x) < rooms[i].width && 
       (c1.y - rooms[i].y) < rooms[i].height && 
       (c2.x >= rooms[i].x) && (c2.y >= rooms[i].y) &&
       (c2.x - rooms[i].x) < rooms[i].width && 
       (c2.y - rooms[i].y) < rooms[i].height){
      return 1;
    }
  }
  
  return 0;
}

/* ----------------------------- */
/*          Pathfinding          */
/* ----------------------------- */

int Dungeon::update_distances()
{ 
  floor_pathfinder.init(hardness);
  floor_pathfinder.dijkstra_floor(player.x, player.y);

  all_pathfinder.init(hardness);
  all_pathfinder.dijkstra_all(player.x, player.y);

  return 0;
}

/* ----------------------------- */
/*       Output Formatting       */
/* ----------------------------- */

int Dungeon::render_pc_cost_floor()
{
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(this->floor_pathfinder.path[r][c].cost == INT_MAX){ printf(" "); } 
      else {printf("%d", this->floor_pathfinder.path[r][c].cost % 10); }
    }
    printf("\n");
  }

  return 0;
}

int Dungeon::render_pc_cost_all()
{
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(this->all_pathfinder.path[r][c].cost == INT_MAX){ printf(" "); } 
      else {printf("%d", this->all_pathfinder.path[r][c].cost % 10); }
    }
    printf("\n");
  }

  return 0;
}
