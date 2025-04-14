/**
 * File: proj2.c
 * Author: Adam Behoun, FIT
 * Login: xbehoua00
 * Date: 21.4.2024
 * 
 */

// includes that are being used in program
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

// function prototypes
void error_exit(const char *fmt, ...);
void check_argument(char *argument, int *dest);
void parser(int argc, char *argv[]);
void skibus_start();
void inc_act_print(const char *fmt, ...);
void skier(int id, int stop_id); 
void sem_clean(sem_t **semaphore);
void sem_create(sem_t **semaphore, int value);
void clean_up();
void init_resources();
void sh_var_create(int **variable, int size, int value);
void sh_var_destroy(int **variable, int size);

// stream ouput is redirected in 
FILE *fp;

// semaphores
sem_t *mutex;
sem_t *allow_to_print;
sem_t *last_skier;
sem_t *final_stop;
sem_t *bus_leave;
sem_t **bus_on_stop;

// shared variables
int *n_of_action;
int *skiers;
int *boarded;
int *skiers_waiting;
int *fork_failed;

// program arguments
int S = 0;
int stops = 0;
int capacity = 0;
int s_wait = 0;
int b_wait = 0;

/**
 * @brief destroy semaphores, deallocate shared variables, free memory and close stream
 * 
 */
void clean_up() {
    sh_var_destroy(&n_of_action, 1);
    sh_var_destroy(&skiers, stops);
    sh_var_destroy(&boarded,1);
    sh_var_destroy(&skiers_waiting,1);
    sh_var_destroy(&fork_failed,1);
    for(int i = 0; i < stops; i++) {
        sem_destroy(bus_on_stop[i]);
    } 
    sem_destroy(mutex);
    sem_destroy(last_skier);
    sem_destroy(final_stop);
    sem_destroy(bus_leave);
    free(bus_on_stop);
    fclose(fp);
}

/**
 * @brief initialize shared variables and semaphores
 * 
 */
void init_resources() {
    sh_var_create(&n_of_action, 1, 0);
    sh_var_create(&fork_failed, 1, 0);

    sh_var_create(&skiers, stops, 0);
    for(int i = 0; i < stops; i++) {
        skiers[i] = 0;
    }

    sh_var_create(&boarded, 1, 0);
    sh_var_create(&skiers_waiting, 1, S);

    sem_create(&mutex, 1);
    sem_create(&allow_to_print, 1);
    sem_create(&last_skier, 0);
    sem_create(&final_stop, 0);
    sem_create(&bus_leave, 0);

    bus_on_stop = malloc(sizeof(sem_t*) * stops);
    for(int i = 0; i < stops; i++) {
        sem_create(&bus_on_stop[i], 0);
    }
}

/**
 * @brief deallocate the shared variable
 * 
 * @param variable to deallocate
 * @param size number of ints to deallocate
 * 
 * @warning if the unmapping fails, program terminates with exit code 1
 */
void sh_var_destroy(int **variable, int size) {
    if(munmap(*variable, sizeof(int) * size) == -1) {
        error_exit("Unmapping failed.");
    }
}

/**
 * @brief initialize the shared variable
 * 
 * @param variable to initialize
 * @param size number of ints to allocate
 * @param value value that will variable store
 * 
 * @warning if the mapping fails, program terminates with exit code 1
 */
void sh_var_create(int **variable, int size, int value) {
    *variable = mmap(NULL, sizeof(int) * size,  PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
    if(*variable == MAP_FAILED) {
        error_exit("Mapping failed.");
    } 
    **variable = value;
}

/**
 * @brief initialize semaphore with value passed as parameter
 * 
 * @param semaphore to initialize
 * @param value value that will semaphore store
 * 
 * @warning if the initialization fails, program terminates with exit code 1
 */
void sem_create(sem_t **semaphore, int value) {
    *semaphore = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if (*semaphore == MAP_FAILED) {
        error_exit("Semaphore creation failed");
    }
    if (sem_init(*semaphore, 1, value) != 0) {
        munmap(*semaphore, sizeof(sem_t));
        error_exit("Semaphore initialization failed");
    }
}

/**
 * @brief destroy the semaphore
 * 
 * @param semaphore to destroy
 * 
 * @warning if destroy fails, program terminates with exit code 1
 */
void sem_clean(sem_t **semaphore) {
    if(sem_destroy(*semaphore) != 0) {
        error_exit("Semaphore cannot be destroyed");
    }

    if(munmap(*semaphore, sizeof(sem_t)) == -1) {
        error_exit("Unmaping failed");
    }
}

/**
 * @brief print output in the stream(fp) with number of action
 * that is protected by mutex semaphore and flush the stream
 * 
 * @param fmt format of the output
 * @param ... next optional arguments
 */
void inc_act_print(const char *fmt, ...) {
    sem_wait(allow_to_print);
    (*n_of_action)++;
    fprintf(fp, "%d: ", *n_of_action);
    va_list args;
    va_start (args, fmt);
    vfprintf(fp, fmt, args);
    fprintf(fp, "\n");
    va_end(args);
    fflush(fp);
    sem_post(allow_to_print);
}

/**
 * @brief print error message and exit the program with code 1
 * 
 * @param fmt format of the message error
 * @param ... optional arguments
 */
void error_exit(const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

/**
 * @brief check if passed argument is only digits and if yes,
 * store in the variable, converted to int
 * 
 * @param argument to check
 * @param dest to store
 * 
 * @warning if the argument is not all digits, program terminates with
 * code 1
 */
void check_argument(char *argument, int *dest) {
    for(size_t i = 0; i < strlen(argument); i++) {
        if(!isdigit(argument[i])){
            error_exit("Invalid numbers.");
        }
    }
    *dest = strtol(argument, NULL, 10);
}

/**
 * @brief parsing arguments and calling function check_argument on each of them
 * also check if the numbers are valid
 * 
 * @param argc number of command-line arguments
 * @param argv two-dimensional array with command-line arguments
 * 
 * @warning if number of arguments in not 6, program terminates
 * with exit code 1
 */
void parser(int argc, char *argv[]) {
    if(argc != 6) 
        error_exit("Wrong arguments.");

    check_argument(argv[1], &S);
    check_argument(argv[2], &stops);
    check_argument(argv[3], &capacity);
    check_argument(argv[4], &s_wait);
    check_argument(argv[5], &b_wait);

    if(S <= 0 || S >= 20000) {
        error_exit("Invalid number of L.");
    } else if(stops <= 0 || stops > 10) {
        error_exit("Invalid number of bus S.");
    } else if(capacity < 10  || capacity > 100) {
        error_exit("Invalid number of K.");
    } else if (s_wait < 0 || s_wait > 10000) {
        error_exit("Invalid number of TL.");
    } else if (b_wait < 0 || b_wait > 10000) {
        error_exit("Invalid number of TB.");
    }
}

/**
 * @brief simulates skibus behaviour. skibus goes through all the stops
 * that are being passed as command-line argument and picking up skiers.
 * when it arrives at final stop, then skiers can get off. If any skier
 * is still waiting for bus, then it goes for another round.
 * 
 */
void skibus_start() {
    int bus_full = 0; // detect, when the bus is full
    for(int i = 1; i <= stops; i++) { // iterate through all the stops

        // generate random delay that takes the bus to arrive at next stop
        usleep(rand() % (b_wait + 1));
        inc_act_print("BUS: arrived to %d", i);

        // if there are any skiers at the stop and the bus is not full,
        // then signal the semaphore that skiers can board and we wait,
        // till the last skier signal us that we can go on
        sem_wait(mutex);
        if(skiers[i-1] > 0 && bus_full != 1) {
            sem_post(bus_on_stop[i-1]);
            sem_wait(last_skier);
        }

        // if the bus is full, set the variable bus_full
        if(*boarded == capacity) {
            bus_full = 1;
        }
        sem_post(mutex);

        inc_act_print("BUS: leaving %d", i);

    }

    inc_act_print("BUS: arrived to final");

    sem_wait(mutex);
    // if there is someone in the bus, start the sequence by signaling the 
    // final stop, so first skier can go skiing, and wait till everyone is off
    if(*boarded > 0) {
        sem_post(final_stop);
        sem_post(mutex);
        sem_wait(bus_leave);
    } else {
        sem_post(mutex);
    }

    bus_full = 0;

    inc_act_print("BUS: leaving final");

}

/**
 * @brief simulates skier behaviour. when skier starts, he go to have breakfast
 * and it takes random time. Then he will arrive at a random bus stop and wait there for skibus.
 * after the bus arrives, skier signals the next skier at the bus stop or he signal the skibus
 * that he is last one at this bus stop and that bus can carry on. Skier then waits till bus
 * will arrive at final stop, then he will get off and go skiing
 * 
 * @param id id of the skier
 * @param stop_id id of the bus stop
 */
void skier(int id, int stop_id) {
    inc_act_print("L %d: started", id);

    // generate random amout of time that skier will have breakfast
    usleep(rand() % (s_wait + 1));

    inc_act_print("L %d: arrived to %d", id, stop_id);

    sem_wait(mutex);
    (skiers[stop_id-1]) ++;
    sem_post(mutex);
   
    // wait at the bus stop until the bus arrives
    sem_wait(bus_on_stop[stop_id-1]);

    inc_act_print("L %d: boarding", id);

    (skiers[stop_id-1]) --; // decrement number of skiers at the bus stop
    (*boarded) ++; // increment people on board 

    // if the bus stop is empty or the bus is full, signal the bus to carry on
    if(skiers[stop_id-1] == 0 || *boarded == capacity) {
        sem_post(last_skier);
    } else {
        // else, signal the next skier at that bus stop to get on
        sem_post(bus_on_stop[stop_id-1]);
    }

    // skier waits till he is on the final stop
    sem_wait(final_stop);

    inc_act_print("L %d: going to ski", id);

    (*boarded) --; // decrement the skiers in the bus
    if(*boarded == 0) {
        sem_post(bus_leave); // if the bus is empty, tell the bus to continue
    } else {
        sem_post(final_stop); // otherwise allow next skier to get off the bus
    }
}

int main(int argc, char *argv[]) {
    // open the file "proj2.out" that we will print output in
    fp = fopen("proj2.out", "w");
    if(fp == NULL) {
        error_exit("Failed to open file");
    }

    // parse the arguments
    parser(argc, argv);

    // initialize resources (semaphores and variables)
    srand(time(NULL));
    init_resources();

    // generate the skibus process
    pid_t skibus_id = fork();
    if(skibus_id == 0) {
        inc_act_print("BUS: started");
        skibus_start();

        // skibus will be running till any skier wait on breakfast or at the bus stop
        while(*skiers_waiting > 0) {
            skibus_start();
        }

        inc_act_print("BUS: finish");
        clean_up();
        exit(0);
    } else if(skibus_id < 0) {
        clean_up();
        error_exit("Fork failed");
    }

    // initialize array to store childs pids
    pid_t childs_pids[S];
    for(int i = 0; i<S; i++) {
        childs_pids[i] = 0;
    }

    // generate (S) skier processes or stop if some fork failed
    for(int i = 1; i <= S; i++) {
        pid_t id = fork();

        if(id>0){
            childs_pids[i-1] = id; // store the childs id in the array
        }

        int stop = rand() %stops + 1; // generate random bus stop

        if(id == 0) {
            skier(i, stop); 
            sem_wait(mutex);
            (*skiers_waiting) --; // if the process ends, decrement waiting skiers
            sem_post(mutex);

            childs_pids[i-1] = 0; // indicate that the process ended

            clean_up();
            exit(0);
        } else if(id < 0) {
            // kill each child process
            for(int j = 0; j < i; j++) {
                if(childs_pids[j] != 0){
                    kill(childs_pids[j], SIGTERM);
                }
            }
            // kill the skibus process
            kill(skibus_id, SIGTERM);

            // clean and exit
            clean_up();
            error_exit("Fork failed.");
        }
    }
    

    // parent process waits on child processes
    while(wait(NULL) > 0);

    clean_up();

    return 0;
}