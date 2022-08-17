# Ethynyl Radical Problem
An exercise in the use of POSIX semaphores.

## Acknowledgement
This exercise is from an assignment created by David Clark and Mike Zastre from
the University of Victoria for the course CSC 360: Operating Systems. The
project structure and framework have been modified slightly to allow my solution
to be featured separately.

## Problem Description
Hydrogen and carbon atoms are added gradually over time to an initially empty
kosmos. Form ethynyl radicals from two carbon atoms and one hydrogen atom as
soon as there is sufficient free hydrogen and carbon in the kosmos. Threads
represent atoms in a 1:1 relationship. Thus, the challenge is to form 2:1 groups
of carbon and hydrogen threads as new threads continue to arrive.
