#include "pingame.h"

uint16_t (*operators[])(uint16_t, int) = {
	&move_horizontal_left, &move_horizontal_right, 
	&move_diagonal_up_left, &move_diagonal_up_right,
	&move_diagonal_down_left, &move_diagonal_down_right
};

struct State *state_init(uint16_t pins, struct State *predecessor)
{
	struct State *st = malloc(sizeof(struct State));
	if(st == NULL)
	{
		fprintf(stderr, "fuggggg\n");
		exit(5);
	}

	st->pins = pins;
	st->predecessor = predecessor;
	return st;
}

struct State_Queue state_queue_init(void)
{
	struct State_Queue qu;
	qu.head = qu.tail = NULL;
	return qu;
}

void print_moves_to_get_to_state(struct State *state)
{
	if(state->predecessor != NULL)
	{
		print_moves_to_get_to_state(state->predecessor);
	}

	print_board(state->pins);
	printf("\n");
}
void print_board(uint16_t pins)
{
	int pos = 0;
	for(int row = 1; row <= 5; row++)
	{
		for(int i = 0; i < 5 - row; i++)
		{
			printf(" ");
		}
		for(int col = 1; col <= row; col++)
		{
			int pin = (pins & (1 << pos)) != 0;
			printf("%c ", pin ? '+' : 'o');
			pos++;
		}
		printf("\n");
	}
}
void state_queue_enqueue(struct State_Queue *queue, struct State *state)
{
	state->next = NULL;
	if(queue->head == NULL)
	{
		queue->head = queue->tail = state;
		return;
	}

	queue->tail->next = state;
	queue->tail = state;
}

struct State *state_queue_dequeue(struct State_Queue *queue)
{
	struct State *out = queue->head;
	if(queue->head == NULL)
	{
		return NULL;
	}
	
	if(queue->tail == queue->head)
	{
		queue->tail = queue->head = NULL;
		return out;
	}

	queue->head = queue->head->next;
	return out;
}

struct State_Space state_space_init(int hole_index)
{
	struct State_Space ss;
	ss.state_queue = state_queue_init();
	ss.visited_states = trie_init(sizeof(uint16_t));

	uint16_t pins = -1;
	pins ^= (1 << hole_index);
	struct State *initial = state_init(pins, NULL);
	state_queue_enqueue(&(ss.state_queue), initial);
	trie_insert(&(ss.visited_states), &(initial->pins), initial);
	return ss;
}

void state_space_destroy(struct State_Space *state_space)
{
	while(state_space->state_queue.head != NULL)
	{
		struct State *temp = state_queue_dequeue(&(state_space->state_queue));
		struct State *trie_node = trie_search(&(state_space->visited_states), &(temp->pins));
		if(trie_node == NULL)
		{
			free(temp);
		}
	}
	trie_destroy(&(state_space->visited_states), &free);
}
struct State *state_space_process_first_in_queue(struct State_Space *state_space)
{
	struct State *first_in_queue = state_queue_dequeue(&(state_space->state_queue));
	uint16_t new_pins;
	int i, j;

	uint16_t victory_check = 1;
	for(i = 0; i < 15; i++)
	{
		if(victory_check == first_in_queue->pins)
		{
			return first_in_queue;
		}
		victory_check <<= 1;
	}

	for(i = 0; i < 6; i++)
	{
		for(j = 0; j < 15; j++)
		{
			new_pins = (*(operators[i]))(first_in_queue->pins, j);
			if(new_pins != 0)
			{
				void *potentially_visited_state = trie_search(&(state_space->visited_states), &(new_pins));
				if(potentially_visited_state == NULL)
				{
					struct State *new_state = state_init(new_pins, first_in_queue);
					state_queue_enqueue(&(state_space->state_queue), new_state); 
					trie_insert(&(state_space->visited_states), &new_pins, new_state);
				}
			}
		}
	}
	return NULL;
}

void bit_index_to_point(int index, int *OUT_x, int *OUT_y)
{
	int count = 0;
	int x = 0, y = 0;

	while(y < 5)
	{
		while(x <= y)
		{
			if(count == index)
			{
				*OUT_x = x;
				*OUT_y = y;
				return;
			}
			count++;
			x++;
		}
		x = 0;
		y++;
	}

	*OUT_x = *OUT_y = -1;
}

int point_to_bit_index(int x, int y)
{
	x++;
	y++;
	return (y * (y - 1) / 2 + x) - 1;
}

int get_bit(int bit)
{
	uint16_t base = 1;
	return base << bit;
}

int is_pin_set(uint16_t pins, int bit)
{
	return pins & get_bit(bit);
}

int bounds_check(int x, int y)
{
	return y >= 0 && y <= 4 && x >= 0 && x <= y;
}

uint16_t operator(uint16_t prev, int pin_to_move, int d_x, int d_y)
{
	if(!is_pin_set(prev, pin_to_move))
	{
		return 0;
	}

	int x_curr, y_curr;
	bit_index_to_point(pin_to_move, &x_curr, &y_curr);

	int x_to_jump = x_curr + d_x, y_to_jump = y_curr + d_y;
	int index_to_jump = point_to_bit_index(x_to_jump, y_to_jump);

	if(!bounds_check(x_to_jump, y_to_jump) || !is_pin_set(prev, index_to_jump))
	{
		return 0;
	}

	int x_landing = x_to_jump + d_x, y_landing = y_to_jump + d_y;
	int index_landing = point_to_bit_index(x_landing, y_landing);

	if(!bounds_check(x_landing, y_landing) || is_pin_set(prev, index_landing))
	{
		return 0;
	}
	uint16_t out = (prev & ~get_bit(pin_to_move) & ~get_bit(index_to_jump)) | get_bit(index_landing);
	out ^= (1 << 15);
	return out;
}
struct State *find_winning_state(struct State_Space *state_space)
{
	while(state_space->state_queue.head != NULL)
	{
		struct State *candidate = state_space_process_first_in_queue(state_space);
		if(candidate != NULL)
		{
			return candidate;
		}
	}
	printf("I couldn't win D:\n");
	exit(4);
	return NULL;
}
uint16_t move_horizontal_right(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, 1, 0);
}

uint16_t move_horizontal_left(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, -1, 0);
}

uint16_t move_diagonal_up_right(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, 0, -1);
}

uint16_t move_diagonal_up_left(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, -1, -1);
}

uint16_t move_diagonal_down_right(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, 1, 1);
}

uint16_t move_diagonal_down_left(uint16_t prev, int pin_to_move)
{
	return operator(prev, pin_to_move, 0, -1);
}
