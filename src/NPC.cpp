#include <cstdint>
#include <cstdio>

#include <unistd.h>
#include <ncurses.h>

#include "NPC.h"
#include "Dungeon.h"
#include "Pathfinder.h"
#include "MonsterTemplate.h"

NPC::NPC(int pos_x, int pos_y, int seed)
{ 
  char symbols[] = "0123456789abcdef";
  srand(time(NULL) + (seed*17));

  // (n->NPC)->characteristics = rand() % 16;

  //*****************************************
  int r = rand()%3;
  if(r == 0){ abilities = 1; }
  else if(r == 1){ abilities = 3; }
  else { abilities = 7; }
  //*****************************************


  is_pc = 0;
  isAlive = 1;
  speed = (rand() % 16) + 5;
  move_time = 1000/(speed);
  symbol = symbols[abilities];
  hitpoints = 10;
  Dice temp;
  temp.init(0, 3, 1);
  attack_damage = temp;
  x = pos_x;
  y = pos_y;
  next_x = pos_x;
  next_y = pos_y;
  last_seen_x = pos_x;
  last_seen_y = pos_y;
}

NPC::NPC(MonsterTemplate &temp, int pos_x, int pos_y)
{
  is_pc = 0;
  isAlive = 1;
  x = pos_x;
  y = pos_y;
  next_x = pos_x;
  next_y = pos_y;
  last_seen_x = pos_x;
  last_seen_y = pos_y;

  name = temp.name;
  description = temp.description;
  speed = temp.speed.roll();
  move_time = 1000/(speed);
  symbol = temp.symbol;
  color = temp.color.at(0);
  abilities = temp.abilities;
  hitpoints = temp.hitpoints.roll();
  attack_damage = temp.attack_damage.copy();
  symbol = temp.symbol;
  template_index = temp.index;
}

int NPC::move(Dungeon &d)
{  
  if(abilities & NPC_ERRATIC){
    if(rand()%2){ 
      switch(rand()%8){
      case 0:
      next_y = y+1;
      next_x = x-1;
      break;
      case 1:
      next_y = y+1;

      break;

      next_x = x+1;
      break;
      case 3:
      next_y = y+1;
      next_x = x+1;
      break;
      case 4:
      next_y = y-1;

      break;
      case 5:

      next_x = x-1;
      break;
      case 6:
      next_y = y-1;
      next_x = x-1;
      break;
      case 7:
      next_y = y-1;
      next_x = x+1;
      break;
      }

      if((abilities & NPC_TUNNELING) &&
          d.map[next_y][next_x] == Dungeon::ter_wall){
        if(d.hardness[next_y][next_x] < 85){
          d.hardness[next_y][next_x] = 0;
          d.map[next_y][next_x] = Dungeon::ter_corridor;
        } else {
          d.hardness[next_y][next_x] -= 85;
          next_x = x;
          next_y = y;
        }
      } else {
        if(d.map[next_y][next_x] == Dungeon::ter_wall){
          next_x = x;
          next_y = y;
        }
      }

      if(next_y > 20 || next_y < 1 || next_x > 80 || next_x < 1){
        next_x = x;
        next_y = y;
      }

      return 0;
    }
  }

  if(d.same_room(*this, d.player)){
    last_seen_y = d.player.y;
    last_seen_x = d.player.x;
  }

  if(!abilities){
    move00(d);
  } else if(abilities & NPC_SMART){
    move01(d);
  } else if(abilities & NPC_TELEPATHIC){
    move02(d);
  } else if(abilities & (NPC_SMART + NPC_TELEPATHIC)){
    move03(d);
  } else if(abilities & NPC_TUNNELING){
    move04(d);
  } else if(abilities & (NPC_SMART + NPC_TUNNELING)){
    move05(d);
  } else if(abilities & (NPC_TELEPATHIC + NPC_TUNNELING)){
    move06(d);
  } else if(abilities & (NPC_SMART + NPC_TELEPATHIC + NPC_TUNNELING)){
    move07(d);
  }
  return 0;
}

int NPC::move00(Dungeon &d)
{
  if(d.same_room(*this, d.player)){
    if(x < d.player.x){ next_x = x+1; }
    else if(x > d.player.x){ next_x = x-1; }
    else{ next_x = x; }

    if(y < d.player.y){ next_y = y+1; }
    else if(y > d.player.y){ next_y = y-1;; }
    else{ next_y = y; }
  } else {
    next_x = x;
    next_y = y;
  }

  return 0;
}

int NPC::move01(Dungeon &d)
{
  if(x < last_seen_x){ next_x = x+1; }
  else if(x > last_seen_x){ next_x = x-1; }
  else{ next_x = x; }

  if(y < last_seen_y){ next_y = y+1; }
  else if(y > last_seen_y){ next_y = y-1; }
  else{ next_y = y; }

  if(d.map[next_y][next_x] == Dungeon::ter_wall){
    next_y = y;
    next_x = x;
  }

  return 0;
}


int NPC::move02(Dungeon &d)
{
  uint16_t r, c, cost = UINT16_MAX;
  for(r = y-1; r < y+2; r++){ 
    for(c = x-1; c < x+2; c++){
      if(d.floor_pathfinder.path[r][c].cost < cost){
        cost = d.floor_pathfinder.path[r][c].cost;
        next_x = c;
        next_y = r;
      }
    }
  }

  return 0;
}


int NPC::move03(Dungeon &d)
{
  uint16_t r, c, cost = UINT16_MAX;
  for(r = y-1; r < y+2; r++){ 
    for(c = x-1; c < x+2; c++){
      if(d.floor_pathfinder.path[r][c].cost < cost){
        cost = d.floor_pathfinder.path[r][c].cost;
        next_x = c;
        next_y = r;
      }
    }
  }
  
  return 0;
}

int NPC::move04(Dungeon &d)
{
  if(x < last_seen_x){ next_x = x+1; }
  else if(x > last_seen_x){ next_x = x-1; }
  else{ next_x = x; }

  if(y < last_seen_y){ next_y = y+1; }
  else if(y > last_seen_y){ next_y = y-1;; }
  else{ next_y = y; }

  if(d.map[next_y][next_x] == Dungeon::ter_wall){
    if(d.hardness[next_y][next_x] < 85){
      d.hardness[next_y][next_x] = 0;
      d.map[next_y][next_x] = Dungeon::ter_corridor;
    } else {
      d.hardness[next_y][next_x] -= 85;
      next_x = x;
      next_y = y;
    }
  }

  return 0;
}

int NPC::move05(Dungeon &d)
{
  if(x < last_seen_x){ next_x = x+1; }
  else if(x > last_seen_x){ next_x = x-1; }
  else{ next_x = x; }

  if(y < last_seen_y){ next_y = y+1; }
  else if(y > last_seen_y){ next_y = y-1;; }
  else{ next_y = y; }

  if(d.map[next_y][next_x] == Dungeon::ter_wall){
    if(d.hardness[next_y][next_x] < 85){
      d.hardness[next_y][next_x] = 0;
      d.map[next_y][next_x] = Dungeon::ter_corridor;
    } else {
      d.hardness[next_y][next_x] -= 85;
      next_x = x;
      next_y = y;
    }
  }

  return 0;
}

int NPC::move06(Dungeon &d)
{
  if(x < d.player.x){ next_x = x+1; }
  else if(x > d.player.x){ next_x = x-1; }
  else{ next_x = x; }

  if(y < d.player.y){ next_y = y+1; }
  else if(y > d.player.y){ next_y = y-1;; }
  else{ next_y = y; }

  if(d.map[next_y][next_x] == Dungeon::ter_wall){
    if(d.hardness[next_y][next_x] < 85){
      d.hardness[next_y][next_x] = 0;
      d.map[next_y][next_x] = Dungeon::ter_corridor;
    } else {
      d.hardness[next_y][next_x] -= 85;
      next_x = x;
      next_y = y;
    }
  }

  return 0;
}

int NPC::move07(Dungeon &d)
{
  uint16_t r, c, cost = UINT16_MAX;
  for(r = y-1; r < y+2; r++){ 
    for(c = x-1; c < x+2; c++){
      if(d.all_pathfinder.path[r][c].cost < cost){
        cost = d.all_pathfinder.path[r][c].cost;
        next_x = c;
        next_y = r;
      }
    }
  }

  if(d.map[next_y][next_x] == Dungeon::ter_wall){
    if(d.hardness[next_y][next_x] < 85){
      d.hardness[next_y][next_x] = 0;
      d.map[next_y][next_x] = Dungeon::ter_corridor;
    } else {
      d.hardness[next_y][next_x] -= 85;
      next_x = x;
      next_y = y;
    }
  }
  
  return 0;
}