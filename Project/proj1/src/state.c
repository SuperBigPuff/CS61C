#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  // My implementation as below
  // malloc space first
  game_state_t* default_state = malloc(sizeof(game_state_t));
  // rows def
  default_state -> num_rows =18;
  // board, a double pointer, we should have 18 of it
  default_state -> board = malloc(sizeof(char*) * default_state -> num_rows);
  // malloc space for each row, and fill with spaces
  for (int i = 0; i < default_state -> num_rows; i++) {
    default_state -> board[i] = malloc(sizeof(char) * 21); // 20 chars + null terminator
    default_state -> board[i][20] = '\0'; // null terminator
  }
  // hardcode the board， stored in static
  const char *rows[18] = {
    "####################",
    "#                  #",
    "# d>D    *         #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "#                  #",
    "####################",
  };
  // pass this board to the default_state
  for (int i = 0; i < default_state -> num_rows; i++) {
    strcpy(default_state -> board[i], rows[i]);
  }
  // snakes def
  // nums
  default_state-> num_snakes = 1;
  // pointer
  default_state -> snakes = malloc(sizeof(snake_t) * 1); // only one snake
  // items: finish snake and fruit position， fruit already in board
  default_state->snakes[0].tail_row = 2;
  default_state->snakes[0].tail_col = 2;
  default_state->snakes[0].head_row = 2;
  default_state->snakes[0].head_col = 4;
  default_state->snakes[0].live = true;

  return default_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  // items to free: default_state pointer, board, snakes
  // free rows
  for (int i = 0; i < state -> num_rows; i++) {
    free(state -> board[i]);
  }
  // free board
  free(state -> board);
  // free snakes
  free(state -> snakes);
  // free state pointer
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state -> num_rows; i++) {
    fprintf(fp, "%s\n", state -> board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  // wasd are the tail characters
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  // WASDx are the head characters
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == '>' || c == 'v') {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
    if (c == '^') {
      return 'w';
    } else if (c == '<') {
      return 'a';
    } else if (c == 'v') {
      return 's';
    } else if (c == '>') {
      return 'd';
    }
    return '?'; 
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
    if (c == 'W') {
      return '^';
    } else if (c == 'A') {
      return '<';
    } else if (c == 'S') {
      return 'v';
    } else if (c == 'D') {
      return '>';
    }
    return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state -> snakes[snum];
  char head = get_board_at(state, snake.head_row, snake.head_col);
  unsigned int next_row = get_next_row(snake.head_row, head);
  unsigned int next_col = get_next_col(snake.head_col, head);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state -> snakes[snum];
  char head = get_board_at(state, snake -> head_row, snake -> head_col);
  unsigned int next_row = get_next_row(snake -> head_row, head);
  unsigned int next_col = get_next_col(snake -> head_col, head);
  // update the head on the board
  set_board_at(state, next_row, next_col, head); // head
  set_board_at(state, snake -> head_row, snake -> head_col, head_to_body(head)); // body
  // update the head in the snake struct
  snake -> head_row = next_row;
  snake -> head_col = next_col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state -> snakes[snum];
  char tail = get_board_at(state, snake -> tail_row, snake -> tail_col);
  unsigned int next_row = get_next_row(snake -> tail_row, tail);
  unsigned int next_col = get_next_col(snake -> tail_col, tail);
  // update the tail on the board
  set_board_at(state, snake -> tail_row, snake -> tail_col, ' '); // tail
  set_board_at(state, next_row, next_col, body_to_tail(get_board_at(state, next_row, next_col))); // body
  // update the tail in the snake struct
  snake -> tail_row = next_row;
  snake -> tail_col = next_col;
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state -> num_snakes; i++) {
    char next = next_square(state, i);
    // normal case
    if (next == ' ') {
      update_head(state, i);
      update_tail(state, i);
    }
    else if (next == '*') {
      update_head(state, i); // do not update tail
      add_food(state); // add one more food
    }
    else if (next == '#' || is_snake(next)) {
      // snake dies
      state -> snakes[i].live = false;
      // set head to 'x'
      set_board_at(state, state -> snakes[i].head_row, state -> snakes[i].head_col, 'x');
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  // create a buffer to store the line
  char buffer[128]; // enough space on stack
  // heap space for the line to return
  char *line = NULL;
  char *new_line = NULL;
  // length
  int str_length = 0;
  int block_length = 0;
  // read a line from the file
  while(1){
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
      return NULL; // read nothing
    }
    block_length = strlen(buffer);
    // allocate new space for the block
    new_line = realloc(line, str_length + block_length + 1); 
    strcpy(new_line + str_length, buffer);
    line = new_line; 
    str_length = str_length + block_length; 
  }
  return line;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  return NULL;
}
