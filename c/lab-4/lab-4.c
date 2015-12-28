#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


// Compiling:
// cc  -std=c99 -D_GNU_SOURCE -pthread  main.c -o main -lm

pthread_barrier_t barrier;
pthread_mutex_t consoleMutex;

struct CalculateExpArgs
{
    double value;
    unsigned int maxNumberOfOperations;
    double result;
};

void CalculateExp(struct CalculateExpArgs* args)
{
    pthread_mutex_lock(&consoleMutex);
    printf("CalculateExp: argument is %f\n", args->value);
    pthread_mutex_unlock(&consoleMutex);

    double exp = 1;
    double factorial = 1;
    double power;

    for (unsigned int i = 0; i < args->maxNumberOfOperations; i++)
    {
        power = i + 1;
        factorial *= power;
        exp += pow(args->value, power) / factorial;
    }

    args->result = exp;

    pthread_mutex_lock(&consoleMutex);
    printf("CalculateExp: done\n");
    pthread_mutex_unlock(&consoleMutex);

    pthread_barrier_wait(&barrier);
}

struct CalculatePiArgs
{
    unsigned int maxNumberOfOperations;
    double result;
};

void CalculatePi(struct CalculatePiArgs* args)
{
    pthread_mutex_lock(&consoleMutex);
    printf("CalculatePi\n");
    pthread_mutex_unlock(&consoleMutex);

    double pi = 1.0;
    int numerator = 2;
    int denominator = 1;

    int numerator_counter = 0;
    int denominator_counter = 1;

    for (unsigned int i = 0; i < args->maxNumberOfOperations; i++)
    {
        if (numerator_counter == 2)
        {
            numerator_counter = 0;
            numerator += 2;
        }
        if (denominator_counter == 2)
        {
            denominator_counter = 0;
            denominator += 2;
        }

        numerator_counter++;
        denominator_counter++;

        pi *= ((double) numerator / (double) denominator);
    }

    args->result = pi;

    pthread_mutex_lock(&consoleMutex);
    printf("CalculatePi: done\n");
    pthread_mutex_unlock(&consoleMutex);

    pthread_barrier_wait(&barrier);
}

int main()
{
    double x = 1.0;

    pthread_t thread_exp;
    pthread_t thread_pi;

    struct CalculateExpArgs expArgs;
    expArgs.value = (-pow(x,2.0)) / 2;
    expArgs.maxNumberOfOperations = 100000;

    struct CalculatePiArgs piArgs;
    piArgs.maxNumberOfOperations =  100000;

    pthread_barrier_init(&barrier, NULL, 2);
    pthread_mutex_init(&consoleMutex, NULL);

    if ( pthread_create(&thread_exp, NULL, &CalculateExp, &expArgs) )
    {
        pthread_mutex_lock(&consoleMutex);
        printf("Error launching exp thread");
        pthread_mutex_unlock(&consoleMutex);
        return 2;
    }
    if ( pthread_create(&thread_pi, NULL, &CalculatePi, &piArgs) )
    {
        pthread_mutex_lock(&consoleMutex);
        printf("Error launching pi thread");
        pthread_mutex_unlock(&consoleMutex);
        return 2;
    }

    pthread_join(thread_exp, NULL);
    pthread_join(thread_pi, NULL);
    pthread_barrier_destroy(&barrier);

    double result = expArgs.result / sqrt(2 * piArgs.result);

    pthread_mutex_lock(&consoleMutex);
    printf("Result:%f\n", result);
    pthread_mutex_unlock(&consoleMutex);

    return 0;
}
