#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <endian.h>

/* ----------------------------- */
/*       Global Constants        */
/* ----------------------------- */

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

/* ----------------------------- */
/*   Global Variables/Structs    */
/* ----------------------------- */

struct room {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
};

struct staircase {
  uint8_t x;
  uint8_t y;
  char dir;
};

struct player {
  uint8_t x;
  uint8_t y;
} pc;

char *file_type = "RLG327-S2019";
uint32_t file_version = 0, file_size;
uint16_t num_rooms = 6, num_up = 2, num_down = 2;

struct room *rooms;
struct staircase *up_stairs;
struct staircase *down_stairs;

/* ----------------------------- */
/*      Dungeon Generation       */
/* ----------------------------- */

int place_rooms(uint8_t map[21][80])
{

  /* Initialize some things */
  uint16_t i, j, r, c, isValid, max_height = 10, max_width = 20;
  srand(time(NULL));

  /* Place immutable spaces and rest of map with rock */
  for(i = 0; i < DUNGEON_HEIGHT; i++){
    if(i == 0 || i == DUNGEON_HEIGHT-1){
      for(j = 0; j < DUNGEON_WIDTH; j++){
	map[i][j] = 255;
      }
    }else{
      map[i][0] = 255;
      map[i][DUNGEON_WIDTH-1] = 255;
      for(j = 1; j < DUNGEON_WIDTH-1; j++){
	map[i][j] = 1;
      }
    }
  }

  /* Begin creating and placing rooms */
  for(i = 0; i < num_rooms; i++){
    do{
      /* Calculate coordinate for room */
      rooms[i].y = (rand() % (DUNGEON_HEIGHT-4)) + 1;
      rooms[i].x = (rand() % (DUNGEON_WIDTH-5)) + 1;
    
      /* Calculate width and height for room */
      rooms[i].width = (rand() % (max_width-4)) + 4;
      rooms[i].height = (rand() % (max_height-3)) + 3;

      isValid = 1;

      for(r = rooms[i].y-1; r < rooms[i].y+rooms[i].height+1; r++){
	for(c = rooms[i].x-1; c < rooms[i].x+rooms[i].width+1; c++){
	  if(map[r][c] == 0 || map[r][c] == 255){ isValid = 0; }
	}
      }
    }while(!isValid);

    /* Place room */
    for(r = rooms[i].y; r < rooms[i].y+rooms[i].height; r++){
      for(c = rooms[i].x; c < rooms[i].x+rooms[i].width; c++){
	map[r][c] = 0;
      }
    }
  }

  return 0;
}

int place_corridors(uint8_t map[21][80]){

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
      
      if(map[r][c] != 255){ map[r][c] = 0; }
    }while( r != rooms[room2].y || c != rooms[room2].x);
  }

  return 0;
}

int place_stairs()
{
  uint16_t i, rand_row, rand_col, r_index;
  srand(time(NULL));
  
  for(i = 0; i < num_up; i++){
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    up_stairs[i].x = rand_col;
    up_stairs[i].y = rand_row;
    up_stairs[i].dir = 'u';
    r_index++;
  }

  for(i = 0; i < num_down; i++){
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    down_stairs[i].x = rand_col;
    down_stairs[i].y = rand_row;
    down_stairs[i].dir = 'd';
    r_index++;
  }

  return 0;
}

/* ----------------------------- */
/*       Output Formatting       */
/* ----------------------------- */

int to_char_arr(uint8_t dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH],
		char char_dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH])
{
  int r, c, i;
  for(r = 0; r < DUNGEON_HEIGHT; r++){
    for(c = 0; c < DUNGEON_WIDTH; c++){
      char_dungeon[r][c] = ' ';
    }
  }

  for(i = 0; i < num_rooms; i++){
    for(r = rooms[i].y; r < rooms[i].y+rooms[i].height; r++){
      for(c = rooms[i].x; c < rooms[i].x+rooms[i].width; c++){
	char_dungeon[r][c] = '.';
      }
    }
  }

  for(i = 0; i < num_up; i++){ 
    char_dungeon[up_stairs[i].y][up_stairs[i].x] = '<';
  }
  for(i = 0; i < num_down; i++){ 
    char_dungeon[down_stairs[i].y][down_stairs[i].x] = '>';
  }

  for(r = 0; r < DUNGEON_HEIGHT; r++){
    for(c = 0; c < DUNGEON_WIDTH; c++){
      if(char_dungeon[r][c] != '.' && 
	char_dungeon[r][c] != '>' &&
	char_dungeon[r][c] != '<'){
	if(dungeon[r][c] == 0){ char_dungeon[r][c] = '#'; }
      }
    }
  }

  char_dungeon[pc.y][pc.x] = '@';

  return 0;
}

int print_dungeon(char dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH])
{
  int r, c;
  for(r = 0; r < DUNGEON_HEIGHT; r++){
    for(c = 0; c < DUNGEON_WIDTH; c++){
      printf("%c", dungeon[r][c]);
    }
    printf("\n");
  }

  return 0;
}

/* ----------------------------- */
/*          File IO              */
/* ----------------------------- */

int write_dungeon(uint8_t hardness[DUNGEON_HEIGHT][DUNGEON_WIDTH])
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
  uint8_t pc_location[2] = { pc.x, pc.y };
  fwrite(pc_location, sizeof(uint8_t), 2, f);
  
  // Hardness matrix
  int i;
  for(i = 0; i < DUNGEON_HEIGHT; i++){
    fwrite(hardness[i], sizeof(uint8_t), DUNGEON_WIDTH, f);
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
    uint8_t stair_location[2] = { up_stairs[i].x, up_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }
  uint16_t be_num_down = htobe16(num_down);
  fwrite(&be_num_down, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_down; i++){
    uint8_t stair_location[2] = { down_stairs[i].x, down_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }

  fclose(f);
  free(path);
  return 0;
}

int read_dungeon(uint8_t hardness[DUNGEON_HEIGHT][DUNGEON_WIDTH])
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
  pc.x = pc_location[0];
  pc.y = pc_location[1];

  // Hardness matrix
  int i, j;
  uint8_t hardness_row[DUNGEON_WIDTH];
  for(i = 0; i < DUNGEON_HEIGHT; i++){
    fread(hardness_row, sizeof(uint8_t), DUNGEON_WIDTH, f);
    for(j = 0; j < DUNGEON_WIDTH; j++){ hardness[i][j] = hardness_row[j]; }
  }

  // Room data
  uint16_t be_num_rooms;
  fread(&be_num_rooms, sizeof(uint16_t), 1, f);
  num_rooms = be16toh(be_num_rooms);
  rooms = realloc(rooms, num_rooms*sizeof(struct room));
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
  up_stairs = realloc(up_stairs, num_up*sizeof(struct staircase));

  uint8_t stair_data[2];
  for(i = 0; i < num_up; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    up_stairs[i].x = stair_data[0];
    up_stairs[i].y = stair_data[1];
  }
  uint16_t be_num_down;
  fread(&be_num_down, sizeof(uint16_t), 1, f);
  num_down = be16toh(be_num_down);
  down_stairs = realloc(down_stairs, num_up*sizeof(struct staircase));
  for(i = 0; i < num_up; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    down_stairs[i].x = stair_data[0];
    down_stairs[i].y = stair_data[1];
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
  uint8_t dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  char char_dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  rooms = malloc(num_rooms*sizeof(struct room));
  up_stairs = malloc(num_up*sizeof(struct staircase));
  down_stairs = malloc(num_down*sizeof(struct staircase));

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load")){ 
      
      read_dungeon(dungeon);
      load = 1;
    }
  }
  if(!load){
    place_rooms(dungeon);
    place_corridors(dungeon);
    place_stairs();
    pc.x = rooms[0].x;
    pc.y = rooms[0].y;
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save")){ write_dungeon(dungeon); }
  } 

  to_char_arr(dungeon, char_dungeon);
  print_dungeon(char_dungeon);


  return 0;
}
