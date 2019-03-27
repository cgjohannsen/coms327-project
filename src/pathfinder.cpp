#include <cstdint>
#include <cstddef>
#include <climits>

#include <stdio.h>

#include "heap.h"
#include "pathfinder.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define row       1
#define col       0

heap_t heap;

static int32_t path_cmp(const void *key, const void *with) {
  return ((pathfinder::pathfinder::dungeon_path *) key)->cost - 
  ((pathfinder::dungeon_path *) with)->cost;
} 

pathfinder::pathfinder()
{
  heap_init(&heap, path_cmp, NULL);
}

pathfinder::~pathfinder()
{
  heap_delete(&heap);
}

int pathfinder::init(uint8_t hardness[DUNGEON_Y][DUNGEON_X])
{
  uint8_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      path[r][c].pos[row] = r;
      path[r][c].pos[col] = c;
      path[r][c].cost = INT_MAX;
      h[r][c] = hardness[r][c];
    }
  } 

  return 0;
}

int pathfinder::pathfinder_dijkstra_floor(uint8_t pc_x, uint8_t pc_y)
{
  if(h[pc_y][pc_x] != 0){ return -1; }

  uint32_t r, c;
  pathfinder::dungeon_path *p;
  path[pc_y][pc_x].cost = 0;

  // Insert all floor cells into heap
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(h[r][c] == 0){ path[r][c].hn = heap_insert(&heap, &path[r][c]); }
      else{ path[r][c].hn = NULL; }
    }
  }

  while ((p = (pathfinder::dungeon_path*)heap_remove_min(&heap))) {

    p->hn = NULL;

    // Cell Above
    if(path[p->pos[row]+1][p->pos[col]  ].hn && 
       path[p->pos[row]+1][p->pos[col]  ].cost > p->cost + 1){
       path[p->pos[row]+1][p->pos[col]  ].cost = p->cost + 1;
       path[p->pos[row]+1][p->pos[col]  ].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]  ].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]+1][p->pos[col]  ].hn);
    }

    // Cell Above + Right
    if(path[p->pos[row]+1][p->pos[col]+1].hn && 
       path[p->pos[row]+1][p->pos[col]+1].cost > p->cost + 1){
       path[p->pos[row]+1][p->pos[col]+1].cost = p->cost + 1;
       path[p->pos[row]+1][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]+1][p->pos[col]+1].hn);
    }

    // Cell Right
    if(path[p->pos[row]  ][p->pos[col]+1].hn && 
       path[p->pos[row]  ][p->pos[col]+1].cost > p->cost + 1){
       path[p->pos[row]  ][p->pos[col]+1].cost = p->cost + 1;
       path[p->pos[row]  ][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]  ][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]  ][p->pos[col]+1].hn);
    }

    // Cell Below + Right
    if(path[p->pos[row]-1][p->pos[col]+1].hn && 
       path[p->pos[row]-1][p->pos[col]+1].cost > p->cost + 1){
       path[p->pos[row]-1][p->pos[col]+1].cost = p->cost + 1;
       path[p->pos[row]-1][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]-1][p->pos[col]+1].hn);
    }

    // Cell Below
    if(path[p->pos[row]-1][p->pos[col]  ].hn && 
       path[p->pos[row]-1][p->pos[col]  ].cost > p->cost + 1){
       path[p->pos[row]-1][p->pos[col]  ].cost = p->cost + 1;
       path[p->pos[row]-1][p->pos[col]  ].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]  ].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]-1][p->pos[col]  ].hn);
    }

    // Cell Below + Left
    if(path[p->pos[row]-1][p->pos[col]-1].hn && 
       path[p->pos[row]-1][p->pos[col]-1].cost > p->cost + 1){
       path[p->pos[row]-1][p->pos[col]-1].cost = p->cost + 1;
       path[p->pos[row]-1][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]-1][p->pos[col]-1].hn);
    }

    // Cell Left
    if(path[p->pos[row]  ][p->pos[col]-1].hn && 
       path[p->pos[row]  ][p->pos[col]-1].cost > p->cost + 1){
       path[p->pos[row]  ][p->pos[col]-1].cost = p->cost + 1;
       path[p->pos[row]  ][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]  ][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]  ][p->pos[col]-1].hn);
    }

    // Cell Above + Left 
    if(path[p->pos[row]+1][p->pos[col]-1].hn && 
       path[p->pos[row]+1][p->pos[col]-1].cost > p->cost + 1){
       path[p->pos[row]+1][p->pos[col]-1].cost = p->cost + 1;
       path[p->pos[row]+1][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[row]+1][p->pos[col]-1].hn);
    }

  }

  return 0;
}

int pathfinder::pathfinder_dijkstra_all(uint8_t pc_x, uint8_t pc_y)
{
  if(h[pc_y][pc_x] != 0){ return -1; }
  
  uint32_t r, c;
  pathfinder::dungeon_path *p;
  path[pc_y][pc_x].cost = 0;

  // Insert all cells into heap
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(h[r][c] != 255){ 
        path[r][c].hn = heap_insert(&heap, &path[r][c]); 

        dungeon_path t = path[r][c];
        path[r][c] = t;

      }
      else{ path[r][c].hn = NULL; }
    }
  }
  
  while ((p = (pathfinder::dungeon_path*)heap_remove_min(&heap))) {
    p->hn = NULL;

    // DEBUG
    dungeon_path t = path[p->pos[row]][p->pos[col]];
    path[p->pos[row]  ][p->pos[col]  ] = t;

    // Cell Above
    if(path[p->pos[row]+1][p->pos[col]  ].hn && 
       path[p->pos[row]+1][p->pos[col]  ].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]+1][p->pos[col]  ].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]+1][p->pos[col]  ].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]  ].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
			     path[p->pos[row]+1][p->pos[col]  ].hn);
    }

    // Cell Above + Right
    if(path[p->pos[row]+1][p->pos[col]+1].hn && 
       path[p->pos[row]+1][p->pos[col]+1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]+1][p->pos[col]+1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]+1][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]+1][p->pos[col]+1].hn);
    }
    
    // Cell Right
    if(path[p->pos[row]  ][p->pos[col]+1].hn && 
       path[p->pos[row]  ][p->pos[col]+1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]  ][p->pos[col]+1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]  ][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]  ][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]  ][p->pos[col]+1].hn);
    }
    

    // Cell Below + Right
    if(path[p->pos[row]-1][p->pos[col]+1].hn && 
       path[p->pos[row]-1][p->pos[col]+1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]-1][p->pos[col]+1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]-1][p->pos[col]+1].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]+1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]-1][p->pos[col]+1].hn);
    }
    

    // Cell Below
    if(path[p->pos[row]-1][p->pos[col]  ].hn && 
       path[p->pos[row]-1][p->pos[col]  ].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]-1][p->pos[col]  ].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]-1][p->pos[col]  ].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]  ].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]-1][p->pos[col]  ].hn);
    }
    

    // Cell Below + Left
    if(path[p->pos[row]-1][p->pos[col]-1].hn && 
       path[p->pos[row]-1][p->pos[col]-1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]-1][p->pos[col]-1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]-1][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]-1][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]-1][p->pos[col]-1].hn);
    }
    

    // Cell Left
    if(path[p->pos[row]  ][p->pos[col]-1].hn && 
       path[p->pos[row]  ][p->pos[col]-1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]  ][p->pos[col]-1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]  ][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]  ][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]  ][p->pos[col]-1].hn);
    }

    // Cell Above + Left 
    if(path[p->pos[row]+1][p->pos[col]-1].hn && 
       path[p->pos[row]+1][p->pos[col]-1].cost > 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1){
       path[p->pos[row]+1][p->pos[col]-1].cost = 
       p->cost + ((h[p->pos[row]][p->pos[col]])/85) + 1;
       path[p->pos[row]+1][p->pos[col]-1].from[row] = p->pos[row];
       path[p->pos[row]+1][p->pos[col]-1].from[col] = p->pos[col];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[row]+1][p->pos[col]-1].hn);
    }
    

  }

  return 0;
}
