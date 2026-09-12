#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>

/* 
 * floating point string to mpq_t.
 * it does not convert the input to mpf_t,
 * it parses the string itself to not lose precision.
 *
 * gcc f2q.c -lgmp -o f2q
 */

/* floating point str to mpq */
int mpq_set_fpstr(mpq_t rop, const char* str) {
	const char* dot = strchr(str, '.');

	if (dot == NULL) {
		return mpq_set_str(rop, str, 10);
	}

	size_t int_len = dot - str;
	size_t decimal_places = strlen(dot + 1);

	// allocate memory for "numerator/denominator" str
	// add one byte to buf_size to safely hold a leading '0' if needed
	size_t buf_size = strlen(str) + 1 + decimal_places + 3;
	char* frac_str = malloc(buf_size);
	if (!frac_str)
		return -1;

	size_t write_pos = 0;
	if (int_len == 0) {
		// if no integer part write a '0' first
		frac_str[write_pos++] = '0';
	} else {
		strncpy(frac_str, str, int_len);
		write_pos += int_len;
	}
	frac_str[write_pos] = '\0';

	// append fractional digits
	strcpy(&frac_str[write_pos], dot + 1);
	write_pos += decimal_places;

	// append division sign '/'
	frac_str[write_pos++] = '/';
	// append denominator '1'
	frac_str[write_pos++] = '1';

	// append zeros directly from the absolute write pointer
	for (size_t i = 0; i < decimal_places; i++) {
		frac_str[write_pos++] = '0';
	}
	frac_str[write_pos] = '\0';

	int result = mpq_set_str(rop, frac_str, 10);

	mpq_canonicalize(rop);

	free(frac_str);
	return result;
}

int main(int argc, char* argv[]) {
	mpq_t rat;
	mpq_init(rat);
	int ret;

	if (argc == 1) {
		printf("enter floating point\n");
		return 1;
	}
	const char* input_str = argv[1];

	if (mpq_set_fpstr(rat, input_str) == 0) {
		mpf_t float_res;

		gmp_printf("rational: %Qd\n", rat); 

		// print the floating point number back

		mpf_init2(float_res, 512); 
		
		mpf_set_q(float_res, rat);
		
		gmp_printf("float from rational: %Ff\n", float_res);
		
		mpf_clear(float_res);

		ret = 0;
	} else {
		printf("no memory or parse failed\n");
		ret = 1;
	}

	mpq_clear(rat);
	return ret;
}

