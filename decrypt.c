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
	FILE *decrypted_file = stdout; 
	char *new_input_file;
	char *new_output_file;
	char *priv_key_file = "rsa.priv";
	char *new_priv_key_file;
	int opt = 0;
	bool new_input = false;
	bool new_output = false;
	bool new_priv_key = false;
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
				
				new_priv_key_file = optarg;
				new_priv_key = true;
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
		printf("	Program to decrypt files\n");
		printf("USAGE\n");
		printf("	./decrypt [-i input file] [-o output file] [-n private key file] [-v] [-h]\n");
		printf("OPTIONS\n");
		printf("	-i 	Specify input file to decrypt\n");
		printf("	-o	Specify output file to decrypt\n");
		printf("	-n	Specify the private key file\n");
		printf("	-v 	Enables verbose output\n");
		printf("	-h	Displays program synopsis and usage\n");
	}
	
	FILE *priv;
	if (new_priv_key) {
		priv = fopen(new_priv_key_file, "r");
	} else {
		priv = fopen(priv_key_file, "r");
	}
	
	if (priv == NULL) {
		printf("Sorry, couldn't find this file");
		return EXIT_FAILURE;
	}
	
	mpz_t n, d;
	mpz_inits(n, d, NULL);
	
	rsa_read_priv(n, d, priv);

	
	fclose(priv);
	
	size_t sizen = mpz_sizeinbase(n, 10);
	size_t sized = mpz_sizeinbase(d, 10);
	
	if (verbose) {
		gmp_fprintf(stderr, "%Zd (bits:%zu)\n%Zd (bits:%zu)\n", n, sizen, d, sized);
	}
	
	
	
	if (new_input) {
		input_file = fopen(new_input_file, "r");
	}
	
	
	if (new_output) {
		decrypted_file = fopen(new_output_file, "a");
	} 

	
	rsa_decrypt_file(input_file, decrypted_file, n, d);
	
	fclose(input_file);
	fclose(decrypted_file);
	mpz_clears(n, d, NULL);

}
