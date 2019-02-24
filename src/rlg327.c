#include <stdio.h>
#include <stdint.h>

#include "dungeon.h"
#include "pathfinder.h"
//#include "character.h"

#define DUNGEON_Y  21
#define DUNGEON_X  80

dungeon_t dungeon;

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

int main(int argc, char *argv[])
{
  init_dungeon(&dungeon);

  // Commands:
  //
  // --load -l     Load file from /.rlg327
  // --save -s     Saves dungeon to file in /.rlg327
  // --pathfind    Output pathfinding dungeons
  // -lt [file]    Load file from .rlg327/saved_dungeons/ 
  

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load") || !strcmp(argv[arg],"-l")){ 
      read_dungeon(&dungeon,(uint8_t)0, NULL);
      load++;
    }
    if(!strcmp(argv[arg], "-lt")) {
      read_dungeon(&dungeon,(uint8_t)1, argv[arg+1]);
      load++;
    }
  }
  if(!load){
    gen_dungeon(&dungeon);
    dungeon.pc.x = dungeon.rooms[0].x;
    dungeon.pc.y = dungeon.rooms[0].y;
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { write_dungeon(&dungeon); }
  } 
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--pathfind")){ pathfind_dungeon(&dungeon); }
  }

  write_char_arr(&dungeon);
  display_dungeon(&dungeon);

  return 0;
}
