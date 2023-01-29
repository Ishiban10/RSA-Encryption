#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gmp.h>

#define OPTIONS "i:o:n:vh"

int main(int argc, char *argv[]) {
	
	FILE *input_file = stdin;
	FILE *encrypted_file = stdout;
	char *new_input_file;
	char *new_output_file;
	char *pub_key_file = "rsa.pub";
	char *new_pub_key_file;
	int opt = 0;
	bool new_input = false;
	bool new_output = false;
	bool new_pub_key = false;
	bool verbose = false;
	bool help = false;
	
	
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		
		switch(opt) {
			
			case 'i':
				
				new_input_file = optarg;
				new_input = true;
				break;
			
			case 'o':
			
				new_output_file = optarg;
				new_output = true;
				break;
				
			case 'n':
				
				new_pub_key_file = optarg;
				new_pub_key = true;
				break;
				
			case 'v':
			
				verbose = true;
				break;
				
			case 'h':
				
				help = true;
				break;
						
		}	
				
	}
	
	if (help) {
		printf("SYNOPSIS\n");
		printf("	Program to encrypt files\n");
		printf("USAGE\n");
		printf("	./encrypt [-i input file] [-o output file] [-n public key file] [-v] [-h]\n");
		printf("OPTIONS\n");
		printf("	-i 	Specify input file to encrypt\n");
		printf("	-o	Specify output file to encrypt\n");
		printf("	-n	Specify the public key file\n");
		printf("	-v 	Enables verbose output\n");
		printf("	-h	Displays program synopsis and usage\n");
	}
	
	FILE *pub;
	if (new_pub_key) {
		pub = fopen(new_pub_key_file, "r");
	} else {
		pub = fopen(pub_key_file, "r");
	}
	
	if (pub == NULL) {
		printf("Sorry, couldn't find this file\n");
		return EXIT_FAILURE;
	}
	
	mpz_t n;
	mpz_t e;
	mpz_t s;
	mpz_init(n);
	mpz_init(e);
	mpz_init(s);
	
	char *username = calloc(30, sizeof(char));
	
	rsa_read_pub(n, e, s, username, pub);
	
	
	size_t sizen = mpz_sizeinbase(n, 10);
	size_t sizee = mpz_sizeinbase(e, 10);
	size_t sizes = mpz_sizeinbase(s, 10);
	
	if (verbose) {
		gmp_fprintf(stderr, "%s\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n%Zd (bits:%zu)\n", username, s, sizes, n, sizen, e, sizee);
	}
	
	
	if (new_input) {
		input_file = fopen(new_input_file, "r");
	}
	
	if (new_output) {
		encrypted_file = fopen(new_output_file, "a");
	}
	
	mpz_t m;
	mpz_init(m);
	
	mpz_set_str(m, username, 62);
	//gmp_printf("%Zd\n", m);
	
	bool verified = rsa_verify(m, s, e, n);
	if (verified == false) {
		printf("Unable to verify\n");	
		return EXIT_FAILURE;
	}
	
	rsa_encrypt_file(input_file, encrypted_file, n, e);
	
	fclose(input_file);
	fclose(encrypted_file);
	fclose(pub);
	mpz_clears(n, e, s, m, NULL);
	free(username);

}
