#include "trie.h"
#include <stdint.h>
struct State
{
	uint16_t pins;
	struct State *predecessor;
	struct State *next;
};
struct State_Queue
{
	struct State *head;
	struct State *tail;
};
struct State_Space
{
	struct State_Queue state_queue;
	struct Trie visited_states;
};


void bit_index_to_point(int index, int *OUT_x, int *OUT_y);
int point_to_bit_index(int x, int y);
int get_bit(int bit);
int is_pin_set(uint16_t pins, int bit);


struct State *state_init(uint16_t pins, struct State *predecessor);

struct State_Queue state_queue_init(void);
void state_queue_enqueue(struct State_Queue *queue, struct State *state);
struct State *state_queue_dequeue(struct State_Queue *queue);
void print_moves_to_get_to_state(struct State *state);

struct State_Space state_space_init(int hole_index);
struct State *state_space_process_first_in_queue(struct State_Space *state_space);
struct State *find_winning_state(struct State_Space *state_space);
void state_space_destroy(struct State_Space *state_space);
void print_board(uint16_t pins);

int bounds_check(int x, int y);
uint16_t operator(uint16_t prev, int pin_to_move, int d_x, int d_y);
uint16_t move_horizontal_right(uint16_t prev, int pin_to_move);
uint16_t move_horizontal_left(uint16_t prev, int pin_to_move);
uint16_t move_diagonal_up_right(uint16_t prev, int pin_to_move);
uint16_t move_diagonal_up_left(uint16_t prev, int pin_to_move);
uint16_t move_diagonal_down_right(uint16_t prev, int pin_to_move);
uint16_t move_diagonal_down_left(uint16_t prev, int pin_to_move);

extern uint16_t (*operators[])(uint16_t, int);
