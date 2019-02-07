#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <endian.h> //NOTE

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

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
};

int place_rooms(struct room rooms[],
		uint16_t num_rooms, 
		uint8_t map[21][80])
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

int place_corridors(struct room rooms[],
		    uint16_t num_rooms,
		    uint8_t map[21][80]){

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

/*
 * precondition: num_up < num_rooms and num_down < num_rooms
 */
int place_stairs(struct staircase *stairs,
		 uint16_t num_up,
		 uint16_t num_down,
		 struct room rooms[],
		 uint16_t num_rooms)
{
  uint16_t i, rand_row, rand_col, r_index;
  srand(time(NULL));
  
  i = r_index = 0;
  do{
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    stairs[i].x = rand_col;
    stairs[i].y = rand_row;
    stairs[i].dir = 'u';
    i++;
    r_index++;
  }while(i < num_up);

  do{
    r_index = r_index % num_rooms;
    rand_row = (rand() % rooms[r_index].height) + rooms[r_index].y;
    rand_col = (rand() % rooms[r_index].width) + rooms[r_index].x;
    stairs[i].x = rand_col;
    stairs[i].y = rand_row;
    stairs[i].dir = 'd';
    i++;
    r_index++;
  }while(i < num_down+num_up);

  /*
  rand_room = rand() % num_rooms;
  rand_row = (rand() % rooms[rand_room].height) + rooms[rand_room].y;
  rand_col = (rand() % rooms[rand_room].width) + rooms[rand_room].x;
  stairs[0] = 0;
  stairs[1] = rand_row;
  stairs[2] = rand_col;

  do{
    rand_room = rand() % num_rooms;
    rand_row = (rand() % rooms[rand_room].height) + rooms[rand_room].y;
    rand_col = (rand() % rooms[rand_room].width) + rooms[rand_room].x;
    stairs[3] = 1;
    stairs[4] = rand_row;
    stairs[5] = rand_col;
  }while(stairs[1] == stairs[4] || stairs[2] == stairs[5]);
  */

  return 0;
}

int to_char_arr(uint8_t dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH],
		struct room *rooms,
		uint16_t num_rooms,
		struct staircase *stairs,
		uint16_t num_stairs,
		struct player pc,
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

  for(i = 0; i < num_stairs; i++){
    if(stairs[i].dir == 'u'){
      char_dungeon[stairs[i].y][stairs[i].x] = '>';
    }else{
      char_dungeon[stairs[i].y][stairs[i].x] = '<';
    }
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

/*
int read(){

  // HOW TO DEAL WITH LOCATION READING
  // Location = /home/student/.rlg327/dungeon
  // str(getenv("HOME"));

  char *home = getenv("HOME");
  char *path;
  path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, 'r');
  free(path);

  return 0;
}
*/

int write_dungeon(struct player pc,
		  uint8_t hardness[DUNGEON_HEIGHT][DUNGEON_WIDTH],
		  struct room *rooms,
		  uint16_t num_rooms,
		  struct staircase *stairs,
		  uint16_t num_up,
		  uint16_t num_down)
{
  char *home = getenv("HOME");
  char *path;
  path = malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, "w");

  // File data
  char file_type[] = "RLG327-S2019";
  uint32_t file_version = 0;
  uint32_t file_size = 1708 + num_rooms*4 + num_up*2 + num_down*2;
  fwrite(file_type, sizeof(char), 12, f);
  fwrite(&file_version, sizeof(uint32_t), 1, f);
  fwrite(&file_size, sizeof(uint32_t), 1, f);
  
  // PC data
  uint8_t pc_location[2] = { pc.x, pc.y };
  fwrite(pc_location, sizeof(uint8_t), 2, f);
  
  // Hardness matrix
  int i;
  for(i = 0; i < DUNGEON_HEIGHT; i++){
    fwrite(hardness[i], sizeof(uint8_t), DUNGEON_WIDTH, f);
  }

  // Room data
  fwrite(&num_rooms, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_rooms; i++){
    uint8_t room_data[4] = { rooms[i].x, rooms[i].y, 
			     rooms[i].width, rooms[i].height };
    fwrite(room_data, sizeof(uint8_t), 4, f);
  }

  // Stair data
  fwrite(&num_up, sizeof(uint16_t), 1, f);
  for(i = 0; i < num_up; i++){
    uint8_t stair_location[2] = { stairs[i].x, stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }
  fwrite(&num_down, sizeof(uint16_t), 1, f);
  for(i = i; i < num_down+num_up; i++){
    uint8_t stair_location[2] = { stairs[i].x, stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }

  fclose(f);
  free(path);
  return 0;
}		  

int main(int argc, char *argv[])
{
  uint8_t dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  uint16_t num_rooms = 6, num_up = 2, num_down = 2;
  char char_dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  struct room rooms[num_rooms];
  struct staircase stairs[num_up + num_down];
  struct player pc;

  int arg = argc;
  while(arg > 1){
    if(!strcmp(argv[arg-1],"--save")){ 
      place_rooms(rooms, num_rooms, dungeon);
      place_corridors(rooms, num_rooms, dungeon);
      place_stairs(stairs, num_up, num_down, rooms, num_rooms);

      pc.x = rooms[0].x;
      pc.y = rooms[0].y;
      
      write_dungeon(pc, dungeon, rooms, num_rooms, 
		    stairs, num_up, num_down); 
    }
    else if(!strcmp(argv[arg-1],"--load")){ 
      //read_dungeon(); 
    }
    arg--;
  }

  to_char_arr(dungeon, rooms, num_rooms, 
	      stairs, num_up+num_down, pc, char_dungeon);
  print_dungeon(char_dungeon);

  return 0;
}
