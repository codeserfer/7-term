#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

int counting_pi = 1;
int counting_exp = 1;
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


//
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


void DoNoting ()
{
    printf ("pass\n");
}


main()
{
    if (pipe(fd) < 0)
	{
		printf("Cannot create pipe!\n");
		return 1;
	}

    int pid;
    if ((pid=fork()) == 0)
    {
        printf ("Child process1 has been started!\n");
        signal(SIGUSR1, start_counting_pi);
        while (1);

    }
    else
    {

        int pid2;
        if ((pid2=fork()) == 0)
        {
            printf ("Child process2 has been started!\n");
            signal(SIGUSR1, start_counting_exp);
            while (1);
        }
        else
        {
            printf ("Main pipe!\n");
            sleep (1);
            signal(SIGUSR1, DoNoting);
            signal(SIGUSR2, DoNoting);
            printf("\nPARENT: sending SIGQUIT\n\n");

            int grp = getpgrp ();
            kill(-1*grp, SIGUSR1);
            printf ("grp is %d\n", grp);
            printf ("child process is %d\n", pid);

            sleep(1);
            kill(-1*grp, SIGUSR2);
            sleep (3);

            ////
            printf ("main () go!\n");


            int pID1 = 0;
            int pID2 = 0;
            char fname1 [4];
            char fname2 [4];
            double result1 = 0;
            double result2 = 0;


            close(fd[1]); // Close unused write end


            char buf [40];
            read(fd[0], &buf, sizeof (buf));

            printf ("buf is '%s'\n", buf);


            int fscanf_result = sscanf(buf, "%d %s %lf@%d %s %lf", &pID1, fname1, &result1, &pID2, fname2, &result2);

            if (fscanf_result < 6)
            {
                printf ("Something went wrong...\n");
                return 0;
            }

            printf ("first:  %d  %s  %lf \n", pID1, fname1, result1);
            printf ("second: %d  %s  %lf \n", pID2, fname2, result2);

            double exp = 0, pi = 0;

            if (!strcmp (fname1, "exp"))
            {
                exp = result1;
                pi = result2;
            }
            else
            {
                exp = result2;
                pi = result1;
            }

            double final_result = exp/sqrt(2*pi);
            printf ("result: %lf\n", final_result);

        }
    }

    printf ("end program!\n");
}

