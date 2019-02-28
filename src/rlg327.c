#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "dungeon.h"
#include "pathfinder.h"
#include "heap.h"
#include "character.h"

#define DUNGEON_Y  21
#define DUNGEON_X  80

static int32_t event_cmp(const void *key, const void *with) {
  return ((character_t *) key)->move_time - ((character_t *) with)->move_time;
}

int play_game(dungeon_t *d, character_t *pc,
	      int n, character_t monsters[n])
{
  heap_t event_queue;
  character_t *cur_character;
  int i, win = 0;

  // Initialize dungeon + characters
  heap_init(&event_queue, event_cmp, NULL);
  for(i = 0; i < d->nummon; i++){
    character_init_monster(&monsters[i], n, monsters, d, i);
    monsters[i].hn = heap_insert(&event_queue, &monsters[i]);
  }
  character_init_pc(pc, d);
  pc->hn = heap_insert(&event_queue, pc);
  
  render_dungeon(d);
  character_render(d, pc, n, monsters);
  display_dungeon(d);
  
  while(pc->isAlive && !win) { // Game ends when pc dies

    // Pull highest priority event (character) from queue
    cur_character = heap_remove_min(&event_queue);

    if(cur_character->isAlive) { // Confirm cur_character is still alive
      // Execute desired move
      character_move(cur_character, d, pc, n, monsters);
    
      // Place current character back into queue 
      cur_character->move_time =
	(cur_character->move_time + (1000 / (cur_character->speed)));
      update_distances(d, pc->x, pc->y);
      
      cur_character->hn = heap_insert(&event_queue, cur_character);
    
      // Display dungeon with delay &
      // Update distances if pc moved
      if(cur_character->isPC) {
	usleep(250000);
        render_dungeon(d);
        character_render(d, pc, n, monsters);
        display_dungeon(d);
      }
    }

    int monsters_alive = n;
    for(i = 0; i < n; i++) {
      if(!monsters[i].isAlive){ monsters_alive--; }
    }
    if(monsters_alive == 0){ win = 1; }
    
  }

  render_dungeon(d);
  character_render(d, pc, n, monsters);
  display_dungeon(d);

  if(win){ printf("You are the winner!"); }
  else{
    printf("Oof that's some hot tea there sport. Better luck next time.\n");
  }

  return 0;
}

int main(int argc, char *argv[])
{
  dungeon_t dungeon;
  character_t *monsters;
  character_t pc;
  
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
    pc.x = (dungeon.rooms[0]).x;
    pc.y = (dungeon.rooms[0]).y;
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { write_dungeon(&dungeon); }
  } 
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--pathfind")){
      pathfind_dungeon(&dungeon, pc.x, pc.y);
    }
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--nummon")) {
      dungeon.nummon = atoi(argv[arg+1]);
      monsters = malloc(sizeof(character_t) * dungeon.nummon);
      play_game(&dungeon, &pc, dungeon.nummon, monsters);
    }
  }

  return 0;
}
