#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
	START,
	PAUSE,
	CONTINUE,
	STOP
} state;

typedef enum {
	ACTION_NA,
	ACTION_START,
	ACTION_PAUSE,
	ACTION_CONTINUE,
	ACTION_STOP
} action;

typedef enum {
	EVT_START,
	EVT_PAUSE,
	EVT_CONTINUE,
	EVT_STOP
} event;

typedef struct {
	state next_state;	// state to be transitioned to
	action action_name; // action to be performed after transitioning
} transition;

transition state_transition_table[4][4] = {
	{ {START, ACTION_NA}, {PAUSE, ACTION_PAUSE}, {START, ACTION_NA}, {STOP, ACTION_STOP} },
	{ {PAUSE, ACTION_NA}, {PAUSE, ACTION_NA}, {CONTINUE, ACTION_CONTINUE}, {STOP, ACTION_STOP} },
	{ {CONTINUE, ACTION_NA}, {PAUSE, ACTION_PAUSE}, {CONTINUE, ACTION_NA}, {STOP, ACTION_STOP} },
	{ {START, ACTION_START}, {STOP, ACTION_NA}, {STOP, ACTION_NA}, {STOP, ACTION_NA} }
};

state current_state = STOP;	// initial state
time_t start_time;
time_t current_time;
time_t elapsed_time = 0;
time_t total_time = 0;
bool is_running = false;

action lookup(event ev){
	transition next_state_action = state_transition_table[current_state][ev];

	action next_action = next_state_action.action_name;
	if (next_action == ACTION_NA) {
		return ACTION_NA;
	}

	current_state = next_state_action.next_state;

	return next_action;
}

void execute_action(action next_action) {
	switch (next_action) {
		case ACTION_START:
			start_time = time(0);
			elapsed_time = 0;
			total_time = 0;
			is_running = true;
			
			printf("Timer started.\n");
			break;
		case ACTION_PAUSE:
			current_time = time(0);			
			elapsed_time = current_time - start_time;	//get time since start/last continue
			total_time += elapsed_time;
			is_running = false;

			printf("Timer paused. Elapsed time: %ld seconds\n", total_time);
			break;
		case ACTION_CONTINUE:
			start_time = time(0);
			is_running = true;
			
			printf("Timer continued.\n");
			break;
		case ACTION_STOP:
			if (is_running) {	//stopped after START or CONTINUE
				current_time = time(0);
				elapsed_time = current_time - start_time;	//get time since start/last continue
				total_time += elapsed_time;
			}	//else, stopped after PAUSE, total_time carried over
			
			printf("Timer stopped. Total time: %ld seconds.\n", total_time);
			break;
		default:
			printf("No action to perform.\n");
			break;
	}
}

int main(void) {
	int response;
	action next_action;

	printf( "Hello!\n"
		"Enter \"0\" to start the timer\n"
		"Enter \"1\" to pause\n"
		"Enter \"2\" to continue\n"
		"Enter \"3\" to stop\n"
		"And enter \"-1\" to quit\n"
	);

	for (;;) {
		printf("Choose an action: ");
		int result = scanf("%d", &response);

		if (result != 1) {
			printf("Invalid input. Try again.\n");
			continue;
		}

		switch (response) {
			case 0:
				next_action = lookup(EVT_START);
				break;
			case 1:
				next_action = lookup(EVT_PAUSE);
				break;
			case 2:
				next_action = lookup(EVT_CONTINUE);
				break;
			case 3:
				next_action = lookup(EVT_STOP);
				break;
			case -1:
				printf("Quitting program...\n");
				return 0;
			default:
				printf("Invalid input.\n");
				break;
		}

		execute_action(next_action);
	}

	return 0;	// shouldn't actually be reached, but here jic
}

