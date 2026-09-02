#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* vim: set noexpandtab tabstop=8 shiftwidth=8 softtabstop=8 textwidth=80 : */

/*
   Ramanujan's pi formula

   $\frac{1}{\pi} = \frac{2\sqrt{2}}{9801} \sum_{k=0}^{\infty} \frac{(4k)!(1103 + 26390k)}{(k!)^4 396^{4k}}$

   gcc mp_pi_ramanujan.c -o mp_pi_ramanujan -lgmp
*/

void compute_pi_mpq(unsigned long precision_bits, unsigned long iterations) {
	// 1. Initialize exact rational variables for the series summation
	mpq_t sum, term, num, den;
	mpq_inits(sum, term, num, den, NULL);

	// Initialize arbitrary precision integers for calculating components
	mpz_t fact_4k, fact_k, fact_k_4, power_396, linear_term;
	mpz_inits(fact_4k, fact_k, fact_k_4, power_396, linear_term, NULL);

	// Temp variables for loop arithmetic
	mpz_t t_num, t_den;
	mpz_inits(t_num, t_den, NULL);

	mpq_set_ui(sum, 0, 1); // sum = 0/1

	// 2. Compute the series exactly as an mpq rational fraction
	for (unsigned long k = 0; k < iterations; k++) {
		// Compute factorials: (4k)! and (k!)^4
		mpz_fac_ui(fact_4k, 4 * k);
		mpz_fac_ui(fact_k, k);
		mpz_pow_ui(fact_k_4, fact_k, 4);

		// Compute exponential term: 396^(4k)
		mpz_ui_pow_ui(power_396, 396, 4 * k);

		// Compute linear term: 1103 + 26390 * k
		mpz_set_ui(linear_term, 26390);
		mpz_mul_ui(linear_term, linear_term, k);
		mpz_add_ui(linear_term, linear_term, 1103);

		// Calculate exact term numerator: (4k)! * (1103 + 26390k)
		mpz_mul(t_num, fact_4k, linear_term);

		// Calculate exact term denominator: (k!)^4 * 396^(4k)
		mpz_mul(t_den, fact_k_4, power_396);

		// Set the rational term component: num / den
		mpq_set_num(term, t_num);
		mpq_set_den(term, t_den);

		// GMP fractions must be normalized to ensure accurate rational math
		mpq_canonicalize(term);

		// Accumulate exact rational term into total sum
		mpq_add(sum, sum, term);
	}

	// 3. Convert the exact fraction to float format for final irrational math
	mpf_set_default_prec(precision_bits);
	mpf_t f_sum, constant, pi, sqrt_2;
	mpf_inits(f_sum, constant, pi, sqrt_2, NULL);

	// Convert sum from mpq_t to mpf_t
	mpf_set_q(f_sum, sum);

	// Compute the constant multiplier: 2 * sqrt(2) / 9801
	mpf_set_ui(sqrt_2, 2);
	mpf_sqrt(sqrt_2, sqrt_2); // sqrt(2)

	mpf_set_ui(constant, 2);
	mpf_mul(constant, constant, sqrt_2); // 2 * sqrt(2)
	mpf_div_ui(constant, constant, 9801); // (2 * sqrt(2)) / 9801

	// Final Pi calculation: pi = 1 / (constant * f_sum)
	mpf_mul(pi, constant, f_sum);
	mpf_ui_div(pi, 1, pi);

	// Print the final floating-point approximation of Pi
	gmp_printf("Calculated Pi:\n%.*Ff\n", (int)(precision_bits * 0.30103), pi);

	// Free memory allocations
	mpq_clears(sum, term, num, den, NULL);
	mpz_clears(fact_4k, fact_k, fact_k_4, power_396, linear_term, t_num, t_den, NULL);
	mpf_clears(f_sum, constant, pi, sqrt_2, NULL);
}

int main() {
	// 1000 bits of precision translates to ~301 decimal digits
	unsigned long precision_bits = 1000; 

	// 40 iterations will yield ~320 correct digits
	unsigned long iterations = 40; 

	printf("Computing Pi using exact mpq fractions (%lu iterations)...\n", iterations);
	compute_pi_mpq(precision_bits, iterations);

	return 0;
}

