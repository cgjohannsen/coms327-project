#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <endian.h>
#include <limits.h>
#include <unistd.h>

#include "heap.h"
#include "pathfinder.h"
#include "character.h"
#include "dungeon.h"
#include "pc.h"
#include "npc.h"

/* ----------------------------- */
/*   Global Variables/Structs    */
/* ----------------------------- */

static char *file_type = "RLG327-S2019";
static uint32_t file_version = 0, file_size;

/* ----------------------------- */
/*      Dungeon Generation       */
/* ----------------------------- */

int init_dungeon(dungeon_t *d)
{
  uint16_t r, c;
  srand(time(NULL));
  
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(r == 0 || r == DUNGEON_Y-1 ||
	 c == 0 || c == DUNGEON_X-1) {
	d->map[r][c] = ter_immutable;
        d->hardness[r][c] = 255;
      }
    }
  }

  d->rooms = malloc(sizeof(room_t) * MIN_ROOMS);
  d->u_stairs = malloc(sizeof(stair_t) * MIN_UP);
  d->d_stairs = malloc(sizeof(stair_t) * MIN_DOWN);

  d->message = "";

  return 0;
}

int clear_dungeon(dungeon_t *d)
{
  int r, c;
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      d->characters[r][c] = NULL;
    }
  }
  
  return 0;
}

int delete_dungeon(dungeon_t *d)
{
  if(d->rooms) {
    free(d->rooms);
  }
  if(d->u_stairs) {
    free(d->u_stairs);
  }
  if(d->d_stairs) {
    free(d->d_stairs);
  }

  d->message = "";

  int r, c;
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      d->characters[r][c] = NULL;
    }
  }
  
  return 0;
}

int place_rooms(dungeon_t *d)
{
  /* Initialize some things */
  uint16_t i, r, c, isValid;
  srand(time(NULL));

  d->num_rooms = (rand() % (MAX_ROOMS-MIN_ROOMS)) + MIN_ROOMS;
  d->rooms = realloc(d->rooms, sizeof(room_t) * d->num_rooms);

  /* Begin creating and placing rooms */
  for(i = 0; i < d->num_rooms; i++){
    do{
      /* Calculate coordinate for room */
      d->rooms[i].y = (rand() % (DUNGEON_Y-ROOM_MIN_Y)) + 1;
      d->rooms[i].x = (rand() % (DUNGEON_X-ROOM_MIN_X)) + 1;
    
      /* Calculate width and height for room */
      d->rooms[i].width = (rand() % (ROOM_MAX_X-ROOM_MIN_X)) + ROOM_MIN_X;
      d->rooms[i].height = (rand() % (ROOM_MAX_Y-ROOM_MIN_Y)) + ROOM_MIN_Y;

      isValid = 1;

      for(r = d->rooms[i].y-1; r < d->rooms[i].y+d->rooms[i].height+1; r++){
	for(c = d->rooms[i].x-1; c < d->rooms[i].x+d->rooms[i].width+1; c++){
	  if(d->hardness[r][c] == 0 || d->hardness[r][c] == 255){ isValid = 0; }
	}
      }
    }while(!isValid);

    /* Place room */
    for(r = d->rooms[i].y; r < d->rooms[i].y+d->rooms[i].height; r++){
      for(c = d->rooms[i].x; c < d->rooms[i].x+d->rooms[i].width; c++){
	d->map[r][c] = ter_floor;
	d->hardness[r][c] = 0;
      }
    }
  }

  return 0;
}

int place_corridors(dungeon_t *d)
{
  uint16_t i, r, c, room1, room2;

  for(i = 0; i < d->num_rooms; i++){
    room1 = i;
    room2 = (i + 1) % d->num_rooms;

    r = d->rooms[room1].y;
    c = d->rooms[room1].x;
    
    do{
      if(r < d->rooms[room2].y){ r++; }
      else if(r > d->rooms[room2].y){ r--; }
      else if(c < d->rooms[room2].x){ c++; }
      else if(c > d->rooms[room2].x){ c--; }
      
      if(d->hardness[r][c] != 255 && d->map[r][c] != ter_floor){
	d->map[r][c] = ter_corridor;
	d->hardness[r][c] = 0;
      }
    }while( r != d->rooms[room2].y || c != d->rooms[room2].x);
  }

  return 0;
}

int place_stairs(dungeon_t *d)
{
  uint16_t i, rand_row, rand_col, r_index;
  srand(time(NULL));

  d->num_up = (rand() % MAX_UP) + MIN_UP;
  d->num_down = (rand() % MAX_DOWN) + MIN_DOWN;
  d->u_stairs = realloc(d->u_stairs, sizeof(stair_t) * d->num_up);
  d->d_stairs = realloc(d->d_stairs, sizeof(stair_t) * d-> num_down);
  
  for(i = 0; i < d->num_up; i++){
    r_index = r_index % d->num_rooms;
    rand_row = (rand() % d->rooms[r_index].height) + d->rooms[r_index].y;
    rand_col = (rand() % d->rooms[r_index].width) + d->rooms[r_index].x;
    d->u_stairs[i].x = rand_col;
    d->u_stairs[i].y = rand_row;
    d->u_stairs[i].dir = 'u';
    d->map[rand_row][rand_col] = ter_stair_up;
    r_index++;
  }

  for(i = 0; i < d->num_down; i++){
    r_index = r_index % d->num_rooms;
    rand_row = (rand() % d->rooms[r_index].height) + d->rooms[r_index].y;
    rand_col = (rand() % d->rooms[r_index].width) + d->rooms[r_index].x;
    d->d_stairs[i].x = rand_col;
    d->d_stairs[i].y = rand_row;
    d->d_stairs[i].dir = 'd';
    d->map[rand_row][rand_col] = ter_stair_down;
    r_index++;
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  srand(time(NULL));

  int r, c;
  for(r = 1; r < DUNGEON_Y-1; r++) {
    for(c = 1; c < DUNGEON_X-1; c++) {
      d->hardness[r][c] = rand() % 254 + 1;
      d->map[r][c] = ter_wall;
    }
  }
  
  place_rooms(d);
  place_corridors(d);
  place_stairs(d);
 
  return 0;
}

int place_characters(dungeon_t *d, heap_t *h)
{
  uint32_t i, r, c;
  
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      d->characters[r][c] = NULL;
    }
  }

  pc_init(d);
  if(!d->nummon){ d->nummon = (rand() % (MAX_MONSTERS - 1)) + 1; }
  for(i = 0; i < d->nummon; i++) {
    npc_init(d, i);
  }

  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d->characters[r][c]){
	heap_insert(h, d->characters[r][c]);
      }
    }
  }
  
  return 0;
}

/* ----------------------------- */
/*          Pathfinding          */
/* ----------------------------- */

int update_distances(dungeon_t *d)
{ 
  pathfinder_init(d->hardness, d->pc_cost_floor);
  pathfinder_dijkstra_floor(d->hardness, d->pc_cost_floor, d->pc.x, d->pc.y);
  
  pathfinder_init(d->hardness, d->pc_cost_all);
  pathfinder_dijkstra_all(d->hardness, d->pc_cost_all, d->pc.x, d->pc.y);

  return 0;
}

/* ----------------------------- */
/*       Output Formatting       */
/* ----------------------------- */

int update_output(dungeon_t *d)
{
  int r, c;

  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d->characters[r][c]){ d->output[r][c] = d->characters[r][c]->symbol; }
      else {
	switch(d->map[r][c]) {
	case ter_wall:
	case ter_immutable:
	  d->output[r][c] = ' ';
	  break;
	case ter_floor:
	  d->output[r][c] = '.';
	  break;
	case ter_corridor:
	  d->output[r][c] = '#';
	  break;
	case ter_stair_up:
	  d->output[r][c] = '>';
	  break;
	case ter_stair_down:
	  d->output[r][c] = '<';
	  break;
	}
      }
    }
  }

  return 0;
}

int render_pc_cost_floor(dungeon_t *d)
{
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(d->pc_cost_floor[r][c].cost == INT_MAX){ printf(" "); } 
      else {printf("%d", d->pc_cost_floor[r][c].cost % 10); }
    }
    printf("\n");
  }

  return 0;
}

int render_pc_cost_all(dungeon_t *d)
{
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(d->pc_cost_all[r][c].cost == INT_MAX){ printf(" "); } 
      else {printf("%d", d->pc_cost_all[r][c].cost % 10); }
    }
    printf("\n");
  }

  return 0;
}

/* ----------------------------- */
/*          File IO              */
/* ----------------------------- */

int write_dungeon(dungeon_t *d)
{
  char *home = getenv("HOME");
  char *path;
  path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, "w");

  // File data
  uint32_t be_file_version = htobe32(file_version);
  uint32_t be_file_size = htobe32(1708 + d->num_rooms*4 +
				  d->num_up*2 + d->num_down*2);
  fwrite(file_type, sizeof(char), 12, f);
  fwrite(&be_file_version, sizeof(uint32_t), 1, f);
  fwrite(&be_file_size, sizeof(uint32_t), 1, f);
  
  // PC data
  uint8_t pc_location[2] = { d->pc.x, d->pc.y };
  fwrite(pc_location, sizeof(uint8_t), 2, f);
  
  // Hardness matrix
  int i;
  for(i = 0; i < DUNGEON_Y; i++){
    fwrite(d->hardness[i], sizeof(uint8_t), DUNGEON_X, f);
  }

  // Room data
  uint16_t be_num_rooms = htobe16(d->num_rooms);
  fwrite(&be_num_rooms, sizeof(uint16_t), 1, f);
  for(i = 0; i < d->num_rooms; i++){
    uint8_t room_data[4] = { d->rooms[i].x, d->rooms[i].y, 
			     d->rooms[i].width, d->rooms[i].height };
    fwrite(room_data, sizeof(uint8_t), 4, f);
  }

  // Stair data
  uint16_t be_num_up = htobe16(d->num_up);
  fwrite(&be_num_up, sizeof(uint16_t), 1, f);
  for(i = 0; i < d->num_up; i++){
    uint8_t stair_location[2] = { d->u_stairs[i].x, d->u_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }
  uint16_t be_num_down = htobe16(d->num_down);
  fwrite(&be_num_down, sizeof(uint16_t), 1, f);
  for(i = 0; i < d->num_down; i++){
    uint8_t stair_location[2] = { d->d_stairs[i].x, d->d_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }

  fclose(f);
  free(path);
  return 0;
}

int read_dungeon(dungeon_t *d, uint8_t test, char *n)
{
  char *home = getenv("HOME");
  char *path;
  if(test) {
    path = malloc(strlen(home) + strlen("/.rlg327/saved_dungeons/") +
		strlen(n) + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/saved_dungeons/");
    strcat(path, n);
  } else {
    path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/dungeon");
  }
  FILE *f = fopen(path, "r");

  // File data
  char *be_file_type = malloc(sizeof(char)*12);
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
  d->pc.x = pc_location[0];
  d->pc.y = pc_location[1];

  // Hardness matrix
  int i, j;
  uint8_t hardness_row[DUNGEON_X];
  for(i = 0; i < DUNGEON_Y; i++){
    fread(hardness_row, sizeof(uint8_t), DUNGEON_X, f);
    for(j = 0; j < DUNGEON_X; j++){ d->hardness[i][j] = hardness_row[j]; }
  }

  // Room data
  uint16_t be_num_rooms;
  fread(&be_num_rooms, sizeof(uint16_t), 1, f);
  d->num_rooms = be16toh(be_num_rooms);
  d->rooms = malloc(sizeof(room_t) * d->num_rooms);
  uint8_t room_data[4];
  for(i = 0; i < d->num_rooms; i++){
    fread(room_data, sizeof(uint8_t), 4, f);
    d->rooms[i].x = room_data[0];
    d->rooms[i].y = room_data[1];
    d->rooms[i].width = room_data[2];
    d->rooms[i].height = room_data[3];
  }

  // Stair data
  uint16_t be_num_up;
  fread(&be_num_up, sizeof(uint16_t), 1, f);
  d->num_up = be16toh(be_num_up);
  d->u_stairs = malloc(sizeof(stair_t) * d->num_up);

  uint8_t stair_data[2];
  for(i = 0; i < d->num_up; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    d->u_stairs[i].x = stair_data[0];
    d->u_stairs[i].y = stair_data[1];
  }
  uint16_t be_num_down;
  fread(&be_num_down, sizeof(uint16_t), 1, f);
  d->num_down = be16toh(be_num_down);
  d->d_stairs = malloc(sizeof(stair_t) * d->num_down);

  for(i = 0; i < d->num_down; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    d->d_stairs[i].x = stair_data[0];
    d->d_stairs[i].y = stair_data[1];
  }
			      
  fclose(f);
  free(path);
  return 0;
}
