#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ncurses.h>

#include "heap.h"
#include "Character.h"
#include "NPC.h"
#include "PC.h"
#include "Dungeon.h"
#include "IO.h"
#include "Pathfinder.h"
#include "Move.h"

#define DUNGEON_Y  21
#define DUNGEON_X  80

#define DISPLAY_ALL_CMD       0
#define DISPLAY_MAP_CMD       1
#define DISPLAY_MONSTERS_CMD  2
#define DISPLAY_TELEPORT_CMD  3

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
  return ((Character *) key)->move_time - ((Character *) with)->move_time;
}

Dungeon d;

int play_game(int debug)
{
  heap_t event_queue;
  Character *cur;
  int cmd, result = 0;

  heap_init(&event_queue, event_cmp, NULL);
  d.place_characters(&event_queue);
  d.place_objects();
  d.update_distances();

  while(d.player.isAlive && d.nummon) {
    cur = (Character *)heap_remove_min(&event_queue);
    
    if(cur->is_pc) {
      do{
		if(debug){
			display(DISPLAY_ALL_CMD, d);
		} else {
			display(DISPLAY_MAP_CMD, d);
		}
        cmd = getch();
        result = move_pc(d, &event_queue, cmd);
        if(result < 0){
          mvprintw(0, 0, "You're a quitter!                                                               ");
          getch();
          endwin();
          return 1;
        } else if(result == 10){
          clear();
          mvprintw(0, 0, win_screen);
          getch();
          endwin();
          return 0;
        }
      } while(result == 2);
      d.update_distances();
    } else if(cur->isAlive) { 
      move_npc(d, *(NPC*)cur, debug); 
    }

    cur->move_time = cur->move_time + (1000 / (cur->speed));
    heap_insert(&event_queue, cur);   
  }

  clear();
  mvprintw(0, 0, loss_screen);
  getch();
  endwin();

  return 0;
}

int main(int argc, char *argv[])
{
  // Commands:
  //
  // --load -l     Load file from ~/.rlg327
  // --save -s     Saves dungeon to file in ~/.rlg327
  // --pathfind    Output pathfinding dungeons
  // --parse       Output monster templates from ~/.rlg327

  // init ncurses
  initscr();
  cbreak();
  noecho();
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  keypad(stdscr, TRUE);

  srand(time(NULL));

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load") || !strcmp(argv[arg],"-l")){ 
      read_dungeon(d);
      load++;
    }
  }
  if(!load){
    d.generate();
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { write_dungeon(d); }
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

  parse_monsters(d);
  parse_objects(d);

  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--parse")) {
      print_monster_templates(d);
      print_object_templates(d);
    }
  }

  int debug = 0;

  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--debug")) {
      debug = 1;
    }
  }

  //prompt_name(d);

  play_game(debug);

  return 0;
}
