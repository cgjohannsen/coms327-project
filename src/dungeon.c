#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <endian.h>

#include "heap.h"
#include "pathfinder.h"

/* ----------------------------- */
/*       Global Constants        */
/* ----------------------------- */

#define DUNGEON_Y  21
#define DUNGEON_X  80
#define MIN_ROOMS  6
#define MAX_ROOMS  10
#define MAX_UP     3
#define MAX_DOWN   3
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 20
#define ROOM_MAX_Y 15

/* ----------------------------- */
/*   Global Variables/Structs    */
/* ----------------------------- */

typedef struct room {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
} room_t;

typedef struct stair {
  uint8_t x;
  uint8_t y;
  char dir;
} stair_t;

typedef struct player {
  uint8_t x;
  uint8_t y;
} player_t;

typedef struct dungeon {
  uint16_t num_rooms;
  room_t *rooms;
  stair_t *u_stairs;
  stair_t *d_stairs;
  uint16_t num_up;
  uint16_t num_down;
  player_t pc;
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  char char_arr[DUNGEON_Y][DUNGEON_X];
} dungeon_t;

char *file_type = "RLG327-S2019";
uint32_t file_version = 0, file_size;
uint16_t num_rooms = 6, num_up = 2, num_down = 2;

/* ----------------------------- */
/*      Dungeon Generation       */
/* ----------------------------- */

int init_dungeon(dungeon_t *d)
{
  uint16_t i, j;
  for(i = 0; i < DUNGEON_Y; i++) {
    for(j = 0; j < DUNGEON_X; j++) {
      d->hardness[i][j] = 1;
      if(i == 0 || i == DUNGEON_Y-1 ||
	 j == 0 || j == DUNGEON_X-1) {
       d->hardness[i][j] = 255;
      }
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
  d->rooms = malloc(sizeof(room_t) * d->num_rooms);

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
	d->hardness[r][c] = 0;
      }
    }
  }

  return 0;
}

int place_corridors(dungeon_t *d){

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
      
      if(d->hardness[r][c] != 255){ d->hardness[r][c] = 0; }
    }while( r != d->rooms[room2].y || c != d->rooms[room2].x);
  }

  return 0;
}

int place_stairs(dungeon_t *d)
{
  uint16_t i, rand_row, rand_col, r_index;
  srand(time(NULL));

  d->num_up = (rand() % MAX_UP) + 1;
  d->num_down = (rand() % MAX_DOWN) + 1;
  d->u_stairs = malloc(sizeof(stair_t) * d->num_up);
  d->d_stairs = malloc(sizeof(stair_t) * d-> num_down);
  
  for(i = 0; i < d->num_up; i++){
    r_index = r_index % d->num_rooms;
    rand_row = (rand() % d->rooms[r_index].height) + d->rooms[r_index].y;
    rand_col = (rand() % d->rooms[r_index].width) + d->rooms[r_index].x;
    d->u_stairs[i].x = rand_col;
    d->u_stairs[i].y = rand_row;
    d->u_stairs[i].dir = 'u';
    r_index++;
  }

  for(i = 0; i < d->num_down; i++){
    r_index = r_index % d->num_rooms;
    rand_row = (rand() % d->rooms[r_index].height) + d->rooms[r_index].y;
    rand_col = (rand() % d->rooms[r_index].width) + d->rooms[r_index].x;
    d->d_stairs[i].x = rand_col;
    d->d_stairs[i].y = rand_row;
    d->d_stairs[i].dir = 'd';
    r_index++;
  }

  return 0;
}

int gen_dungeon(dungeon_t *d)
{
  place_rooms(d);
  place_corridors(d);
  place_stairs(d);
 
  return 0;
}

/* ----------------------------- */
/*          Pathfinding          */
/* ----------------------------- */

int pathfind_dungeon(dungeon_t *d)
{
  dungeon_path_t path[DUNGEON_Y][DUNGEON_X];

  pathfinder_init(d->hardness, path);
  pathfinder_dijkstra_floor(d->hardness, path, d->pc.x, d->pc.y);
  pathfinder_print(path);

  pathfinder_init(d->hardness, path);
  pathfinder_dijkstra_all(d->hardness, path, d->pc.x, d->pc.y);
  pathfinder_print(path);
  
  return 0;
}

/* ----------------------------- */
/*       Output Formatting       */
/* ----------------------------- */

int write_char_arr(dungeon_t *d)
{
  int r, c, i;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      d->char_arr[r][c] = ' ';
    }
  }

  for(i = 0; i < d->num_rooms; i++){
    for(r = d->rooms[i].y; r < d->rooms[i].y+d->rooms[i].height; r++){
      for(c = d->rooms[i].x; c < d->rooms[i].x+d->rooms[i].width; c++){
	d->char_arr[r][c] = '.';
      }
    }
  }

  for(i = 0; i < d->num_up; i++){ 
    d->char_arr[d->u_stairs[i].y][d->u_stairs[i].x] = '<';
  }
  for(i = 0; i < d->num_down; i++){ 
    d->char_arr[d->d_stairs[i].y][d->d_stairs[i].x] = '>';
  }

  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(d->char_arr[r][c] != '.' && 
	d->char_arr[r][c] != '>' &&
	d->char_arr[r][c] != '<'){
	if(d->hardness[r][c] == 0){ d->char_arr[r][c] = '#'; }
      }
    }
  }

  d->char_arr[d->pc.y][d->pc.x] = '@';

  return 0;
}

int display_dungeon(dungeon_t *d)
{
  int r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      printf("%c", d->char_arr[r][c]);
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
  uint32_t be_file_size = htobe32(1708 + num_rooms*4 + num_up*2 + num_down*2);
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

int read_dungeon(dungeon_t *d)
{
  char *home = getenv("HOME");
  char *path;
  path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
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

/* ----------------------------- */
/*            Main               */
/* ----------------------------- */

int main(int argc, char *argv[])
{
  dungeon_t dungeon;

  init_dungeon(&dungeon);

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load")){ 
      
      read_dungeon(&dungeon);
      load = 1;
    }
  }
  if(!load){
    gen_dungeon(&dungeon);
    dungeon.pc.x = dungeon.rooms[0].x;
    dungeon.pc.y = dungeon.rooms[0].y;
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save")){ write_dungeon(&dungeon); }
  } 
  for(arg = 1; arg < argc; arg++){
    // -p = pathfind
    if(!strcmp(argv[arg],"-p")){ pathfind_dungeon(&dungeon); }
  } 

  write_char_arr(&dungeon);
  display_dungeon(&dungeon);

  return 0;
}
