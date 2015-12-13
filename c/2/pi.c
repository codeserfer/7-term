#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int counting_pi = 1;

int fd[2]; // pipe

void end_counting_pi(int sig)
{
    printf ("CHILD:  End counting!\n");
    counting_pi = 0;
}

void start_counting_pi(int sig)
{
    printf ("CHILD:  Start counting pi!\n");
    signal(SIGUSR2, end_counting_pi);



    // counting
    double pi = 1.0;
    int numerator = 2;
    int denominator = 1;

    int numerator_counter = 0;
    int denominator_counter = 1;

    unsigned int i;
    while (counting_pi)
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

        pi = pi* ((double)numerator/(double)denominator);
    }

    pi = 2*pi;

    printf ("pi is %f\n", pi);

    char text [40];
    sprintf(text, "@%d pi %f", getpid(), pi);

    printf ("sending text: '%s'\n", text);

    close(fd[0]); // Close unused read end
    write(fd[1], text, strlen(text));

    close (fd[1]);

    exit (0);

}

int main(int argc, char *argv[])
{

    if (argc<2)
    {
        printf ("No arguments! pi\n");
        printf ("Has reseived %d arguments!\n", argc);
        printf ("%s, %s, %s\n", argv[0], argv[1], argv[2]);
        return -1;
    }

    fd[0] = atoi (argv[0]);
    fd[1] = atoi (argv[1]);

    printf ("args: %d, %d", fd[0], fd[1]);

    printf ("Child process1 has been started!\n");
    signal(SIGUSR1, start_counting_pi);
    while (1);

    return 0;
}

