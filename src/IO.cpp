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
      if(d.characters[r-1][c]){ 
        attron(COLOR_PAIR(d.characters[r-1][c]->color));
        mvaddch(r, c, d.characters[r-1][c]->symbol);
        attroff(COLOR_PAIR(d.characters[r-1][c]->color));
      }
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
      if(d.characters[r-1][c]){
        attron(COLOR_PAIR(d.characters[r-1][c]->color));
        mvaddch(r, c, d.output[r-1][c]);
        attroff(COLOR_PAIR(d.characters[r-1][c]->color));
      } else 
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

int IO::parse_objects(Dungeon &d)
{
  char *home = getenv("HOME");
  char *path;
  path = (char*)malloc(strlen(home) + strlen("/.rlg327/object_desc.txt") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/object_desc.txt");
  
  std::ifstream file(path);
  std::string line, param, n, des, ty, c, hb, damb,
  dogb, defb, w, sb, attr, val, art, rrty;

  uint8_t param_flags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
  in_progress = 0, i;
  
  if(file.is_open()){

    getline(file,line);
    if(line.compare("RLG327 OBJECT DESCRIPTION 1")){ return -1; }

    while(getline(file, line)) {
      if(!line.compare("BEGIN OBJECT")){
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

          } else if(param == "TYPE"){
            param_flags[2]++;
            getline(str, ty);
          } else if(param == "COLOR"){
            param_flags[3]++;
            getline(str, c);
          } else if(param == "HIT"){
            param_flags[4]++;
            getline(str, hb);
          } else if(param == "DAM"){
            param_flags[5]++;
            getline(str, damb);
          } else if(param == "DODGE"){
            param_flags[6]++;
            getline(str, dogb);
          } else if(param == "DEF"){
            param_flags[7]++;
            getline(str, defb);
          } else if(param == "WEIGHT"){
            param_flags[8]++;
            getline(str, w);
          } else if(param == "SPEED"){
            param_flags[9]++;
            getline(str, sb);
          } else if(param == "ATTR"){
            param_flags[10]++;
            getline(str, attr);
          } else if(param == "VAL"){
            param_flags[11]++;
            getline(str, val);
          } else if(param == "ART"){
            param_flags[12]++;
            getline(str, art);
          } else if(param == "RRTY"){
            param_flags[13]++;
            getline(str, rrty);
          } else if(param == "END") {
            for(i = 0; i < 14; i++){
              if(param_flags[i] != 1)
                in_progress = 0;
            }
            if(in_progress) {
              d.object_templates
              .push_back(ObjectTemplate(n, des, ty, c, hb, damb, dogb, defb,
                  w, sb, attr, val, art, rrty));
              in_progress = 0;
            }
            des = "";
          }
          for(i = 0; i < 14; i++){
            if(param_flags[i] > 1)
              in_progress = 0;
          }
        }
        for(i = 0; i < 14; i++){
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
        case COLOR_RED:
          printw("RED");
          break;
        case COLOR_GREEN:
          printw("GREEN");
          break;
        case COLOR_BLUE:
          printw("BLUE");
          break;
        case COLOR_CYAN:
          printw("CYAN");
          break;
        case COLOR_YELLOW:
          printw("YELLOW");
          break;
        case COLOR_MAGENTA:
          printw("MAGENTA");
          break;
        case COLOR_WHITE:
          printw("WHITE");
          break;
        case COLOR_BLACK:
          printw("BLACK");
          break;
      }
      printw(" ");
    }
    printw("\n");


    printw(d.monster_templates.at(i).speed.c_str());
    printw("\n");

    if(d.monster_templates.at(i).abilities & NPC_SMART)
      printw("SMART ");
    if(d.monster_templates.at(i).abilities & NPC_TELEPATHIC)
      printw("TELEPATHIC ");
    if(d.monster_templates.at(i).abilities & NPC_TUNNELING)
      printw("TUNNELING ");
    if(d.monster_templates.at(i).abilities & NPC_ERRATIC)
      printw("ERRATIC ");
    if(d.monster_templates.at(i).abilities & NPC_PASS)
      printw("PASS ");
    if(d.monster_templates.at(i).abilities & NPC_DESTROY)
      printw("DESTROY ");
    if(d.monster_templates.at(i).abilities & NPC_PICKUP)
      printw("PICKUP ");
    if(d.monster_templates.at(i).abilities & NPC_UNIQUE)
      printw("UNIQUE ");
    if(d.monster_templates.at(i).abilities & NPC_BOSS)
      printw("BOSS ");

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

int IO::print_object_templates(Dungeon &d)
{
  uint32_t cmd, i = 0;
  char buffer[21];
  
  do{
    clear();
    printw("Press enter to proceed to game, use arrow keys to navigate templates\n\n");
    printw("OBJECT TEMPLATES\n\n");

    printw(d.object_templates.at(i).name.c_str());
    printw("\n");

    printw(d.object_templates.at(i).description.c_str());
    printw("\n");

    printw(d.object_templates.at(i).type.c_str());
    printw("\n");

    switch(d.object_templates.at(i).color){
        case COLOR_RED:
          printw("RED");
          break;
        case COLOR_GREEN:
          printw("GREEN");
          break;
        case COLOR_BLUE:
          printw("BLUE");
          break;
        case COLOR_CYAN:
          printw("CYAN");
          break;
        case COLOR_YELLOW:
          printw("YELLOW");
          break;
        case COLOR_MAGENTA:
          printw("MAGENTA");
          break;
        case COLOR_WHITE:
          printw("WHITE");
          break;
        case COLOR_BLACK:
          printw("BLACK");
          break;
      }
    printw("\n");

    printw(d.object_templates.at(i).hit_bonus.c_str());
    printw("\n");

    printw(d.object_templates.at(i).damage_bonus.c_str());
    printw("\n");

    printw(d.object_templates.at(i).dodge_bonus.c_str());
    printw("\n");

    printw(d.object_templates.at(i).defense_bonus.c_str());
    printw("\n");

    printw(d.object_templates.at(i).weight.c_str());
    printw("\n");

    printw(d.object_templates.at(i).speed_bonus.c_str());
    printw("\n");

    printw(d.object_templates.at(i).attribute.c_str());
    printw("\n");

    printw(d.object_templates.at(i).value.c_str());
    printw("\n");

    printw(d.object_templates.at(i).artifact.c_str());
    printw("\n");

    printw(&(d.object_templates.at(i).symbol));
    printw("\n");

    sprintf(buffer, "%d", d.object_templates.at(i).rarity);
    printw(buffer);
    printw("\n");

    cmd = getch();

    switch(cmd){
      case KEY_UP:
        if(i > 0)
          i--;
        break;
      case KEY_DOWN:
        if(i < d.object_templates.size()-1)
          i++;
        break;
    }
  } while(cmd != 10);

  return 0;
}

int IO::read_dungeon(Dungeon &d)
{
  char *home = getenv("HOME");
  char *path;
  path = (char*)malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, "r");

  // File data
  char *be_file_type = (char*)malloc(sizeof(char)*12);
  fread(be_file_type, sizeof(char), 12, f);

  uint32_t be_file_version;
  fread(&be_file_version, sizeof(uint32_t), 1, f);
  d.file_version = be32toh(be_file_version);

  uint32_t be_file_size;
  fread(&be_file_size, sizeof(uint32_t), 1, f);
  d.file_size = be32toh(be_file_size);

  // PC data
  uint8_t pc_location[2];
  fread(pc_location, sizeof(uint8_t), 2, f);
  d.player.x = pc_location[0];
  d.player.y = pc_location[1];

  // Hardness matrix
  int i, j, r, c;
  uint8_t hardness_row[DUNGEON_X];
  for(i = 0; i < DUNGEON_Y; i++){
    fread(hardness_row, sizeof(uint8_t), DUNGEON_X, f);
    for(j = 0; j < DUNGEON_X; j++){ 
      d.hardness[i][j] = hardness_row[j]; 
      if(d.hardness[i][j] == 255)
        d.map[i][j] = Dungeon::ter_immutable;
      else if(d.hardness[i][j] == 0)
        d.map[i][j] = Dungeon::ter_corridor;
      else
        d.map[i][j] = Dungeon::ter_wall;
    }
  }

  // Room data
  uint16_t be_num_rooms;
  fread(&be_num_rooms, sizeof(uint16_t), 1, f);
  d.num_rooms = be16toh(be_num_rooms);
  d.rooms = (Dungeon::room*)malloc(sizeof(Dungeon::room) * d.num_rooms);
  uint8_t room_data[4];
  for(i = 0; i < d.num_rooms; i++){
    fread(room_data, sizeof(uint8_t), 4, f);
    d.rooms[i].x = room_data[0];
    d.rooms[i].y = room_data[1];
    d.rooms[i].width = room_data[2];
    d.rooms[i].height = room_data[3];
    for(r = d.rooms[i].y; r < d.rooms[i].y+d.rooms[i].height; r++){
    for(c = d.rooms[i].x; c < d.rooms[i].x+d.rooms[i].width ; c++){
      d.map[r][c] = Dungeon::ter_floor;
    }
    }
  }

  // Stair data
  uint16_t be_num_up;
  fread(&be_num_up, sizeof(uint16_t), 1, f);
  d.num_up = be16toh(be_num_up);
  d.u_stairs = (Dungeon::stair*)malloc(sizeof(Dungeon::stair) * d.num_up);

  uint8_t stair_data[2];
  for(i = 0; i < d.num_up; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    d.u_stairs[i].x = stair_data[0];
    d.u_stairs[i].y = stair_data[1];
    d.map[d.u_stairs[i].y][d.u_stairs[i].x] = Dungeon::ter_stair_up;
  }
  uint16_t be_num_down;
  fread(&be_num_down, sizeof(uint16_t), 1, f);
  d.num_down = be16toh(be_num_down);
  d.d_stairs = (Dungeon::stair*)malloc(sizeof(Dungeon::stair) * d.num_down);

  for(i = 0; i < d.num_down; i++){
    fread(stair_data, sizeof(uint8_t), 2, f);
    d.d_stairs[i].x = stair_data[0];
    d.d_stairs[i].y = stair_data[1];
    d.map[d.d_stairs[i].y][d.d_stairs[i].x] = Dungeon::ter_stair_down;
  }
            
  fclose(f);
  free(path);
  return 0;
}

int IO::write_dungeon(Dungeon &d)
{
  char *home = getenv("HOME");
  char *path;
  path = (char*)malloc(strlen(home) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, home);
  strcat(path, "/.rlg327/dungeon");
  FILE *f = fopen(path, "w");

  // File data
  uint32_t be_file_version = htobe32(d.file_version);
  uint32_t be_file_size = htobe32(1708 + d.num_rooms*4 +
          d.num_up*2 + d.num_down*2);
  fwrite(d.file_type.c_str(), sizeof(char), 12, f);
  fwrite(&be_file_version, sizeof(uint32_t), 1, f);
  fwrite(&be_file_size, sizeof(uint32_t), 1, f);
  
  // PC data
  uint8_t pc_location[2] = { d.player.x, d.player.y };
  fwrite(pc_location, sizeof(uint8_t), 2, f);
  
  // Hardness matrix
  int i;
  for(i = 0; i < DUNGEON_Y; i++){
    fwrite(d.hardness[i], sizeof(uint8_t), DUNGEON_X, f);
  }

  // Room data
  uint16_t be_num_rooms = htobe16(d.num_rooms);
  fwrite(&be_num_rooms, sizeof(uint16_t), 1, f);
  for(i = 0; i < d.num_rooms; i++){
    uint8_t room_data[4] = { d.rooms[i].x, d.rooms[i].y, 
           d.rooms[i].width, d.rooms[i].height };
    fwrite(room_data, sizeof(uint8_t), 4, f);
  }

  // Stair data
  uint16_t be_num_up = htobe16(d.num_up);
  fwrite(&be_num_up, sizeof(uint16_t), 1, f);
  for(i = 0; i < d.num_up; i++){
    uint8_t stair_location[2] = { d.u_stairs[i].x, d.u_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }
  uint16_t be_num_down = htobe16(d.num_down);
  fwrite(&be_num_down, sizeof(uint16_t), 1, f);
  for(i = 0; i < d.num_down; i++){
    uint8_t stair_location[2] = { d.d_stairs[i].x, d.d_stairs[i].y };
    fwrite(stair_location, sizeof(uint8_t), 2, f);
  }

  fclose(f);
  free(path);
  return 0;
}
