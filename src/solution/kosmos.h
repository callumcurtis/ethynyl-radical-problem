/*
 * kosmos.h
 *
 * By: Callum Curtis
 *
 * Solves the ethynyl-radical problem by forming 2:1 groups of carbon and
 * hydrogen threads.
 *
 * Defines global variables for setting the number of atoms in the kosmos and
 * functions to initialize the kosmos, simulate carbon and hydrogen atoms, and
 * terminate the process after the maximum number of radicals have been created.
 */

#ifndef ETHYNYL_RADICAL_PROBLEM_SOLUTION_KOSMOS_H
#define ETHYNYL_RADICAL_PROBLEM_SOLUTION_KOSMOS_H

extern long num_atoms;
extern int num_c;
extern int num_h;

void kosmos_init(void);

/*
 * Simulates a carbon atom having the provided integer id
 *
 * The thread terminates after it has formed a radical or the maximum number
 * of radicals have been created.
 */
void *c_ready(void *);

/*
 * Simulates a hydrogen atom having the provided integer id
 *
 * The thread terminates after it has formed a radical or the maximum number
 * of radicals have been created.
 */
void *h_ready(void *);

/*
 * Terminates the process after the maximum number of radicals have been
 * created.
 */
void wait_to_terminate();

#endif //ETHYNYL_RADICAL_PROBLEM_SOLUTION_KOSMOS_H
