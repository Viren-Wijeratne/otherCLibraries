//This program uses the maths library called libgmp-dev
//Compilation: gcc -Wall -Wextra -std=c11 gmp_demo.c -o gmp_demo -lgmp

#include <stdio.h>
#include <gmp.h>

int main(){

    mpz_t result;
    mpz_init(result);
    mpz_fac_ui(result, 100);
    gmp_printf("100! = %Zd\n", result);
    mpz_clear(result);

    return 0;
}