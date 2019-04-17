#ifndef IO_H
# define IO_H

# include "Dungeon.h" 

# define DUNGEON_X 80
# define DUNGEON_Y 21

# define DISPLAY_ALL_CMD       0
# define DISPLAY_MAP_CMD       1
# define DISPLAY_MONSTERS_CMD  2
# define DISPLAY_TELEPORT_CMD  3

class NPC;
class Object;

int prompt_name(Dungeon &);
int display(int, Dungeon &);	
int display_all(Dungeon &);
int display_map(Dungeon &);
int display_monsters(Dungeon &);
int display_teleport(Dungeon &);
int print_inventory(Dungeon &);
int print_equipment(Dungeon &);
int display_monster_targetting(Dungeon &);
int display_object_info(Object &);
int display_monster_info(NPC &);
int parse_monsters(Dungeon &);
int parse_objects(Dungeon &);
int print_monster_templates(Dungeon &);
int print_object_templates(Dungeon &);
int read_dungeon(Dungeon &);
int write_dungeon(Dungeon &);

#endif