#include "pingame.h"
int main(void)
{
	struct State_Space state_space = state_space_init(0);
	struct State *winning_state = find_winning_state(&state_space);
	print_moves_to_get_to_state(winning_state);
	state_space_destroy(&state_space);
	return 0;
}
