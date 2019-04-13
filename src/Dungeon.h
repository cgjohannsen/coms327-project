#ifndef DUNGEON_H
# define DUNGEON_H

# include <cstdint>
# include <cstdlib>
# include <cstring>
# include <string>
# include <vector>

# include <endian.h>

# include "heap.h"
# include "Character.h"
# include "PC.h"
# include "Pathfinder.h"
# include "Object.h"
# include "MonsterTemplate.h"
# include "ObjectTemplate.h"

# define DUNGEON_Y    21
# define DUNGEON_X    80
# define MIN_ROOMS    6
# define MAX_ROOMS    10
# define MIN_UP       1
# define MIN_DOWN     1
# define MAX_UP       3
# define MAX_DOWN     3 
# define ROOM_MIN_X   4
# define ROOM_MIN_Y   3
# define ROOM_MAX_X   20
# define ROOM_MAX_Y   15
# define MAX_BUFFER   1000
# define MAX_MONSTERS 50


class Dungeon {
	
public:

	const std::string file_type = "RLG327-S2019";
	uint32_t file_version = 0, file_size;

	enum terrain {
  		ter_wall,
  		ter_immutable,
  		ter_floor,
  		ter_corridor,
  		ter_stair_up,
  		ter_stair_down,
  		ter_unknown
	};

	class room {
	public:
  		uint8_t x;
  		uint8_t y;
  		uint8_t width;
  		uint8_t height;
	};

	class stair {
	public:
  		uint8_t x;
  		uint8_t y;
  		char dir;
	};

	std::string message;

	uint16_t num_rooms;
	room *rooms;
	uint16_t num_up;
	uint16_t num_down;
	stair *u_stairs;
	stair *d_stairs;
	uint16_t nummon;
	
	Pathfinder floor_pathfinder;
	Pathfinder all_pathfinder;

	uint8_t hardness[DUNGEON_Y][DUNGEON_X];
	terrain map[DUNGEON_Y][DUNGEON_X];
	terrain seen[DUNGEON_Y][DUNGEON_X];

	PC player;
	Character *characters[DUNGEON_Y][DUNGEON_X];
	Object *objects[DUNGEON_Y][DUNGEON_X];
	std::vector<MonsterTemplate> monster_templates;
	std::vector<ObjectTemplate> object_templates;

	// Constructor/Destructor
	Dungeon();
	~Dungeon();

	// Generation handling
	int place_rooms();
	int place_corridors();
	int place_stairs();
	int place_characters(heap_t *h);
	int place_objects();
	int generate();
	int clear();

	// Character functionality
	int same_room(Character &, Character &);
	int update_distances();
	int render_pc_cost_floor();
	int render_pc_cost_all();

};



#endif
