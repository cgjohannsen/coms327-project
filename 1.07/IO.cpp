#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#include <ncurses.h>

#include "IO.h"
#include "NPC.h"
#include "Character.h"

IO::IO(){}

int IO::display(int cmd, Dungeon &d)
{
  switch(cmd){
    case DISPLAY_ALL_CMD:
      display_all(d);
      break;
    case DISPLAY_MAP_CMD:
      display_map(d);
      break;
    case DISPLAY_MONSTERS_CMD:
      display_monsters(d);
      break;
    case DISPLAY_TELEPORT_CMD:
      display_teleport(d);
      break;
  }

  return 0;
}

int IO::display_all(Dungeon &d)
{
	uint8_t r, c;

	mvprintw(0, 0, d.message.c_str());
	for(r = 1; r < DUNGEON_Y+1; r++) {
		for(c = 0; c < DUNGEON_X; c++) {
      if(d.characters[r-1][c]){ mvaddch(r, c, d.characters[r-1][c]->symbol); }
      else{
      switch(d.map[r-1][c]) {
        case Dungeon::ter_wall:
        case Dungeon::ter_unknown:
        case Dungeon::ter_immutable:
          mvaddch(r, c, ' ');
          break;
        case Dungeon::ter_floor:
          mvaddch(r, c, '.');
          break;
        case Dungeon::ter_corridor:
          mvaddch(r, c, '#');
          break;
        case Dungeon::ter_stair_up:
          mvaddch(r, c, '>');
          break;
        case Dungeon::ter_stair_down:
          mvaddch(r, c, '<');
          break;
        }
      }
		}      
	}

	return 0;
}

int IO::display_map(Dungeon &d)
{
  uint8_t r, c;

  mvprintw(0, 0, d.message.c_str());
  for(r = 1; r < DUNGEON_Y+1; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      mvaddch(r, c, d.output[r-1][c]);
    }      
  }

  return 0;
}

int IO::display_monsters(Dungeon &d)
{
  uint32_t cmd = (int) 'm';
  int r, c, i = 0;

  Character *monsters[d.nummon];
  
  // Collect list of monsters
  for(r = 0; r < DUNGEON_Y; r++) {
    for(c = 0; c < DUNGEON_X; c++) {
      if(d.characters[r][c] &&
        d.characters[r][c]->symbol != '@'){
        monsters[i] = d.characters[r][c];
        i++;
      }
    }
  }

  i = 0;
  do{
    //clear();
    mvprintw(1, 25, "--------------------------------");
    mvprintw(2, 25, "|         Monster List         |");
    mvprintw(3, 25, "--------------------------------");
    mvprintw(4, 25, "       Live Monsters = %d       ", d.nummon);
    if(d.nummon > 16) {
      switch(cmd) {
        case KEY_UP:
          if(i > 0)
          { i -= 16; }
          break;
        case KEY_DOWN:
          if(d.nummon - i >= 16)
          { i += 16; }
          break;
      }
    }
    
    for(r = 5; r < DUNGEON_Y+1 && i+(r-5) < d.nummon; r++) {
      std::string dir_x, dir_y;
  
      int dis_x = d.player.x - monsters[i+(r-5)]->x;
      if(dis_x > 0) {
        dir_x = "west";
      } else {
        dir_x = "east";
      }
  
      int dis_y = d.player.y - monsters[i+(r-5)]->y;
      if(dis_y > 0) {
        dir_y = "north";
      } else {
        dir_y = "south";
      }
  
      mvprintw(r, 25, "  %c, %d %s and %d %s      ", monsters[i+(r-5)]->symbol,
         abs(dis_x), dir_x.c_str(), abs(dis_y), dir_y.c_str());
      }
    for(; r < DUNGEON_Y+1; r++) {
      mvprintw(r, 25, "                                ");
    }
    cmd = getch();
  } while(cmd != 27); // Escape key
  
  return 0;
}

int IO::display_teleport(Dungeon &d)
{
  uint8_t cmd = ' ', tx = d.player.x, ty = d.player.y;
  srand(time(NULL));

  mvprintw(0, 0, d.message.c_str());
  do{
    display_all(d);
    mvaddch(ty+1, tx, '*');

    cmd = getch();
    switch(cmd){
      case 'r':
        do{
          ty = (rand() % (DUNGEON_Y-1)) + 1;
          tx = (rand() % (DUNGEON_X-1)) + 1;
        }while(d.hardness[ty][tx]);
        break;
      case '7':
      case 'y':
        if(tx-1 > 0 && ty-1 > 0){
          tx--;
          ty--;
        }
        break;
      case '8':
      case 'k':
        if(ty-1 > 0){
          ty--;
        }
        break;
      case '9':
      case 'u':
        if(tx+1 < DUNGEON_X && ty-1 > 0){
          tx++;
          ty--;
        }
        break;
      case '6':
      case 'l':
        if(tx+1 < DUNGEON_X){
          tx++;
        }
        break;
      case '3':
      case 'n':
        if(tx+1 < DUNGEON_X && ty+1 < DUNGEON_Y){
          tx++;
          ty++;
        }
        break;
      case '2':
      case 'j':
        if(ty+1 < DUNGEON_Y){
          ty++;
        }
        break;
      case '1':
      case 'b':
        if(tx-1 > 0 && ty+1 < DUNGEON_Y){
          tx--;
          ty++;
        }
        break;
      case '4':
      case 'h':
        if(tx-1 > 0){
          tx--;
        }
        break;
    }

  }while(cmd != 't' && cmd != 'r');

  d.characters[d.player.y][d.player.x] = NULL;
  d.characters[ty][tx] = &(d.player);
  d.player.y = ty;
  d.player.x = tx;

  return 0;
}

int IO::parse_monsters(Dungeon &d)
{
  char *home = getenv("HOME");
  char *path;
  path = (char*)malloc(strlen(home) + strlen("/.rlg327/monster_desc.txt") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/monster_desc.txt");
  
  std::ifstream file(path);
  std::string line, param, n, des, c, sp, ab, hp, ad, sy, r;
  //uint8_t flag_n = 0, flag_des = 0, flag_c = 0, flag_sp = 0,flag_ab = 0, 
  //flag_sy = 0, flag_r = 0, flag_ad = 0, flag_hp = 0, in_progress = 0;

  uint8_t param_flags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0}, in_progress = 0, i;
  
  if(file.is_open()){

    getline(file,line);
    if(line.compare("RLG327 MONSTER DESCRIPTION 1")){ return -1; }

    while(getline(file, line)) {
      if(!line.compare("BEGIN MONSTER")){
        in_progress = 1;
        while(getline(file,line) && in_progress) {
          std::stringstream str(line);
          getline(str, param, ' ');
          if(param == "NAME"){
            param_flags[0]++;;
            getline(str, n);
          } else if(param == "DESC"){
            param_flags[1]++;
            getline(file, line);
            des = "";
            while(line.compare(".")){
              des += line;
              getline(file, line);
              if(line.compare("."))
                des += "\n";
            }
          } else if(param == "COLOR"){
            param_flags[2]++;
            getline(str, c);
          } else if(param == "SPEED"){
            param_flags[3]++;
            getline(str, sp);
          } else if(param == "ABIL"){
            param_flags[4]++;
            getline(str, ab);
          } else if(param == "HP"){
            param_flags[5]++;
            getline(str, hp);
          } else if(param == "DAM"){
            param_flags[6]++;
            getline(str, ad);
          } else if(param == "SYMB"){
            param_flags[7]++;
            getline(str, sy);
          } else if(param == "RRTY"){
            param_flags[8]++;
            getline(str, r);
          } else if(param == "END") {
            for(i = 0; i < 9; i++){
              if(param_flags[i] != 1)
                in_progress = 0;
            }
            if(in_progress) {
              d.monster_templates
              .push_back(MonsterTemplate(n, des, c, sp, ab, hp, ad, sy, r));
              in_progress = 0;
            }
            des = "";
          }
          for(i = 0; i < 9; i++){
            if(param_flags[i] > 1)
              in_progress = 0;
          }
        }
        for(i = 0; i < 9; i++){
          param_flags[i] = 0;
        }
      }
    }
  } else { return -1; }
  

  return 0;
}

int IO::print_monster_templates(Dungeon &d)
{
  uint32_t cmd, i = 0;
  char buffer[21];
  
  do{
    clear();
    printw("Press enter to proceed to game, use arrow keys to navigate templates\n\n");
    printw("MONSTER TEMPLATES\n\n");

    printw(d.monster_templates.at(i).name.c_str());
    printw("\n");

    printw(d.monster_templates.at(i).description.c_str());
    printw("\n");
  
    for (auto j = d.monster_templates.at(i).color.begin(); 
      j != d.monster_templates.at(i).color.end(); ++j) {
      switch(*j){
        case Character::red:
          printw("RED");
          break;
        case Character::green:
          printw("GREEN");
          break;
        case Character::blue:
          printw("BLUE");
          break;
        case Character::cyan:
          printw("CYAN");
          break;
        case Character::yellow:
          printw("YELLOW");
          break;
        case Character::magenta:
          printw("MAGENTA");
          break;
        case Character::white:
          printw("WHITE");
          break;
        case Character::black:
          printw("BLACK");
          break;
      }
      printw(" ");
    }
    printw("\n");


    printw(d.monster_templates.at(i).speed.c_str());
    printw("\n");

    for (auto j = d.monster_templates.at(i).abilities.begin(); 
      j != d.monster_templates.at(i).abilities.end(); ++j) {
      switch(*j){
        case NPC::smart:
          printw("SMART");
          break;
        case NPC::telepathic:
          printw("TELE");
          break;
        case NPC::tunneling:
          printw("TUNNEL");
          break;
        case NPC::erratic:
          printw("ERRATIC");
          break;
        case NPC::pass:
          printw("PASS");
          break;
        case NPC::pickup:
          printw("PICKUP");
          break;
        case NPC::destroy:
          printw("DESTROY");
          break;
        case NPC::unique:
          printw("UNIQ");
          break;
        case NPC::boss:
          printw("BOSS");
          break;
      }
      printw(" ");
    }
    printw("\n");

    printw(d.monster_templates.at(i).hitpoints.c_str());
    printw("\n");

    printw(d.monster_templates.at(i).attack_damage.c_str());
    printw("\n");

    printw(&(d.monster_templates.at(i).symbol));
    printw("\n");

    sprintf(buffer, "%d", d.monster_templates.at(i).rarity);
    printw(buffer);
    printw("\n");

    cmd = getch();

    switch(cmd){
      case KEY_UP:
        if(i > 0)
          i--;
        break;
      case KEY_DOWN:
        if(i < d.monster_templates.size()-1)
          i++;
        break;
    }
  } while(cmd != 10);

  return 0;
}  