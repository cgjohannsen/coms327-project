#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DUNGEON_HEIGHT 21
#define DUNGEON_WIDTH 80

struct room {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
};

int place_rooms(uint8_t num_rooms, 
		struct room rooms[], 
		uint8_t map[21][80])
{

  /* Initialize some things */
  uint16_t i, j, k, isValid, max_height = 10, max_width = 10;
  srand(time(NULL));

  /*  
   *  Each room has a coordinate which specifies the upper left corner,  
   *  a width, and height. There are then always a set of valid coordinates 
   *  to place a room, and a valid width/height range for each room.
   */ 

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

      /* Place and check validity */
      isValid = 1, j = rooms[i].y-1;
      do{
	k = rooms[i].x-1;
	do{
	  if(map[j][k] == 0){
	    k--;
	    for(j = j; j > rooms[i].y-1; j--){
	      for(k = k; k > rooms[i].x-1; k--){
		map[j][k] = 1;
	      }
	    }
	    isValid = 0;
	  }else if(k != rooms[i].x-1 && k != rooms[i].x+rooms[i].width+1 &&
		   j != rooms[i].y-1 && j != rooms[i].y+rooms[i].height+1){
	    map[j][k] = 0;
	  }
	  k++;
	}while(isValid && k < rooms[i].width+rooms[i].x+1);
	j++;
      }while(isValid && j < rooms[i].height+rooms[i].y+1);
    }while(!isValid);
  }

  return 0;
}

int place_corridors(uint8_t map[21][80]){
  return 0;
}

int main(int argc, char *argv[]){

  uint8_t dungeon[DUNGEON_HEIGHT][DUNGEON_WIDTH];
  struct room rooms[6];

  place_rooms(6, rooms, dungeon);
  place_corridors(dungeon);
  
  int r, c;
  for(r = 0; r < DUNGEON_HEIGHT; r ++){
    for(c = 0; c < DUNGEON_WIDTH; c++){
      if(dungeon[r][c] > 0){
	printf(" ");
      }else{
	printf(".");
      }
    }
    printf("\n");
  }

  return 0;
}
