// Weedguet Mildort
// Optimal Noise Reduction
// Description:

#include "main.h"


// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ANIMALS 20
#define EMPTY 0
#define UNPLACED 1

// Typedef declaration
typedef struct edge_t edge_t;
typedef struct edge_set_t edge_set_t;
typedef struct constraints_t constraints_t;
typedef struct solver_t solver_t;

// Constraint graph edges
struct edge_t {
    // the peer animal with the constraint
    int peer;
    // and the required distance
    int dist;
};

struct edge_set_t {
    // number in the set
    int n;
    // and the edges themselves
    edge_t edges[MAX_ANIMALS];
};

struct constraints_t {
    // forced distance between two animals or -1
    // edge set per animal
    edge_set_t graph[MAX_ANIMALS];
    // # animals
    int animals;
    // is animal in a constraint?
    bool forced[MAX_ANIMALS];
};

struct solver_t {
    // the constraint graph
    constraints_t *cons;
    // current slot trying to fill
    int slot;
    // what animals are in each cage (or EMPTY)
    int cages[MAX_ANIMALS];
    // what cage each animal is in (or UNPLACED)
    int animcage[MAX_ANIMALS];
    // stack of animals placed so far
    int placed_stk[MAX_ANIMALS];
    // and how many
    int nplaced;
};

// read in the constraint graph
//
void read_constraints(constraints_t *cons)
{
    int             i, j;                   // loop indices
    int             ncons;                  // # constraints
    int             an0, an1;               // animal indices
    int             dist;                   // distance between them

    scanf("%d %d\n", &cons->animals, &ncons);

    // set defaults: everything is unconstrained
    //
    for (int i = 0; i < MAX_ANIMALS; i++) {
        cons -> forced[i] = false;
        cons -> graph[i].n = 0;
    }

    // read in the constraint pairs
    //
    for (i = 0; i < ncons; i++) {
        scanf("%d %d %d", &an0, &an1, &dist);

        an0--;  // animals are zero-based
        an1--;

        dist++; // (n+1)-(n)=1 but this is a constraint of zero

        cons -> graph[an0].edges[cons -> graph[an0].n].peer = an1;
        cons -> graph[an0].edges[cons -> graph[an0].n].dist = dist;
        cons -> graph[an0].n++;

        cons -> graph[an1].edges[cons -> graph[an1].n].peer = an0;
        cons -> graph[an1].edges[cons -> graph[an1].n].dist = dist;
        cons -> graph[an1].n++;

        cons -> forced[an0] = true;
        cons -> forced[an1] = true;
    }
}

// clear out the solver state
//
void init_solver(constraints_t *cons, solver_t *solver)
{
    int             i;                      // loop index

    solver -> cons = cons;
    solver -> slot = 0;

    for (i = 0; i < cons -> animals; i++) {
        solver -> animcage[i] = UNPLACED;
        solver -> cages[i] = EMPTY;
    }

    solver -> nplaced = 0;
}

// put an animal in a cage
//
void cage_animal(struct solver_t *solver, int cage, int anim)
{
    solver -> placed_stk[solver -> nplaced++] = anim;
    solver -> cages[cage] = anim;
    solver -> animcage[anim] = cage;
}

// free the most recently caged animal
//
void free_animal(struct solver_t *solver)
{
    int             anim;                   // animal being free'd
    int             cage;                   //   and the cage it's currently in

    anim = solver -> placed_stk[--solver -> nplaced];
    cage = solver -> animcage[anim];

    solver -> animcage[anim] = UNPLACED;
    solver -> cages[cage] = EMPTY;
}

// remove animals from cages based on the stack until
// the stack pointer is stkptr
//
void pop_goes_the_weasels(struct solver_t *solver, int stkptr)
{
    while (solver -> nplaced > stkptr) {
        free_animal(solver);
    }
}

// add constraints for the animal at the given slot
//
bool add_animal_constraints(struct solver_t *solver, int stkptr)
{
    int             i;                      // loop index
    int             anim;                   // animal under consideration
    edge_set_t      *edges;                 //   and its edge set
    int             peer;                   // animal which shares constraint
    int             peercage;               //   the cage it's in
    int             dist;                   //   and the required distance

    anim = solver -> cages[solver -> slot];
    edges = &solver -> cons -> graph[anim];

    for (i = 0; i < edges -> n; i++) {
        peer = edges -> edges[i].peer;
        dist = edges -> edges[i].dist;
        peercage = solver -> animcage[peer];

        // if the peer is already placed, verify it satisfies constraints
        //
        if (peercage != UNPLACED) {
            if (abs(solver -> slot - peercage) != dist) {
                pop_goes_the_weasels(solver, stkptr);
                return false;
            }
            continue;
        }

        // otherwise, see if we can place the peer
        // we place left to right, so we only need to consider higher cages
        //
        peercage = solver -> slot + dist;
        if (
                peercage >= solver -> cons -> animals ||
                (solver -> cages[peercage] != EMPTY && solver -> cages[peercage] != peer)) {
            pop_goes_the_weasels(solver, stkptr);
            return false;
        }

        cage_animal(solver, peercage, peer);
    }

    return true;
}

// place an animal at the current slot in the solver, ensuring any
// direct constraints are satisfied.
//
bool place_animal_with_constraints(struct solver_t *solver, int anim)
{
    // stack pointer for cleanup
    int             stkptr = solver -> nplaced;

    cage_animal(solver, solver -> slot, anim);

    // note that this will clean up the animal we just placed, if
    // constraints fail, because we put it on the stack
    //
    return add_animal_constraints(solver, stkptr);
}

// recursively try to place the animals in the cages. returns true if
// a solution was found, else false
//
bool solve(struct solver_t *solver)
{
    constraints_t   *cons = solver -> cons; // the constraint graph
    int             i;                      // loop index
    int             anim;                   // the animal we're trying to place
    int             dist;                   // constraint for the animal
    bool            tried_unforced = false; // at least one unforced animal has been tried and failed
    bool            cons_fail;              // true on constraint failure
    int             stkptr;                 // save depth of stack when placing multiple animals

    // base case: we found a solution
    //
    if (solver -> nplaced == solver -> cons -> animals) {
        return true;
    }

    // if there's already a constraint-forced animal in this slot,
    // ensure any of its constraints that aren't already there are
    // placed.
    //
    if (solver -> cages[solver -> slot] != EMPTY) {
        anim = solver -> cages[solver -> slot];

        stkptr = solver -> nplaced;

        if (!add_animal_constraints(solver, solver -> nplaced)) {
            return false;
        }

        solver -> slot++;
        if (solve(solver)) {
            return true;
        }
        solver -> slot--;

        pop_goes_the_weasels(solver, stkptr);
        return false;
    }

    for (anim = 0; anim < cons -> animals; anim++) {
        if (solver -> animcage[anim] != UNPLACED) {
            continue;
        }

        // if we've already tried an unconstrained animal, no point
        // trying another one.
        //
        if (!cons -> forced[anim]) {
            if (tried_unforced) {
                continue;
            } else {
                tried_unforced = true;
            }

            // just do the unconstrained case here
            //
            cage_animal(solver, solver -> slot, anim);
            solver -> slot++;

            if (solve(solver)) {
                return true;
            }

            free_animal(solver);

            solver -> slot--;

            continue;
        }

        // we need to add an animal that has constraints
        //
        stkptr = solver -> nplaced;
        if (!place_animal_with_constraints(solver, anim)) {
            continue;
        }

        solver -> slot++;
        if (solve(solver)) {
            return true;
        }
        solver -> slot--;

        pop_goes_the_weasels(solver, stkptr);
    }

    return false;
}

int main()
{
    int             i;                      // loop index
    constraints_t   cons;                   // the constraints
    solver_t        solver;                 // the solver state

    read_constraints(&cons);
    init_solver(&cons, &solver);

    if (solve(&solver)) {
        for (i = 0; i < cons.animals; i++) {
            // NB solution expects 1-based animals
            //
            printf("%d ", solver.cages[i] + 1);
        }
        printf("\n");
    } else {
        printf("No Solution\n");
    }
}
