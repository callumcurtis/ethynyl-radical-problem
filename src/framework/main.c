/*
 * main.c
 *
 * By David Clark and Mike Zastre for UVic CSC 360, Summer 2022
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 136 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solution/kosmos.h"
#include "logging.h"


/* Random # below threshold indicates H; otherwise C. */
#define ATOM_THRESHOLD 0.55
#define DEFAULT_NUM_ATOMS 40


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
    int *pi = (int *)malloc(sizeof(int));
    assert( pi != NULL);
    *pi = i;
    return pi;
}


int main(int argc, char *argv[])
{
    long seed;
    num_atoms = DEFAULT_NUM_ATOMS;
    pthread_t **atom;
    int i;
    int status;

    if ( argc < 2 ) {
        fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
        exit(1);
    }

    if ( argc >= 2) {
        seed = atoi(argv[1]);
    }

    if (argc == 3) {
        num_atoms = atoi(argv[2]);
        if (num_atoms < 0) {
            fprintf(stderr, "%ld is not a valid number of atoms\n",
                    num_atoms);
            exit(1);
        }
    }

    kosmos_log_init();
    kosmos_init();

    srand(seed);
    atom = (pthread_t **)malloc(num_atoms * sizeof(pthread_t *));
    assert (atom != NULL);
    for (i = 0; i < num_atoms; i++) {
        atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
        if ( (double)rand()/(double)RAND_MAX < ATOM_THRESHOLD ) {
            num_h++;
            status = pthread_create (
                    atom[i], NULL, h_ready,
                    (void *)dupInt(num_h)
            );
        } else {
            num_c++;
            status = pthread_create (
                    atom[i], NULL, c_ready,
                    (void *)dupInt(num_c)
            );
        }
        if (status != 0) {
            fprintf(stderr, "Error creating atom thread\n");
            exit(1);
        }
    }

#ifdef VERBOSE
    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * num_h and num_c/2.
     */

    int max_radicals = (num_h < num_c / 2 ? num_h : (int)(num_c / 2));
    printf("Maximum # of radicals expected: %d\n", max_radicals);
#endif

    wait_to_terminate();
}
