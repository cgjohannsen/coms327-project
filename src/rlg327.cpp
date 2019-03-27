#include <cstdio>
#include <cstdint>
#include <cstring>

//#include <unistd.h>
#include <ncurses.h>

#include "heap.h"
#include "character.h"
#include "npc.h"
#include "pc.h"
#include "dungeon.h"
#include "io.h"
#include "pathfinder.h"

#define DUNGEON_Y  21
#define DUNGEON_X  80

static char win_screen[] =
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
static char loss_screen[] =
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
  return ((character *) key)->move_time - ((character *) with)->move_time;
}

int play_game(dungeon &d)
{
  heap_t event_queue;
  character *cur;
  io out;
  int cmd;

  heap_init(&event_queue, event_cmp, NULL);
  d.place_characters(&event_queue);
  d.update_distances();
 
  // init ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);

  while(d.player.isAlive && d.nummon) {
    cur = (character *)heap_remove_min(&event_queue);
    
    if(cur->is_pc) {
      do {
        d.update_output();
        out.display_map(d);

        cmd = getch();

        if(cmd == 'm') {
          out.display_monsters(d);
        } else if(( (pc*)cur )->pc_move(d, cmd, &event_queue)){
          mvprintw(0, 0, "You're a quitter!                         ");
          getch();
          endwin();
          return 1;
        }

        d.update_distances();
      } while(cmd == (int) 'm');
    } else if(cur->isAlive) { 
      ((npc*)cur)->npc_move(d); 
    }

    cur->move_time = cur->move_time + (1000 / (cur->speed));
    heap_insert(&event_queue, cur);   
  }

  clear();
  if(d.nummon){
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
  dungeon d;

  // Commands:
  //
  // --load -l     Load file from /.rlg327
  // --save -s     Saves dungeon to file in /.rlg327
  // --pathfind    Output pathfinding dungeons
  // -lt [file]    Load file from .rlg327/saved_dungeons/ 
  

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load") || !strcmp(argv[arg],"-l")){ 
      d.read_dungeon((uint8_t)0, NULL);
      load++;
    }
    if(!strcmp(argv[arg], "-lt")) {
      d.read_dungeon((uint8_t)1, argv[arg+1]);
      load++;
    }
  }
  if(!load){
    d.gen_dungeon();
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { d.write_dungeon(); }
  } 
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--pathfind")){
      d.render_pc_cost_floor();
      d.render_pc_cost_all();
    }
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--nummon")) {
      d.nummon = atoi(argv[arg+1]);
    }
  }

  play_game(d);

  return 0;
}
