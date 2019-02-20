#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdio.h>

#include "heap.h"
#include "pathfinder.h"

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define x         1
#define y         0

heap_t heap;

static int32_t path_cmp(const void *key, const void *with) {
  return ((dungeon_path_t *) key)->cost - ((dungeon_path_t *) with)->cost;
} 

int pathfinder_init(uint8_t h[DUNGEON_Y][DUNGEON_X],
		    dungeon_path_t path[DUNGEON_Y][DUNGEON_X])
{
  heap_init(&heap, path_cmp, NULL);
  
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      path[r][c].pos[y] = r;
      path[r][c].pos[x] = c;
      path[r][c].cost = INT_MAX;
    }
  } 

  return 0;
}

int pathfinder_dijkstra_floor(uint8_t h[DUNGEON_Y][DUNGEON_X], 
			      dungeon_path_t path[DUNGEON_Y][DUNGEON_X],
			      uint8_t pc_x, uint8_t pc_y)
{
  if(h[pc_y][pc_x] != 0){ return -1; }

  uint32_t r, c;
  dungeon_path_t *p;
  path[pc_y][pc_x].cost = 0;

  // Insert all floor cells into heap
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(h[r][c] == 0){ path[r][c].hn = heap_insert(&heap, &path[r][c]); }
      else{ path[r][c].hn = NULL; }
    }
  }

  while ((p = heap_remove_min(&heap))) {
    p->hn = NULL;

    // Cell Above
    if(path[p->pos[y]+1][p->pos[x]  ].hn && 
       path[p->pos[y]+1][p->pos[x]  ].cost > p->cost + 1){
       path[p->pos[y]+1][p->pos[x]  ].cost = p->cost + 1;
       path[p->pos[y]+1][p->pos[x]  ].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]  ].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]+1][p->pos[x]  ].hn);
    }

    // Cell Above + Right
    if(path[p->pos[y]+1][p->pos[x]+1].hn && 
       path[p->pos[y]+1][p->pos[x]+1].cost > p->cost + 1){
       path[p->pos[y]+1][p->pos[x]+1].cost = p->cost + 1;
       path[p->pos[y]+1][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]+1][p->pos[x]+1].hn);
    }

    // Cell Right
    if(path[p->pos[y]  ][p->pos[x]+1].hn && 
       path[p->pos[y]  ][p->pos[x]+1].cost > p->cost + 1){
       path[p->pos[y]  ][p->pos[x]+1].cost = p->cost + 1;
       path[p->pos[y]  ][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]  ][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]  ][p->pos[x]+1].hn);
    }

    // Cell Below + Right
    if(path[p->pos[y]-1][p->pos[x]+1].hn && 
       path[p->pos[y]-1][p->pos[x]+1].cost > p->cost + 1){
       path[p->pos[y]-1][p->pos[x]+1].cost = p->cost + 1;
       path[p->pos[y]-1][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]-1][p->pos[x]+1].hn);
    }

    // Cell Below
    if(path[p->pos[y]-1][p->pos[x]  ].hn && 
       path[p->pos[y]-1][p->pos[x]  ].cost > p->cost + 1){
       path[p->pos[y]-1][p->pos[x]  ].cost = p->cost + 1;
       path[p->pos[y]-1][p->pos[x]  ].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]  ].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]-1][p->pos[x]  ].hn);
    }

    // Cell Below + Left
    if(path[p->pos[y]-1][p->pos[x]-1].hn && 
       path[p->pos[y]-1][p->pos[x]-1].cost > p->cost + 1){
       path[p->pos[y]-1][p->pos[x]-1].cost = p->cost + 1;
       path[p->pos[y]-1][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]-1][p->pos[x]-1].hn);
    }

    // Cell Left
    if(path[p->pos[y]  ][p->pos[x]-1].hn && 
       path[p->pos[y]  ][p->pos[x]-1].cost > p->cost + 1){
       path[p->pos[y]  ][p->pos[x]-1].cost = p->cost + 1;
       path[p->pos[y]  ][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]  ][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]  ][p->pos[x]-1].hn);
    }

    // Cell Above + Left 
    if(path[p->pos[y]+1][p->pos[x]-1].hn && 
       path[p->pos[y]+1][p->pos[x]-1].cost > p->cost + 1){
       path[p->pos[y]+1][p->pos[x]-1].cost = p->cost + 1;
       path[p->pos[y]+1][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
				    path[p->pos[y]+1][p->pos[x]-1].hn);
    }

  }

  heap_delete(&heap);

  return 0;

}

int pathfinder_dijkstra_all(uint8_t h[DUNGEON_Y][DUNGEON_X], 
			    dungeon_path_t path[DUNGEON_Y][DUNGEON_X],
			    uint8_t pc_x, uint8_t pc_y)
{
  if(h[pc_y][pc_x] != 0){ return -1; }

  uint32_t r, c;
  dungeon_path_t *p;
  path[pc_y][pc_x].cost = 0;

  // Insert all cells into heap
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(h[r][c] != 255){ path[r][c].hn = heap_insert(&heap, &path[r][c]); }
      else{ path[r][c].hn = NULL; }
    }
  }

  while ((p = heap_remove_min(&heap))) {
    p->hn = NULL;

    // Cell Above
    if(path[p->pos[y]+1][p->pos[x]  ].hn && 
       path[p->pos[y]+1][p->pos[x]  ].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]+1][p->pos[x]  ].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]+1][p->pos[x]  ].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]  ].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
			     path[p->pos[y]+1][p->pos[x]  ].hn);
    }

    // Cell Above + Right
    if(path[p->pos[y]+1][p->pos[x]+1].hn && 
       path[p->pos[y]+1][p->pos[x]+1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]+1][p->pos[x]+1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]+1][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]+1][p->pos[x]+1].hn);
    }
    
    // Cell Right
    if(path[p->pos[y]  ][p->pos[x]+1].hn && 
       path[p->pos[y]  ][p->pos[x]+1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]  ][p->pos[x]+1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]  ][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]  ][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]  ][p->pos[x]+1].hn);
    }
    

    // Cell Below + Right
    if(path[p->pos[y]-1][p->pos[x]+1].hn && 
       path[p->pos[y]-1][p->pos[x]+1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]-1][p->pos[x]+1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]-1][p->pos[x]+1].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]+1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]-1][p->pos[x]+1].hn);
    }
    

    // Cell Below
    if(path[p->pos[y]-1][p->pos[x]  ].hn && 
       path[p->pos[y]-1][p->pos[x]  ].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]-1][p->pos[x]  ].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]-1][p->pos[x]  ].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]  ].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]-1][p->pos[x]  ].hn);
    }
    

    // Cell Below + Left
    if(path[p->pos[y]-1][p->pos[x]-1].hn && 
       path[p->pos[y]-1][p->pos[x]-1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]-1][p->pos[x]-1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]-1][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]-1][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]-1][p->pos[x]-1].hn);
    }
    

    // Cell Left
    if(path[p->pos[y]  ][p->pos[x]-1].hn && 
       path[p->pos[y]  ][p->pos[x]-1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]  ][p->pos[x]-1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]  ][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]  ][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]  ][p->pos[x]-1].hn);
    }

    // Cell Above + Left 
    if(path[p->pos[y]+1][p->pos[x]-1].hn && 
       path[p->pos[y]+1][p->pos[x]-1].cost > 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1){
       path[p->pos[y]+1][p->pos[x]-1].cost = 
       p->cost + ((h[p->pos[y]][p->pos[x]])/85) + 1;
       path[p->pos[y]+1][p->pos[x]-1].from[y] = p->pos[y];
       path[p->pos[y]+1][p->pos[x]-1].from[x] = p->pos[x];
       heap_decrease_key_no_replace(&heap, 
       			    path[p->pos[y]+1][p->pos[x]-1].hn);
    }
    

  }

  heap_delete(&heap);

  return 0;
}

int pathfinder_print(dungeon_path_t path[DUNGEON_Y][DUNGEON_X])
{
  uint32_t r, c;
  for(r = 0; r < DUNGEON_Y; r++){
    for(c = 0; c < DUNGEON_X; c++){
      if(path[r][c].cost == INT_MAX){ printf(" ");} 
      else {printf("%d", path[r][c].cost % 10); }
    }
    printf("\n");
  }

  return 0;
}
