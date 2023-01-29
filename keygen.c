
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>


#define OPTIONS "b:i:n:d:s:vh"


int main (int argc, char* argv[]) {
	
	// default values
	uint64_t iters = 50;
	char *pub_file_new;
	char *pub_file = "rsa.pub";
	bool new_pub_file = false;
	char *priv_file_new;
	char *priv_file = "rsa.priv";
	bool new_priv_file = false;
	time_t seed;
	seed = time(NULL);
	uint64_t min_bits = 1024;
	bool verbose = false;
	bool help = false;

	
	int opt = 0;
	
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
	
		switch (opt) {
		
			case 'b':
				min_bits = strtoul(optarg, NULL, 10);
				break;
			
			case 'i':
				iters = strtoul(optarg, NULL, 10);
				break;
			
			case 'n':
				pub_file_new = optarg;
				new_pub_file = true;
				break;
				
			case 'd':
				priv_file_new = optarg;
				new_priv_file = true;
				break;
				
			case 's':
				seed = strtoul(optarg, NULL, 10);
				break;
				
			case 'v':
				verbose = true;
				break;
				
			case 'h':
				help = true;
				break;

		}
	}	
	
	randstate_init(seed);
	
	// do actual keygen logic here with rsa functions
	
	// mpz variables that I have to use for the key gens
	mpz_t p;
	mpz_init(p);
	
	mpz_t q;
	mpz_init(q);
	
	mpz_t n;
	mpz_init(n);
	
	mpz_t e;
	mpz_init(e);
	
	mpz_t d;
	mpz_init(d);
	
	mpz_t s;
	mpz_init(s);
	
	mpz_t m;
	mpz_init(m);
	
	rsa_make_pub(p, q, n, e, min_bits, iters);
	rsa_make_priv(d, e, p, q);
	//gmp_printf("%Zd\n", d);

	char *username = getenv("USER");
	mpz_set_str(m, username, 62);
	
	rsa_sign(s, m, d, n);

	
	FILE *pub_out_file;
	if (new_pub_file) {
		pub_out_file = fopen(pub_file_new, "w");
	} else {
		pub_out_file = fopen(pub_file, "w");
	}
	
	if (pub_out_file == NULL) {
		printf("Sorry, could not open file\n");
		return EXIT_FAILURE;
	}  
	
	rsa_write_pub(n, e, s, username, pub_out_file);
	
	fclose(pub_out_file);
	
	FILE *priv_out_file;
	if (new_priv_file) {
		priv_out_file = fopen(priv_file_new, "w");
	} else {
		priv_out_file = fopen(priv_file, "w");
	}
	
	if (priv_out_file == NULL) {
		printf("Sorry, could not open file\n");
		return EXIT_FAILURE;
	}
	
	//on fchmod fileno
	int fildes = fileno(priv_out_file);
	fchmod(fildes, S_IRUSR | S_IWUSR);

	rsa_write_priv(n, d, priv_out_file);
	
	fclose(priv_out_file);
	
	size_t bits_s = mpz_sizeinbase(s, 10);
	size_t bits_p = mpz_sizeinbase(p, 10);
	size_t bits_q = mpz_sizeinbase(q, 10);
	size_t bits_n = mpz_sizeinbase(n, 10);
	size_t bits_e = mpz_sizeinbase(e, 10);
	size_t bits_d = mpz_sizeinbase(d, 10);
	
	if (verbose) {
		
		gmp_fprintf(stderr, "%s\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n", username, s, bits_s, p, bits_p, q, bits_q, n, bits_n, e, bits_e, d, bits_d);	
		
	}
	
	if (help) {
		//print help manual
		printf("SYNOPSIS\n");
		printf("	Program to generate public and private RSA keys for encryption and decryption\n");
		printf("USAGE\n");
		printf("	./keygen [-b bits] [-i Miller-Rabin iterations] [-n public key file] [-d private key file] [-s seed] [-v] [-h]\n");
		printf("OPTIONS\n");
		printf("	-b 	Specify minimum number of bits needed for public modulus n (default: 1024)\n");
		printf("	-i	Specify number of Miller-Rabin iterations for testing primes (default: 50)\n");
		printf("	-n	Specify the public key file\n");
		printf("	-d	Specify the private key file\n");
		printf("	-s	Speicifies random seed\n");
		printf("	-v 	Enables verbose output\n");
		printf("	-h	Displays program synopsis and usage\n");
	}
	
	randstate_clear();
	mpz_clear(p);
	mpz_clear(q);
	mpz_clear(s);
	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(d);
	mpz_clear(m);
	
	
}
