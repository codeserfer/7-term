#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int counting_exp = 1;

int fd[2]; // pipe

void end_counting_exp(int sig)
{
    printf ("CHILD:  End counting exp!\n");
    counting_exp = 0;
}

void start_counting_exp(int sig)
{
    printf ("CHILD:  Start counting exp!\n");
    signal(SIGUSR2, end_counting_exp);

    // TODO: fix this
    int x = 2;
    double value = -(pow(x, 2.0))/2;

    // counting
    printf ("argument is %lf", value);
    double exp = 1;
    int i = 0;
    double factorial = 1;
    double power;

    while (counting_exp && i < 300)
    {
        power = i + 1;
        factorial *= power;
        exp += pow(value, power)/factorial;
        i++;

    }


    printf ("exp is %f\n", exp);

    char text [40];
    sprintf(text, "%d exp %f", getpid(), exp);

    printf ("sending text: '%s'\n", text);

    close(fd[0]); // Close unused read end
    write(fd[1], text, strlen(text));

    exit (0);

}


int main(int argc, char *argv[])
{
    if (argc<2)
    {
        printf ("No arguments! exp\n");
        printf ("Has reseived %d arguments!\n", argc);
        printf ("%s, %s, %s\n", argv[0], argv[1], argv[2]);
        return -1;
    }

    fd[0] = atoi (argv[0]);
    fd[1] = atoi (argv[1]);

    printf ("args: %d, %d", fd[0], fd[1]);

    printf ("Child process2 has been started!\n");
    signal(SIGUSR1, start_counting_exp);
    while (1);


    return 0;
}

