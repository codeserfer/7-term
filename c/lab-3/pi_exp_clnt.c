#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <rpc/rpc.h>

// Generated with rpcgen:
#include "rcalculations.h"

#define RMACHINE "localhost"

CLIENT* handle;

double calculate_pi(void)
{
	return *calculate_pi_1(NULL, handle);
}

double calculate_exp(double x)
{
	return *calculate_exp_1(&x, handle);
}

int main(int argc, char **argv)
{
    handle = clnt_create(RMACHINE, RCALCULATIONS_PROG, RCALCULATIONS_VERS, "tcp");
    
    if (handle == NULL)
    {
        perror("");
        exit(1);
    }
    
    double x = 1;
    double pi = calculate_pi();
    double e = calculate_exp((-pow(x, 2.0)) / 2.0);

    char result[64];

    sprintf (result, "pi = %f%c", pi, '\0');
    puts(result);

    sprintf (result, "e = %f%c", e, '\0');
    puts(result);

    sprintf (result, "f(x) = %f%c", e / sqrt(2.0 * pi), '\0');
    puts(result);
}
