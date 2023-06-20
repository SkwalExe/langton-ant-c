#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "utils.h"
#include "langton.h"
#define RED "\033[91m"
#define YELLOW "\033[93m"
#define PURPLE "\033[95m"
#define WHITE "\033[97m"
#define RESET "\033[0m"

#define VERSION "0.1.0"

// Parameters
int height = 0;
int width = 0;
int delay = 75;
bool random_generation = false;
int one_chance_over = 3;
long long moves = 0;
bool teleport = false;
bool info = false;
// --------------

void sigint_handler(int sig)
{
  if (sig == SIGINT)
  {
    print_at(0, height + 2, "Bye !\n");
    show_cursor();
    exit(0);
  }
}

void set_signal_action(void)
{
	struct sigaction act;

  memset(&act, 0, sizeof(act));
	act.sa_handler = &sigint_handler;
	sigaction(SIGINT, &act, NULL);
}


// Prints the help message
void help(void)
{
  printf("Langton's ant C\n");
  printf("%s━━━━━━━━━━━━━━━━━%s\n", PURPLE, RESET);
  printf("Author: %sSkwalExe%s\n", PURPLE, RESET);
  printf("Github: %shttps://github.com/SkwalExe/%s\n", PURPLE, RESET);
  printf("%s━━━━━━━━━━━━━━━━━%s\n", PURPLE, RESET);
  printf("C implementation of the langton's ant cellular automate algorithm\n");
  printf("%s━━━━━━━━━━━━━━━━━%s\n", PURPLE, RESET);
  printf("Options: \n");
  printf("\t%s-h, --help: %sPrints this message and exits%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-v, --version: %sPrints the program version and exits%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-d, --delay: %sSets the delay in ms between each generation step [D: 75]%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-r, --random: %sGenerate cells randomly%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-o, --over: %sWith random cell generation, a cell has one chance over [arg] to be alive [D: 3]%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-g, --generation: %sSelect from which generation to start from [D: 0]%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-s, --size: %sIf the automatic size doesn't fit your terminal, specify it manually with this parameter (ex --size 100 50 -> width of 100 and height of 50)%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-t, --teleport: %sAllow the ant to teleport to the other side of the world if it touches a border, alse if just won't move%s\n", PURPLE, YELLOW, RESET);
  printf("\t%s-i, --info: %sOptionnaly print the number of generations and the position of the ant%s\n", PURPLE, YELLOW, RESET);
}

// Prints the version
void version(void)
{
  printf("%sGame of life C => %s%s%s\n", PURPLE, YELLOW, VERSION, RESET);
}

int main(int argc, char **argv)
{
  set_signal_action();
  // Parse args ------------------------------------------------------------------------------
  while (--argc > 0)
  {
    char *arg = *++argv;

    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
    {
      help();
      return 0;
    }
    else if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0)
    {
      version();
      return 0;
    }
    else if (strcmp(arg, "--delay") == 0 || strcmp(arg, "-d") == 0)
    {
      if (argc <= 1)
      {
        printf("%s[ x ] : An argument is required after -d%s", RED, RESET);
        return 1;
      }

      delay = atoi(*++argv);
      argc--;

      if (delay <= 0)
      {
        printf("%s[ x ] : The delay must be a number higher than 0%s", RED, RESET);
        return 1;
      }
    }
    else if (strcmp(arg, "--random") == 0 || strcmp(arg, "-r") == 0)
    {
      random_generation = true;
    }
    else if (strcmp(arg, "--teleport") == 0 || strcmp(arg, "-t") == 0)
    {
      teleport = true;
    }
    else if (strcmp(arg, "--info") == 0 || strcmp(arg, "-i") == 0)
    {
      info = true;
    }
    else if (strcmp(arg, "--over") == 0 || strcmp(arg, "-o") == 0)
    {
      if (argc <= 1)
      {
        printf("%s[ x ] : An argument is required after -o%s", RED, RESET);
        return 1;
      }

      one_chance_over = atoi(*++argv);
      argc--;

      if (one_chance_over <= 1)
      {
        printf("%s[ x ] : The argumentent after -o must be higher than 1 (and not too high!!)%s", RED, RESET);
        return 1;
      }
    }
    else if (strcmp(arg, "--generation") == 0 || strcmp(arg, "-g") == 0)
    {
      if (argc <= 1)
      {
        printf("%s[ x ] : An argument is required after -g%s", RED, RESET);
        return 1;
      }

      moves = atoll(*++argv);
      argc--;

      if (moves < 0)
      {
        printf("%s[ x ] : The argumentent after -g must be 0 or positive (and not too high!!)%s", RED, RESET);
        return 1;
      }
    }
    else if (strcmp(arg, "--size") == 0 || strcmp(arg, "-s") == 0)
    {
      if (argc <= 2)
      {
        printf("%s[ x ] : Two arguments are required after -s%s", RED, RESET);
        return 1;
      }

      width = atoi(*++argv);
      argc--;
      height = atoi(*++argv);
      argc--;

      if (height <= 1 || width <= 1)
      {
        printf("%s[ x ] : The height and the width must be higher than 1 (and not too high!!)%s", RED, RESET);
        return 1;
      }
    }

    else
    {
      printf("Unknown argument: %s\n", arg);
      return 1;
    }
  }
  // ----------- End of parsing args -------------------------------------------------------

  // if the size has not been set manually by the user, determine it automatically
  if (height == 0 && width == 0)
  {
    get_terminal_size(&width, &height);
    if (info)
      height -= 2;
  }

  // Cut the width in half because a cell taxe two cols
  width /= 2;

  Cell world[height][width];
  Ant ant;
  MatrixChangeList changes;
  changes.length = 0;
  reset_ant(&ant, height, width);

  reset_world(height, width, world);

  if (random_generation)
    populate_world(height, width, world, one_chance_over);

  hide_cursor();
  if (moves != 0)
  {
    for (long long i = 0; i < moves; i++)
    {
      next_gen(height, width, world, &ant, teleport, &changes, false);
      if ((i + 1) % 1000000 == 0)
      {
        printf("\rGenerating : %lld/%lld - %d%%", i + 1, moves, (int)(100.0 * i / moves));
        fflush(stdout);
      }
    }
    printf("\n");
  }
  show_world(height, width, world, ant);
  while (true)
  {
    moves++;

    render_matrix(&changes, ant);
    print_at(0, height + 2, "");
    if (info)
      printf("\rGenerations : %lld - Ant position : X%d/%d - Y%d/%d - Ant direction : %s", moves, ant.pos.x, width, ant.pos.y, height, ant.dir == 0 ? "Up" : (ant.dir == 1 ? "Right" : (ant.dir == 2 ? "Down" : "Left")));

    next_gen(height, width, world, &ant, teleport, &changes, true);
    sleep_ms(delay);
  }
  return 0;
}