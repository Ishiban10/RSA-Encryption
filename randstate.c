#include "randstate.h"
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>

gmp_randstate_t state;

void randstate_init(uint64_t seed) {

	//gmp_randstate_t state;		
		
	gmp_randinit_mt(state);
	
	gmp_randseed_ui(state, seed);
	
}

void randstate_clear() {

	//gmp_randstate_t state;
	
	gmp_randclear(state);
}
