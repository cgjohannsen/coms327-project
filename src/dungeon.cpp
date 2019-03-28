#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <climits>
#include <string>

#include <endian.h>
#include <ncurses.h>

#include "heap.h"
#include "pathfinder.h"
#include "character.h"
#include "dungeon.h"
#include "pc.h"
#include "npc.h"

/* ----------------------------- */
/*      Dungeon Generation       */
/* ----------------------------- */

dungeon::dungeon()
{
  uint16_t r, c;
  srand(time(NULL));
  
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

  rooms = (room*)malloc(sizeof(room) * MIN_ROOMS);
  u_stairs = (stair*)malloc(sizeof(stair) * MIN_UP);
  d_stairs = (stair*)malloc(sizeof(stair) * MIN_DOWN);

  message = "";
}

dungeon::~dungeon()
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

int dungeon::clear_dungeon()
{
  int r, c;
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      characters[r][c] = NULL;
    }
  }
  
  return 0;
}

int dungeon::place_rooms()
{
  /* Initialize some things */
  uint16_t i, r, c, isValid;
  srand(time(NULL));

  num_rooms = (rand() % (MAX_ROOMS-MIN_ROOMS)) + MIN_ROOMS;
  rooms = (room*)realloc(rooms, sizeof(room) * num_rooms);

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

int dungeon::place_corridors()
{
  uint16_t i, r, c, room1, room2;

  for(i = 0; i < num_rooms; i++){
    room1 = i;
    room2 = (i + 1) % num_rooms;

    r = rooms[room1].y;
    c = rooms[room1].x;
    
    do{
      if(r < rooms[room2].y){ r++; }
      else if(r > rooms[room2].y){ r--; }
      else if(c < rooms[room2].x){ c++; }
      else if(c > rooms[room2].x){ c--; }
      
      if(hardness[r][c] != 255 && map[r][c] != ter_floor){
        map[r][c] = ter_corridor;
        hardness[r][c] = 0;
      }
    }while( r != rooms[room2].y || c != rooms[room2].x);
  }

  return 0;
}

int dungeon::place_stairs()
{
  uint16_t i, rand_row, rand_col, r_index;
  srand(time(NULL));

  num_up = (rand() % MAX_UP) + MIN_UP;
  num_down = (rand() % MAX_DOWN) + MIN_DOWN;
  u_stairs = (stair*)realloc(u_stairs, sizeof(stair) * num_up);
  d_stairs = (stair*)realloc(d_stairs, sizeof(stair) *  num_down);
  
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

int dungeon::place_characters(heap_t *h)
{
  uint32_t i, r, c, x, y;
  
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      characters[r][c] = NULL;
    }
  }

  player.init(rooms[0].x, rooms[0].y);
  this->characters[player.y][player.x] = &player;
  heap_insert(h, (character*)&player);

  if(!nummon){ nummon = (rand() % (MAX_MONSTERS - 1)) + 1; }

  for(i = 0; i < nummon; i++) {
    do{
      room room = this->rooms[rand()%this->num_rooms];
      x = room.x + rand()%room.width;
      y = room.y + rand()%room.height;    
    }while(this->characters[y][x] ||
    abs(this->player.y - y) < 3 ||
    abs(this->player.x - x) < 3);

    //npc n(x, y, i);
    this->characters[y][x] = new npc(x, y, i);
    heap_insert(h, this->characters[y][x]);
  }
  
  return 0;
}

int dungeon::gen_dungeon()
{
  srand(time(NULL));

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

/* ----------------------------- */
/*          Pathfinding          */
/* ----------------------------- */

int dungeon::update_distances()
{ 
  floor_pathfinder.init(hardness);
  floor_pathfinder.pathfinder_dijkstra_floor(player.x, player.y);
  
  all_pathfinder.init(hardness);
  all_pathfinder.pathfinder_dijkstra_all(player.x, player.y);

  return 0;
}

/* ----------------------------- */
/*       Output Formatting       */
/* ----------------------------- */

int dungeon::update_output()
{
  int r, c;

  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(r < player.y+3 && r > player.y-3 && c < player.x+3 && c > player.x-3) {
        seen[r][c] = map[r][c];
        if(characters[r][c]){ output[r][c] = characters[r][c]->symbol; }
        else{
        switch(seen[r][c]) {
        case ter_wall:
        case ter_immutable:
        case ter_unknown:
          output[r][c] = ' ';
          break;
        case ter_floor:
          output[r][c] = '.';
          break;
        case ter_corridor:
          output[r][c] = '#';
          break;
        case ter_stair_up:
          output[r][c] = '>';
          break;
        case ter_stair_down:
          output[r][c] = '<';
          break;
        }
        }
      } else {
        switch(seen[r][c]) {
        case ter_wall:
        case ter_immutable:
        case ter_unknown:
          output[r][c] = ' ';
          break;
        case ter_floor:
          output[r][c] = '.';
          break;
        case ter_corridor:
          output[r][c] = '#';
          break;
        case ter_stair_up:
          output[r][c] = '>';
          break;
        case ter_stair_down:
          output[r][c] = '<';
          break;
        }
      }
    }
  }

  return 0;
}


int dungeon::render_pc_cost_floor()
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

int dungeon::render_pc_cost_all()
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


/* ----------------------------- */
/*          File IO              */
/* ----------------------------- */

int dungeon::write_dungeon()
{
  char *home = getenv("HOME");
  char *path;
  path = (char*)malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, "w");

  // File data
  uint32_t be_file_version = htobe32(file_version);
  uint32_t be_file_size = htobe32(1708 + num_rooms*4 +
				  num_up*2 + num_down*2);
  fwrite(this->file_type.c_str(), sizeof(char), 12, f);
  fwrite(&be_file_version, sizeof(uint32_t), 1, f);
  fwrite(&be_file_size, sizeof(uint32_t), 1, f);
  
  // PC data
  uint8_t pc_location[2] = { player.x, player.y };
  fwrite(pc_location, sizeof(uint8_t), 2, f);
  
  // Hardness matrix
  int i;
  for(i = 0; i < DUNGEON_Y; i++){
    fwrite(hardness[i], sizeof(uint8_t), DUNGEON_X, f);
  }

  // Room data
  uint16_t be_num_rooms = htobe16(num_rooms);
  fwrite(&be_num_rooms, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_rooms; i++){
    uint8_t room_data[4] = { rooms[i].x, rooms[i].y, 
			     rooms[i].width, rooms[i].height };
    fwrite(room_data, sizeof(uint8_t), 4, f);
  }

  // Stair data
  uint16_t be_num_up = htobe16(num_up);
  fwrite(&be_num_up, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_up; i++){
    uint8_t stair_location[2] = { u_stairs[i].x, u_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }
  uint16_t be_num_down = htobe16(num_down);
  fwrite(&be_num_down, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_down; i++){
    uint8_t stair_location[2] = { d_stairs[i].x, d_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }

  fclose(f);
  free(path);
  return 0;
}

int dungeon::read_dungeon(uint8_t test, char *n)
{
  char *home = getenv("HOME");
  char *path;
  if(test) {
    path = (char*)malloc(strlen(home) + strlen("/.rlg327/saved_dungeons/") +
		strlen(n) + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/saved_dungeons/");
    strcat(path, n);
  } else {
    path = (char*)malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/dungeon");
  }
  FILE *f = fopen(path, "r");

  // File data
  char *be_file_type = (char*)malloc(sizeof(char)*12);
  fread(be_file_type, sizeof(char), 12, f);

  uint32_t be_file_version;
  fread(&be_file_version, sizeof(uint32_t), 1, f);
  file_version = be32toh(be_file_version);

  uint32_t be_file_size;
  fread(&be_file_size, sizeof(uint32_t), 1, f);
  file_size = be32toh(be_file_size);

  // PC data
  uint8_t pc_location[2];
  fread(pc_location, sizeof(uint8_t), 2, f);
  player.x = pc_location[0];
  player.y = pc_location[1];

  // Hardness matrix
  int i, j;
  uint8_t hardness_row[DUNGEON_X];
  for(i = 0; i < DUNGEON_Y; i++){
    fread(hardness_row, sizeof(uint8_t), DUNGEON_X, f);
    for(j = 0; j < DUNGEON_X; j++){ hardness[i][j] = hardness_row[j]; }
  }

  // Room data
  uint16_t be_num_rooms;
  fread(&be_num_rooms, sizeof(uint16_t), 1, f);
  num_rooms = be16toh(be_num_rooms);
  rooms = (room*)malloc(sizeof(room) * num_rooms);
  uint8_t room_data[4];
  for(i = 0; i < num_rooms; i++){
    fread(room_data, sizeof(uint8_t), 4, f);
    rooms[i].x = room_data[0];
    rooms[i].y = room_data[1];
    rooms[i].width = room_data[2];
    rooms[i].height = room_data[3];
  }

  // Stair data
  uint16_t be_num_up;
  fread(&be_num_up, sizeof(uint16_t), 1, f);
  num_up = be16toh(be_num_up);
  u_stairs = (stair*)malloc(sizeof(stair) * num_up);

  uint8_t stair_data[2];
  for(i = 0; i < num_up; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    u_stairs[i].x = stair_data[0];
    u_stairs[i].y = stair_data[1];
  }
  uint16_t be_num_down;
  fread(&be_num_down, sizeof(uint16_t), 1, f);
  num_down = be16toh(be_num_down);
  d_stairs = (stair*)malloc(sizeof(stair) * num_down);

  for(i = 0; i < num_down; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    d_stairs[i].x = stair_data[0];
    d_stairs[i].y = stair_data[1];
  }
			      
  fclose(f);
  free(path);
  return 0;
}
