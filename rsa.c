#include <stdio.h>
#include <stdbool.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
		
	
	
	uint64_t nbits_max = ((3 * nbits) / 4);	
	uint64_t nbits_min = (nbits/4);
	
	uint64_t size_p, rand_num_1;
	while(true) {
		rand_num_1 = random();
		if (rand_num_1 >= nbits_min && rand_num_1 < nbits_max) {
			size_p = rand_num_1;
			break;
		}
	}
	//= (random() % (nbits_max - nbits_min + 1)) + nbits_min;
	
	uint64_t size_q = (nbits - size_p);
	
	make_prime(p, size_p, iters);
	make_prime(q, size_q, iters);	
	
	mpz_mul(n, p, q);
	
	mpz_t p_minus_one;
	mpz_init(p_minus_one);
	mpz_sub_ui(p_minus_one, p, 1);
	
	mpz_t q_minus_one;
	mpz_init(q_minus_one);
	mpz_sub_ui(q_minus_one, q, 1);
	
	mpz_t totient;
	mpz_init(totient);
	mpz_mul(totient, p_minus_one, q_minus_one);
	
	mpz_t gcd_pq1;
	mpz_init(gcd_pq1);
	gcd(gcd_pq1, p_minus_one, q_minus_one);
	
	mpz_t lcm;
	mpz_init(lcm);
	mpz_fdiv_q(lcm, totient, gcd_pq1);
	
	mpz_t rand_num;
	mpz_init(rand_num);
	mpz_urandomb(rand_num, state, nbits);
	
	mpz_t gcd_totient_rand;
	mpz_init(gcd_totient_rand);
	gcd(gcd_totient_rand, totient, rand_num);
	
	while (mpz_cmp_ui(gcd_totient_rand, 1) != 0) {
		mpz_urandomb(rand_num, state, (nbits+1));
		gcd(gcd_totient_rand, totient, rand_num);
	}
	
	mpz_set(e, rand_num);
	
	mpz_clear(p_minus_one);
	mpz_clear(q_minus_one);
	mpz_clear(totient);
	mpz_clear(gcd_pq1);
	mpz_clear(lcm);
	mpz_clear(rand_num);
	mpz_clear(gcd_totient_rand);
	
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	
	mpz_t p_minus_one;
	mpz_init(p_minus_one);
	mpz_sub_ui(p_minus_one, p, 1);
	
	mpz_t q_minus_one;
	mpz_init(q_minus_one);
	mpz_sub_ui(q_minus_one, q, 1);
	
	mpz_t totient;
	mpz_init(totient);
	mpz_mul(totient, p_minus_one, q_minus_one);
	
	mpz_t gcd_pq1;
	mpz_init(gcd_pq1);
	gcd(gcd_pq1, p_minus_one, q_minus_one);
	
	mpz_t lcm;
	mpz_init(lcm);
	mpz_fdiv_q(lcm, totient, gcd_pq1);

	mod_inverse(d, e, lcm);
	
	
	
	mpz_clear(p_minus_one);
	mpz_clear(q_minus_one);
	mpz_clear(totient);
	mpz_clear(gcd_pq1);
	//mpz_clear(inv);
	mpz_clear(lcm);

}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	
	pow_mod(s, m, d, n);
	
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {

	mpz_t t;
	mpz_init(t);
	
	pow_mod(t, s, e, n);
	
	if (mpz_cmp(t, m) == 0) {
		return true;
	} else {
		return false;
	}
	
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	
	gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);

}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {

	gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
	
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {

	gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s", n, e, s, username);	
}


void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {

	gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);

}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	
	pow_mod(c, m, e, n);
	
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	
	pow_mod(m, c, d, n);	
	
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n , mpz_t e) {
	
	uint64_t log = mpz_sizeinbase(n, 2);
	
	uint64_t k = (log - 1)/8;
	
	uint8_t first_byte = 0xFF;
	
	uint8_t *block = calloc(k, sizeof(uint8_t));
	
	block[0] = first_byte;
	
	int z = 0;
	
	uint8_t ind = 1;
	
	mpz_t m;
	mpz_init(m);
	
	mpz_t c;
	mpz_init(c);
	

	do {
		z = fgetc(infile);
		if (z == -1 /*feof(infile)*/) {
			break;
		}
		
		block[ind] = z;
		
		ind += 1;
		
		if (ind >= k) {
			ind = 1;	
			mpz_import(m, k, 1, sizeof(block[0]), 1, 0, block);
			rsa_encrypt(c, m, e, n); 
			gmp_fprintf(outfile, "%Zx\n", c);
		}
		
	} while(1);
	
	uint8_t *block_end = calloc(ind, sizeof(uint8_t));
	
	//clears remaining array
	for (uint8_t i = 0; i < ind; i += 1) {
		block_end[i] = block[i];	
	}
	
	mpz_import(m, ind, 1, sizeof(block_end[0]), 1, 0, block_end);
	rsa_encrypt(c, m, e, n); 
	gmp_fprintf(outfile, "%Zx\n", c);
	
		
	mpz_clear(m);
	mpz_clear(c);
	free(block);
	free(block_end);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {

	uint64_t log = mpz_sizeinbase(n, 2);
	
	uint64_t k = (log - 1)/8;
	
	uint8_t *block2 = calloc(k, sizeof(uint8_t));
	
	//int z = 0;
	
	mpz_t c;
	mpz_init(c);
	
	mpz_t m;
	mpz_init(m);
	
	
	
	while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
	
		rsa_decrypt(m, c, d, n);
		
		mpz_export(block2, &k, 1, sizeof(uint8_t), 1, 0, m);
		
		for (uint8_t i = 1; i < k; i += 1) {
			gmp_fprintf(outfile, "%c", (char)(block2[i]));
		} 	
	}
	
	mpz_clear(c);
	mpz_clear(m);
	free(block2);
}

