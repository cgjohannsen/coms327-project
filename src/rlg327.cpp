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
IO out;

int play_game()
{
  heap_t event_queue;
  Character *cur;
  int cmd;

  heap_init(&event_queue, event_cmp, NULL);
  d.place_characters(&event_queue);
  d.update_distances();

  while(d.player.isAlive && d.nummon) {
    cur = (Character *)heap_remove_min(&event_queue);
    
    if(cur->is_pc) {
      do {
        d.update_output();
        clear();
        out.display(DISPLAY_MAP_CMD, d);

        cmd = getch();

        if(cmd == 'm') {
          out.display(DISPLAY_MONSTERS_CMD, d);
        } else if(cmd == 'f'){
          clear();
          d.message = "Revealing dungeon.... (Press f to exit)";
          cmd = ' ';
          do{
            out.display(DISPLAY_ALL_CMD, d);
            cmd = getch();
          }while(cmd != 'f' );
          d.message = "";
        } else if(cmd == 't'){
          clear();
          d.message = "Entering teleport mode... (Press t to teleport, r for random)";
          out.display(DISPLAY_TELEPORT_CMD, d);
          d.message = "";
        } else if(( (PC*)cur )->move(d, cmd, &event_queue)){
          mvprintw(0, 0, "You're a quitter!                         ");
          getch();
          endwin();
          return 1;
        }

        d.update_distances();
      } while(cmd == 'm' || cmd == 'f' || cmd == 't');
    } else if(cur->isAlive) { 
      ((NPC*)cur)->move(d); 
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
  //start_color();
  keypad(stdscr, TRUE);

  int arg, load = 0;
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--load") || !strcmp(argv[arg],"-l")){ 
      out.read_dungeon(d);
      load++;
    }
  }
  if(!load){
    d.generate();
  }
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--save") || !strcmp(argv[arg], "-s"))
      { out.write_dungeon(d); }
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
  for(arg = 1; arg < argc; arg++){
    if(!strcmp(argv[arg],"--parse")) {
      out.parse_monsters(d);
      out.print_monster_templates(d);
      out.parse_objects(d);
      out.print_object_templates(d);
    }
  }

  play_game();

  return 0;
}
