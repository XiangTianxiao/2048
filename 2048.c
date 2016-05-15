/* A version for HP 39gs graphing calculator
 *
 * see https://github.com/XiangTianxiao/2048
 *
 */

/* Clone of the 2048 sliding tile puzzle game. (C) Wes Waugh 2014
*
* This program is free software, licensed under the GPLv3. Check the
* LICENSE file for details.
*/

#include <hpgcc49.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

#define NROWS 4
#define NCOLS NROWS

typedef unsigned int tile_t;

struct game_t
{
	int turns, score;
	tile_t board[NROWS][NCOLS];
};

int my_random()
{
	return random()*10e10;
}

// place_tile() returns 0 if it did place a tile and -1 if there is no open
// space.
int place_tile(struct game_t *game)
{
	// lboard is the "linear board" -- no need to distinguish rows/cols
	tile_t *lboard = (tile_t *)game->board;
	int i, num_zeros = 0;

	// Walk the board and count the number of empty tiles
	for (i = 0; i < NROWS * NCOLS; i++)
	{
		num_zeros += lboard[i] ? 0 : 1;
	}

	if (!num_zeros)
	{
		return -1;
	}

	// Choose the insertion point
	int loc = my_random() % num_zeros;

	// Find the insertion point and place the new tile
	for (i = 0; i < NROWS * NCOLS; i++)
	{
		if (!lboard[i] && !(loc--))
		{
			lboard[i] = my_random() % 10 ? 1 : 2;
			return 0;
		}
	}
	return -1;
}

void print_tile(int tile)
{
	if (tile)
	{
		printf("%4d", 1 << tile);
	}
	else
	{
		printf("   .");
	}
}

void print_game(struct game_t *game)
{
	int r, c;
	gotoxy(0, 0);
	printf("Score: %6d  Turns: %4d", game->score, game->turns);
	for (r = 0; r < NROWS; r++)
	{
		for (c = 0; c < NCOLS; c++)
		{
			gotoxy(5 * c, r + 2);
			print_tile(game->board[r][c]);
		}
	}
}

int combine_left(struct game_t *game, tile_t row[NCOLS])
{
	int c, did_combine = 0;
	for (c = 1; c < NCOLS; c++)
	{
		if (row[c] && row[c - 1] == row[c])
		{
			row[c - 1]++;
			row[c] = 0;
			game->score += 1 << (row[c - 1] - 1);
			did_combine = 1;
		}
	}
	return did_combine;
}

// deflate_left() returns nonzero if it did deflate, and 0 otherwise
int deflate_left(tile_t row[NCOLS])
{
	tile_t buf[NCOLS] = { 0 };
	tile_t *out = buf;
	int did_deflate = 0;

	int in;
	for (in = 0; in < NCOLS; in++)
	{
		if (row[in] != 0)
		{
			*out++ = row[in];
			did_deflate |= buf[in] != row[in];
		}
	}

	memcpy(row, buf, sizeof(buf));
	return did_deflate;
}

void rotate_clockwise(struct game_t *game)
{
	tile_t buf[NROWS][NCOLS];
	memcpy(buf, game->board, sizeof(game->board));

	int r, c;
	for (r = 0; r < NROWS; r++)
	{
		for (c = 0; c < NCOLS; c++)
		{
			game->board[r][c] = buf[NCOLS - c - 1][r];
		}
	}
}

void move_left(struct game_t *game)
{
	int r, ret = 0;
	for (r = 0; r < NROWS; r++)
	{
		tile_t *row = &game->board[r][0];
		ret |= deflate_left(row);
		ret |= combine_left(game, row);
		ret |= deflate_left(row);
	}

	game->turns += ret;
}

void move_right(struct game_t *game)
{
	rotate_clockwise(game);
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
}

void move_up(struct game_t *game)
{
	rotate_clockwise(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
}

void move_down(struct game_t *game)
{
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
}

// Pass by value because this function mutates the game
int lose_game(struct game_t test_game)
{
	int start_turns = test_game.turns;
	move_left(&test_game);
	move_up(&test_game);
	move_down(&test_game);
	move_right(&test_game);
	return test_game.turns == start_turns;
}

void init_curses()
{
	clear_screen(); // curses init
	gotoxy(0, 0);
}

int max_tile(tile_t *lboard)
{
	int i, ret = 0;
	for (i = 0; i < NROWS * NCOLS; i++)
	{
		ret = max(ret, lboard[i]);
	}
	return ret;
}

int main()
{
	init_curses();

	const char *exit_msg = "";

	struct game_t game = { 0 };
	int last_turn = game.turns;

	place_tile(&game);
	place_tile(&game);

	gotoxy(0, 7);
	printf("Press ENTER to quit.");

	int cur_key;

	while (1)
	{
		print_game(&game);

		if (lose_game(game))
		{
			exit_msg = "lost";
			goto lose;
		}

		last_turn = game.turns;

		switch (cur_key = keyb_getkey(1))
		{

		case 22:
			move_left(&game);
			break;
		case 38:
			move_down(&game);
			break;
		case 6:
			move_up(&game);
			break;
		case 54:
			move_right(&game);
			break;
		case 96:
			exit_msg = "quit";
			goto end;

		}

		if (last_turn != game.turns)
			place_tile(&game);
	}

lose:
	gotoxy(0, 7);
	printf("You lose! Press ENTER to quit.");
	while (keyb_getkey(1) != 96);
end:
	gotoxy(0, 7);
	printf("You %s after scoring %d points in %d turns, "
		"with largest tile %d\n",
		exit_msg, game.score, game.turns,
		1 << max_tile((tile_t *)game.board));
	sys_sleep(3000);
	return 0;
}

