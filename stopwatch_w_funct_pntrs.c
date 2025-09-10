#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
	EVT_START,
	EVT_PAUSE,
	EVT_CONTINUE,
	EVT_STOP,

    EVT_INVALID,
    EVT_QUIT
} Event;

typedef enum {
    START,
	PAUSE,
	CONTINUE,
	STOP,
    QUIT,
    NOCHANGE
} StateCode;

// Function prototypes for states
int st_start(void);
int st_pause(void);
int st_continue(void);
int st_stop(void);

// Array of pointers to each state function, corresponding to each StateCode
int (*States[])(void) = {
    st_start,
    st_pause,
    st_continue,
    st_stop
    // QUIT will be skipped, no need for a value
    // NOCHANGE will be skipped, no need a value here
};

typedef struct {
	StateCode curr_state;
    Event trigger;
	StateCode next_state;
} StateTransition;

StateTransition state_transition_table[4][4] = {
    {{START, EVT_START, NOCHANGE}, {START, EVT_PAUSE, PAUSE}, {START, EVT_CONTINUE, NOCHANGE}, {START, EVT_STOP, STOP}},
    {{PAUSE, EVT_START, NOCHANGE}, {PAUSE, EVT_PAUSE, NOCHANGE}, {PAUSE, EVT_CONTINUE, CONTINUE}, {PAUSE, EVT_STOP, STOP}},
    {{CONTINUE, EVT_START, NOCHANGE}, {CONTINUE, EVT_PAUSE, PAUSE}, {CONTINUE, EVT_CONTINUE, NOCHANGE}, {CONTINUE, EVT_STOP, STOP}},
    {{STOP, EVT_START, START}, {STOP, EVT_PAUSE, NOCHANGE}, {STOP, EVT_CONTINUE, NOCHANGE}, {STOP, EVT_STOP, NOCHANGE}}
};

// Global vars for tracking time
time_t start_time;
time_t current_time;
time_t elapsed_time = 0;
time_t total_time = 0;
bool is_running = false;


int st_start(void) {
    // ACTION_START
    start_time = time(0);
    elapsed_time = 0;
    total_time = 0;
    is_running = true;
    
    printf("Timer started.\n");

    return 0;
}

int st_pause(void) {
    // ACTION_PAUSE
    current_time = time(0);			
    elapsed_time = current_time - start_time;	//get time since start/last continue
    total_time += elapsed_time;
    is_running = false;

    printf("Timer paused. Elapsed time: %ld seconds\n", total_time);

    return 0;
}

int st_continue(void) {
    // ACTION_CONTINUE
    start_time = time(0);
    is_running = true;
    
    printf("Timer continued.\n");

    return 0;
}

int st_stop(void) {
    // ACTION_STOP
    if (is_running) {	//stopped after START or CONTINUE
        current_time = time(0);
        elapsed_time = current_time - start_time;	//get time since start/last continue
        total_time += elapsed_time;
    }	//else, stopped after PAUSE, total_time carried over
    
    printf("Timer stopped. Total time: %ld seconds.\n", total_time);

    return 0;
}

/* Translates input to event*/
Event get_next_event(void) {
    int response;

    printf("Choose an action: ");
    int result = scanf("%d", &response);

    if (result != 1) {
        printf("Invalid input. Try again.\n");
        return EVT_INVALID; // Let the main loop call for the function again
    }

    if (response == -1) {
        printf("Quitting program...\n");
        return EVT_QUIT;
    }

    switch (response) {
        case 0:
            return EVT_START;
        case 1:
            return EVT_PAUSE;
        case 2:
            return EVT_CONTINUE;
        case 3:
            return EVT_STOP;
        default:
            printf("Invalid input\n");
            return EVT_INVALID;
    }
}

int main(void) {
    Event current_event;
    StateCode current_state_code = STOP;
    int (*next_state)(void); // function pointer to the state function to be run

    printf( "Hello!\n"
		"Enter \"0\" to start the timer\n"
		"Enter \"1\" to pause\n"
		"Enter \"2\" to continue\n"
		"Enter \"3\" to stop\n"
		"And enter \"-1\" to quit\n"
	);

    while ((current_event = get_next_event()) != EVT_QUIT) {
        // 1. Use current_event and current_state to get next_state via state_transition table
        StateCode next_state_code = state_transition_table[current_state_code][current_event].next_state;
        // 1a. If next state is NOCHANGE, skip 2
        if (next_state_code == NOCHANGE) {
            printf("No action to perform.\n");
            continue;
        }
        // 2. Call next state
        next_state = States[next_state_code];
        next_state();
        // 3. Update current_state_code for next loop
        current_state_code = next_state_code;
    }
}