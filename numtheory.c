#include "numtheory.h"
#include <stdlib.h>
#include <stdio.h>
#include "randstate.h"
#include <gmp.h>
#include <stdbool.h>


void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t temp_a;
	mpz_init (temp_a);
	
	mpz_t temp_b;
	mpz_init (temp_b);
	
	mpz_set(temp_a, a);
	mpz_set(temp_b, b);
	
	mpz_t temp_temp_b;
	mpz_init (temp_temp_b);
	
	
	while(mpz_cmp_ui(temp_b, 0) > 0) {
		
		mpz_set(temp_temp_b, temp_b);
		
		mpz_mod(temp_b, temp_a, temp_b);
		
		mpz_set(temp_a, temp_temp_b);
	}
	
	mpz_set(d, temp_a);
	mpz_clear(temp_a);
	mpz_clear(temp_b);
	mpz_clear(temp_temp_b);
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
	mpz_t v;
	mpz_init(v);
	mpz_set_ui(v, 1);
	
	mpz_t p; 
	mpz_init(p);
	mpz_set(p, base);

	mpz_t temp_exponent;
	mpz_init(temp_exponent);
	mpz_set(temp_exponent, exponent);
	
	mpz_t temp_mul_v;
	mpz_init (temp_mul_v);		
	
	//mpz_t temp_mod_v;
	//mpz_init(temp_mod_v);
	
	mpz_t temp_mul_p;
	mpz_init (temp_mul_p);
	
	//mpz_t temp_mod_p;
	//mpz_init (temp_mod_p);
	
	while (mpz_cmp_ui(temp_exponent, 0) > 0) {
		if (mpz_odd_p(temp_exponent) == 1) {	
			mpz_mul(temp_mul_v, v, p);
			mpz_mod(v, temp_mul_v, modulus);
			//mpz_set(v, temp_mod_v);
		}
		mpz_mul(temp_mul_p, p, p);
		mpz_mod(p, temp_mul_p, modulus);
		//mpz_set(p, temp_mod_p);
		
		mpz_fdiv_q_ui(temp_exponent, temp_exponent, 2); 
		//mpz_set(temp_exponent, temp_exp_div);	
	}
	mpz_set(out, v);
	mpz_clear(v);
	mpz_clear(p);
	mpz_clear(temp_exponent);
	//mpz_clear(temp_exp_div);
	mpz_clear(temp_mul_v);
	//mpz_clear(temp_mod_v);
	mpz_clear(temp_mul_p);
	//mpz_clear(temp_mod_p);
	
}

void find_totient(mpz_t s, mpz_t r, mpz_t n) {
	mpz_t temp_n;
	mpz_init(temp_n);
	mpz_set(temp_n, n);
	mpz_sub_ui(temp_n, temp_n, 1);
	
	mpz_t temp_mod;
	mpz_init(temp_mod);
	mpz_mod_ui(temp_mod, temp_n, 2);
	
	while (mpz_cmp_ui(temp_mod, 0) == 0) {
		mpz_add_ui(s, s, 1);
		mpz_fdiv_q_ui(temp_n, temp_n, 2);
		mpz_mod_ui(temp_mod, temp_n, 2);
	}
	mpz_init_set(r, temp_n);
	mpz_clear(temp_n);
	mpz_clear(temp_mod);
	
}

bool is_prime(mpz_t n, uint64_t iters) {
	
	mpz_t k, z, z2, n_min_one, a, r, s_min_one, s, d;
	mpz_inits(k, z, z2, n_min_one, a, r, s_min_one, s, d, NULL);
	find_totient(s, r, n);
	
	mpz_sub_ui(n_min_one, n, 1);
	mpz_sub_ui(s_min_one, s, 1);
	
	mpz_init_set_ui(d, 2);
	
	for (uint64_t i = 1; i < iters; i += 1) {
	
		mpz_urandomm(a, state, n_min_one);
		
		if (mpz_cmp_ui(a, 2) >= 0) {
			pow_mod(z2, a, r, n);
			mpz_set(z, z2);
			
			if ((mpz_cmp_ui(z, 1) != 0) && (mpz_cmp(z, n_min_one) != 0)) {
				mpz_init_set_ui(k, 1);
				
				while ((mpz_cmp(k, s_min_one) <= 0) && (mpz_cmp(z, n_min_one) != 0)) {
					pow_mod(z, z2, d, n);
					
					if (mpz_cmp_ui(z, 1) == 0) {
						mpz_clears(k, z, n_min_one, a, r, s_min_one, s, d, NULL);
						return false;
					}
					
					mpz_add_ui(k, k, 1);
				}
				
				if (mpz_cmp(z, n_min_one) != 0) {
					mpz_clears(k, z, n_min_one, a, r, s_min_one, s, d, NULL);
					return false;
				}
			}
		}	
		
	}
	mpz_clears(k, z, n_min_one, a, r, s_min_one, s, d, NULL);
	return true;
	
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	/*
	mpz_t m;
	mpz_init(m);
	mpz_set_ui(m, 2);
	
	
	mpz_t base;
	mpz_init(base);
	mpz_set_ui(base, 2);
	
	for (uint64_t i = 1; i <= bits; i += 1) {
		mpz_mul(m, m, base);
	}
	//gmp_printf("%Zd\n", m);
	
	
	//mpz_pow_ui(m, m, bits);
	mpz_add_ui(m, m, 2);
	*/
	
	mpz_urandomb(p, state, bits);
	
	bool is_p = is_prime(p, iters);
	
	while (is_p == 0) {
		mpz_urandomb(p, state, bits);
		//mpz_xor(p, p, m);
		
		is_p = is_prime(p, iters);
		
		if (is_p == 1) {
			break;
		}
	}	
	//mpz_clear(m);
	//mpz_clear(base);

}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
	mpz_t r;
	mpz_init(r);
	mpz_set(r, n);
	
	mpz_t r_prime;
	mpz_init(r_prime);
	mpz_set(r_prime, a);
	
	mpz_t t;
	mpz_init(t);
	mpz_set_ui(t, 0);
	
	mpz_t t_prime;
	mpz_init(t_prime);
	mpz_set_ui(t_prime, 1);
	
	mpz_t q;
	mpz_init(q);
	
	mpz_t temp_r;
	mpz_init(temp_r);
	
	mpz_t temp_t;
	mpz_init(temp_t);
	
	mpz_t prod_r_prime;
	mpz_init(prod_r_prime);
	
	mpz_t prod_t_prime;
	mpz_init(prod_t_prime);
	
	mpz_t sub_r_prime;
	mpz_init(sub_r_prime);
	
	mpz_t sub_t_prime;
	mpz_init(sub_t_prime);
	
	
	while (mpz_cmp_ui(r_prime, 0) != 0) {
		
		mpz_fdiv_q(q, r, r_prime);
		mpz_set(temp_r, r);
		mpz_mul(prod_r_prime, q, r_prime);
		mpz_sub(sub_r_prime, temp_r, prod_r_prime);
		
		mpz_set(r, r_prime);
		mpz_set(r_prime, sub_r_prime);
		
		mpz_set(temp_t, t);
		mpz_mul(prod_t_prime, q, t_prime);
		mpz_sub(sub_t_prime, temp_t, prod_t_prime);
		
		mpz_set(t, t_prime);
		mpz_set(t_prime, sub_t_prime);
		
	}
	
	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_set_ui(i, 0);
	}
	
	if (mpz_cmp_ui(t, 0) < 0){
		mpz_add(i, t, n);
	}
	
	mpz_clear(r);
	mpz_clear(r_prime);
	mpz_clear(t);
	mpz_clear(t_prime);
	mpz_clear(q);
	mpz_clear(temp_r);
	mpz_clear(temp_t);
	mpz_clear(prod_r_prime);
	mpz_clear(sub_r_prime);
	mpz_clear(prod_t_prime);
	mpz_clear(sub_t_prime);	
}
