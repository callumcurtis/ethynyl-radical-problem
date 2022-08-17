/*
 * kosmos.c
 *
 * By: Callum Curtis
 *
 * Solves the ethynyl-radical problem by forming 2:1 groups of carbon and
 * hydrogen threads.
 */

#include "kosmos.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framework/logging.h"

#define MAX_ATOM_NAME_LEN 10
#define SHARE_BETWEEN_THREADS 0
#define NUM_COMBINING_C 2
#define NUM_COMBINING_H 1

long num_atoms;
int num_c;
int num_h;

static int num_radicals;
static int num_free_c;
static int num_free_h;

// Ids of the atoms participating in the next reaction
static int combining_c1;
static int combining_c2;
static int combining_h;

// Points to combining_c1 or combining_c2 to specify the position
// of the carbon thread that is next to join the reaction
static int *destination_of_next_combining_c;

static sem_t mutex;
static sem_t wait_c;
static sem_t wait_h;
static sem_t staging_area;
static sem_t reaction;

static void empty_the_kosmos();
static void initialize_semaphores();
static void destroy_semaphores();
static void add_free_hydrogen();
static void add_free_carbon();
static bool can_react();
static void add_combining_hydrogen(int id);
static void add_combining_carbon(int id);
static void add_second_combining_carbon(int id);
static void signal_first_combining_carbon_and_wait_until_ready();
static void signal_second_combining_carbon_and_wait_until_ready();
static void signal_combining_hydrogen_and_wait_until_ready();
static void react(char *trigger);
static void try_adding_combining_hydrogen_when_signaled(int id);
static void try_adding_combining_carbon_when_signaled(int id);
static void wait_until_finished_reactions();
static bool finished_reactions();
static void release_excess_atoms();
static void release_excess_hydrogen();
static int num_excess_hydrogen();
static void release_excess_carbon();
static int num_excess_carbon();

void kosmos_init() {
    empty_the_kosmos();
    initialize_semaphores();
}

void empty_the_kosmos() {
    num_radicals = 0;
    num_free_c = 0;
    num_free_h = 0;
}

void initialize_semaphores() {
    sem_init(&mutex, SHARE_BETWEEN_THREADS, 1);
    sem_init(&wait_c, SHARE_BETWEEN_THREADS, 0);
    sem_init(&wait_h, SHARE_BETWEEN_THREADS, 0);
    sem_init(&staging_area, SHARE_BETWEEN_THREADS, 0);
    sem_init(&reaction, SHARE_BETWEEN_THREADS, 0);
}

/*
 * Triggers a reaction immediately if enough free atoms, otherwise waits to
 * try and participate in a future reaction
 */
void *h_ready( void *arg )
{
    int id = *((int *)arg);
    free(arg);

    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "h%03d", id);

    sem_wait(&mutex);

#ifdef VERBOSE
    printf("%s now exists\n", name);
#endif

    add_free_hydrogen();
    if(can_react()) {
        add_combining_hydrogen(id);
        signal_first_combining_carbon_and_wait_until_ready();
        signal_second_combining_carbon_and_wait_until_ready();
        react(name);
        sem_post(&mutex);
    } else {
        sem_post(&mutex);
        try_adding_combining_hydrogen_when_signaled(id);
    }

    return NULL;
}

/*
 * Triggers a reaction immediately if enough free atoms, otherwise waits to
 * try and participate in a future reaction
 */
void *c_ready( void *arg )
{
    int id = *((int *)arg);
    free(arg);

    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "c%03d", id);

    sem_wait(&mutex);

#ifdef VERBOSE
    printf("%s now exists\n", name);
#endif

    add_free_carbon();
    if(can_react()) {
        add_second_combining_carbon(id);
        signal_first_combining_carbon_and_wait_until_ready();
        signal_combining_hydrogen_and_wait_until_ready();
        react(name);
        sem_post(&mutex);
    } else {
        sem_post(&mutex);
        try_adding_combining_carbon_when_signaled(id);
    }

    return NULL;
}

bool can_react() {
    return (num_free_c >= NUM_COMBINING_C) && (num_free_h >= NUM_COMBINING_H);
}

void add_second_combining_carbon(int id) {
    combining_c2 = id;
}

void signal_first_combining_carbon_and_wait_until_ready() {
    destination_of_next_combining_c = &combining_c1;
    sem_post(&wait_c);
    sem_wait(&staging_area);
}

void signal_second_combining_carbon_and_wait_until_ready() {
    destination_of_next_combining_c = &combining_c2;
    sem_post(&wait_c);
    sem_wait(&staging_area);
}

void signal_combining_hydrogen_and_wait_until_ready() {
    sem_post(&wait_h);
    sem_wait(&staging_area);
}

/*
 * Create a radical by consuming the prepared atoms and treating trigger
 * as the name of the atom that caused the reaction
 */
void react(char *trigger) {
    num_radicals++;
    num_free_c -= NUM_COMBINING_C;
    num_free_h -= NUM_COMBINING_H;
    sem_post(&reaction);
#ifdef VERBOSE
    fprintf(stdout, "An ethynyl radical was made: c%03d  c%03d  h%03d\n",
		combining_c1, combining_c2, combining_h);
#endif
    kosmos_log_add_entry(num_radicals, combining_c1, combining_c2, combining_h, trigger);
}

void add_free_carbon() {
    num_free_c++;
}

void add_free_hydrogen() {
    num_free_h++;
}

void try_adding_combining_hydrogen_when_signaled(int id) {
    sem_wait(&wait_h);
    if(!finished_reactions()) {
        add_combining_hydrogen(id);
    }
    sem_post(&staging_area);
}

void try_adding_combining_carbon_when_signaled(int id) {
    sem_wait(&wait_c);
    if(!finished_reactions()) {
        add_combining_carbon(id);
    }
    sem_post(&staging_area);
}

void add_combining_hydrogen(int id) {
    combining_h = id;
}

void add_combining_carbon(int id) {
    *destination_of_next_combining_c = id;
}

/*
 * Wait until the expected number of radicals have been created, then clean-up,
 * dump the contents of the log, and exit
 */
void wait_to_terminate() {
    wait_until_finished_reactions();
    release_excess_atoms();
    destroy_semaphores();
    kosmos_log_dump();
    exit(0);
}

void wait_until_finished_reactions() {
    bool done_waiting = false;

    // Account for maximum number of reactions possibly being 0
    sem_wait(&mutex);
    done_waiting = finished_reactions();
    sem_post(&mutex);

    while(!done_waiting) {
        sem_wait(&reaction);
        sem_wait(&mutex);
        done_waiting = finished_reactions();
        sem_post(&mutex);
    }
}

bool finished_reactions() {
    int max_radicals = (num_h < num_c / 2 ? num_h : (int)(num_c / 2));
    return num_radicals == max_radicals;
}

void release_excess_atoms() {
    release_excess_hydrogen();
    release_excess_carbon();
}

void release_excess_hydrogen() {
    for(int i = 0; i < num_excess_hydrogen(); i++) {
        sem_post(&wait_h);
        sem_wait(&staging_area);
    }
}

int num_excess_hydrogen() {
    return num_h - (num_radicals * NUM_COMBINING_H);
}

void release_excess_carbon() {
    for(int i = 0; i < num_excess_carbon(); i++) {
        sem_post(&wait_c);
        sem_wait(&staging_area);
    }
}

int num_excess_carbon() {
    return num_c - (num_radicals * NUM_COMBINING_C);
}

void destroy_semaphores() {
    sem_destroy(&mutex);
    sem_destroy(&wait_c);
    sem_destroy(&wait_h);
    sem_destroy(&staging_area);
    sem_destroy(&reaction);
}
