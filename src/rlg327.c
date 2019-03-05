#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>

#include "dungeon.h"
#include "pathfinder.h"
#include "heap.h"
#include "character.h"
#include "npc.h"
#include "pc.h"

#define DUNGEON_Y  21
#define DUNGEON_X  80

static int32_t event_cmp(const void *key, const void *with) {
  return ((character_t *) key)->move_time - ((character_t *) with)->move_time;
}

int play_game(dungeon_t *d)
{
  uint32_t r, c, i;
  heap_t event_queue;
  character_t *cur;
  int cmd;

  heap_init(&event_queue, event_cmp, NULL);

  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d->characters[r][c]){
	heap_insert(&event_queue, d->characters[r][c]);
      }
    }
  }

  update_distances(d);

  // init ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while(d->pc.isAlive) {
    cur = heap_remove_min(&event_queue);
    
    if(cur->pc) {
      clear();
      update_output(d);
      mvprintw(0, 0, d->message);
      for(i = 1; i < DUNGEON_Y; i++) {
	mvprintw(i, 0, d->output[i-1]);
      }
      cmd = getch();
      if(pc_move(d, cmd)){
	mvprintw(0, 0, "You're a quitter!");
	getch();
	endwin();
	return 1;
      }
      update_distances(d);
    }
    
    if(cur->npc && cur->isAlive) { npc_move(d, cur); }
    cur->move_time = cur->move_time + (1000 / (cur->speed));
    heap_insert(&event_queue, cur);   
  }

  if(d->nummon){
    // PC loss
    printw("Oof that's some hot tea, better luck next time bucko\n");
  } else {
    // PC win
    printw("Nice job man, way to live!\n");
  }

  // end ncurses
  getch();
  endwin();
  
  return 0;
}

int main(int argc, char *argv[])
{
  dungeon_t dungeon;
  
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
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { write_dungeon(&dungeon); }
  } 
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--pathfind")){
      render_pc_cost_floor(&dungeon);
      render_pc_cost_all(&dungeon);
    }
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--nummon")) {
      dungeon.nummon = atoi(argv[arg+1]);
    }
  }

  place_characters(&dungeon);
  play_game(&dungeon);

  return 0;
}
