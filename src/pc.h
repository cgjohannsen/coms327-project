#ifndef PC_H
# define PC_H

typedef struct dungeon dungeon_t;

typedef struct pc {
} pc_t;

int pc_init(dungeon_t *d);
void pc_delete(pc_t *pc);
int pc_move(dungeon_t *d, int c);

#endif
