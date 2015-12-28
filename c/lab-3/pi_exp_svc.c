#include <stdio.h>
#include <time.h>
#include <math.h>
#include <rpc/rpc.h>

// Generated with rpcgen:
#include "rcalculations.h"

static double pi;
static double e;

static int numberOfOperations = 100000;

double calculate_pi(void)
{
	double _pi = 1;

	int numerator = 2, denominator = 1;
    int numeratorCounter = 0, denominatorCounter = 1;

    for (unsigned int i = 0; i < numberOfOperations; i++)
    {
        if (numeratorCounter == 2)
        {
            numeratorCounter = 0;
            numerator += 2;
        }

        if (denominatorCounter == 2)
        {
            denominatorCounter = 0;
            denominator += 2;
        }

        numeratorCounter++;
        denominatorCounter++;

        _pi *= ((double) numerator / (double) denominator);
    }

    _pi *= 2;

    return _pi;
}

double calculate_exp(double x)
{
	double factorial = 1, power;
    double _e = 1;

    for (unsigned int i = 0; i < numberOfOperations; i++)
    {
        power = i + 1;
        factorial *= power;
        _e += pow(x, power) / factorial;
    }

    return _e;
}

double* calculate_pi_1_svc(void* p, struct svc_req *cl)
{
    pi = calculate_pi();
    return &pi;
}

double* calculate_exp_1_svc(double* x, struct svc_req *cl)
{
    e = calculate_exp(*x);
    return &e;
}
