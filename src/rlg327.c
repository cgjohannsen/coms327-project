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

static char *win_screen =
"           .__    _\n"
"           @ V; .Z~M\n"
"          || :|:@  d\n"
"          d' d\\@  jf\n"
"   .*\\   :P  #P  |P\n"
"   M `|  W  .@   Z\n"
"   | .b :!  d'  W'\n"
"   |  V W   #  .W**=m_\n"
"    |  !||   @  W'_   ~V;\n"
"    ||  M| _ Nm4| YmjL|PN               Way 2 Go\n"
"     #   W#~    YN_W'YL#W#b\n"
"     |;  +       |f   `#'#8L\n"
"     W        ._#L_  .#,`'||\n"
"     |,     .WMP' ~Mm#`Nm;d|\n"
"     `|       W   Mmd#; .df\n"
"      |       M    `M#@-W'\n"
"      W       !b     WtZ'\n"
"      M        V;    |P\n"
"      ||        b   .@\n"
"       D        Y| .W'\n"
"      j|         'j@'\n"
"     jP'  L_mq=-_@'\n"
"   .Z!         jf\n"
"  mf         .W'\n"
"            .@'\n"
"           .@'\n"
"          .@\n"
"         :@\n";
static char *loss_screen =
"                           ___________________________\n"
"               ...        /                           \\\n"
"             ;::::;      /  oof thats some hot tea ... \\\n"
"           ;::::; :;     \\ better luck next time bucko /\n"
"         ;:::::'   :;     \\___________________________/\n"
"        ;:::::;     ;.     /\n"
"       ,:::::'       ;    /      OOO\\\n"
"       ::::::;       ;   /      OOOOO\\\n"
"       ;:::::;       ;         OOOOOOOO\n"
"      ,;::::::;     ;'         / OOOOOOO\n"
"    ;:::::::::`. ,,,;.        /  / DOOOOOO\n"
"  .';:::::::::::::::::;,     /  /     DOOOO\n"
" ,::::::;::::::;;;;::::;,   /  /        DOOO\n"
";`::::::`'::::::;;;::::: ,#/  /          DOOO\n"
":`:::::::`;::::::;;::: ;::#  /            DOOO\n"
"::`:::::::`;:::::::: ;::::# /              DOO\n"
"`:`:::::::`;:::::: ;::::::#/               DOO\n"
" :::`:::::::`;; ;:::::::::##                OO\n"
" ::::`:::::::`;::::::::;:::#                OO\n"
" `:::::`::::::::::::;'`:;::#                O\n"
"  `:::::`::::::::;' /  / `:#\n"
"   ::::::`:::::;'  /  /   `#\n";
  
static int32_t event_cmp(const void *key, const void *with) {
  return ((character_t *) key)->move_time - ((character_t *) with)->move_time;
}

int play_game(dungeon_t *d)
{
  uint32_t r, c;//, i;
  heap_t event_queue;
  character_t *cur;
  int cmd;

  heap_init(&event_queue, event_cmp, NULL);
  place_characters(d, &event_queue);

  update_distances(d);

  // init ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while(d->pc.isAlive) {
    cur = heap_remove_min(&event_queue);
    
    if(cur->pc) {
      do {
	clear();
	update_output(d);
	mvprintw(0, 0, d->message);
	for(r = 1; r < DUNGEON_Y+1; r++) {
	  for(c = 0; c < DUNGEON_X; c++) {
	    mvaddch(r, c, d->output[r-1][c]);
	  }
	}
	cmd = getch();
	if(cmd == 'm') {
	  display_monsters(d, cmd);
	} else if(pc_move(d, cmd, &event_queue)){
	  mvprintw(0, 0, "You're a quitter!");
	  getch();
	  endwin();
	  return 1;
	}
	update_distances(d);
      } while(cmd == 'm');
    }
    
    if(cur->npc && cur->isAlive) { npc_move(d, cur); }
    cur->move_time = cur->move_time + (1000 / (cur->speed));
    heap_insert(&event_queue, cur);   
  }

  clear();
  if(d->nummon){
    // PC loss
    mvprintw(1, 0, loss_screen);
  } else {
    // PC win
    mvprintw(0, 0, win_screen);
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

  
  play_game(&dungeon);

  return 0;
}
