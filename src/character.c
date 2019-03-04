#include "character.h"
#include "dungeon.h"
#include "npc.h"

// Function courtesy of Jeremy Scheaffer 3-2-19
void character_delete(void *v)
{
  character_t *c;

  if (v) {
    c = v;

    if (c->npc) {
      npc_delete(c->npc);
      free(c);
    }
  }
}

int can_see(dungeon_t *d, character_t *viewer, character_t *viewee)
{
  return 0;
}
