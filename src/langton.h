#ifndef LANGTON_H
#define LANGTON_H

#include <stdbool.h>

typedef struct
{
  int x;
  int y;
} Position;

typedef enum
{
  right,
  down,
  left,
  up
} Orientation;

typedef struct
{
  Position pos;
  Orientation dir;
} Ant;

typedef enum
{
  empty = 1,
  filled = 2
} Cell;

typedef struct
{
  Position pos;
  Cell value;
} MatrixChange;

typedef struct
{
  MatrixChange changes[64];
  int length;
} MatrixChangeList;

void reset_world(int height, int width, Cell world[height][width]);
void show_world(int height, int width, Cell world[height][width], Ant ant);
void reset_ant(Ant *ant, int height, int width);
void next_gen(int height, int width, Cell world[height][width], Ant *ant, bool teleport, MatrixChangeList *changes, bool do_changes);
void render_matrix(MatrixChangeList *changes, Ant ant);
void update_cell(int height, int width, Cell world[height][width], MatrixChangeList *changes, Position pos, Cell value, bool do_changes);

void populate_world(int height, int width, Cell world[height][width], int one_chance_over);

#endif