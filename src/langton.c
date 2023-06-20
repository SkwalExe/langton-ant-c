#include "langton.h"
#include "utils.h"
#include <stdio.h>
#include <stdbool.h>

void reset_world(int height, int width, Cell world[height][width])
{
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      world[i][j] = empty;
    }
  }
}

void show_world(int height, int width, Cell world[height][width], Ant ant)
{
  clear_terminal();
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      printf("\x1b[%sm  \x1b[0m", ant.pos.y == i && ant.pos.x == j ? "41" : (world[i][j] == filled ? "47" : "40"));
    }
    printf("\n");
  }
  fflush(stdout);
}

void render_matrix(MatrixChangeList *changes, Ant ant)
{
  for (int i = 0; i < changes->length; i++)
  {
    print_at(changes->changes[i].pos.x * 2 + 1, changes->changes[i].pos.y, changes->changes[i].value == filled ? "\x1b[47m  \x1b[0m" : "\x1b[40m  \x1b[0m");
  }
  print_at(ant.pos.x * 2 + 1, ant.pos.y, "\x1b[41m  \x1b[0m");
  fflush(stdout);
  changes->length = 0;
}

void reset_ant(Ant *ant, int height, int width)
{
  ant->pos.x = width / 2;
  ant->pos.y = height / 2;
  ant->dir = right;
}

void update_cell(int height, int width, Cell world[height][width], MatrixChangeList *changes, Position pos, Cell value, bool do_changes) 
{ 
  world[pos.y][pos.x] = value;
  if (!do_changes) return;
  changes->changes[changes->length].pos = pos;
  changes->changes[changes->length].value = value;
  changes->length++;
}

void next_gen(int height, int width, Cell world[height][width], Ant *ant, bool teleport, MatrixChangeList *changes,  bool do_changes)
{
  // inverse the cell the ant is on
  if (world[ant->pos.y][ant->pos.x] == filled)
    update_cell(height, width, world, changes, ant->pos, empty, do_changes);
  else
  {
    update_cell(height, width, world, changes, ant->pos, filled, do_changes);
  } 

  // rotate the ant
  if (world[ant->pos.y][ant->pos.x] == filled)
  {
    // turn left
    if (ant->dir == 0)
    {
      ant->dir = 3;
    }
    else
    {
      ant->dir--;
    }
  }
  else
  {
    // turn right
    if (ant->dir == 3)
    {
      ant->dir = 0;
    }
    else
    {
      ant->dir++;
    }
  }

  // make the ant move
  switch (ant->dir)
  {
  case down:
    if (ant->pos.y + 1 <= height - 1)
      ant->pos.y++;
    else if (teleport)
      ant->pos.y = 0;
    break;
  case up:
    if (ant->pos.y - 1 >= 0)
      ant->pos.y--;
    else if (teleport)
      ant->pos.y = height - 1;
    break;
  case left:
    if (ant->pos.x - 1 >= 0)
      ant->pos.x--;
    else if (teleport)
      ant->pos.x = width - 1;
    break;
  case right:
    if (ant->pos.x + 1 <= width - 1)
      ant->pos.x++;
    else if (teleport)
      ant->pos.x = 0;
    break;
  default:
    break;
  }
}

// Fill randomly the world with dead or living cell
void populate_world(int height, int width, Cell world[height][width], int one_chance_over)
{
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      world[i][j] = random_int(1, one_chance_over) == 1 ? filled : empty;
    }
  }
}