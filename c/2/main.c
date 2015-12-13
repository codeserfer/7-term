#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


int fd[2]; // pipe




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

	char argument1 [5];
	char argument2 [2];

	sprintf (argument1, "%d", fd[0]);
	sprintf (argument2, "%d", fd[1]);

	printf ("Arguments are %s and %s\n", argument1, argument2);

	switch (fork())
	{
        case 0:
        {
            int exec = execl("/home/codeserfer/Dropbox/Code_Blocks/HW-2-exp/bin/Debug/HW-2-exp", argument1, argument2, (char*)0 );
            if (exec == -1)
            {
                printf ("Cant execute exp program!\n Exiting..\n");
                return -1;
            }

            exit (0);
        }
        case -1:
            printf ("Can't fork! Exiting...\n");
            return -1;
	}

	switch (fork())
	{
        case 0:
        {
            int exec = execl("/home/codeserfer/Dropbox/Code_Blocks/HW-2-pi/bin/Debug/HW-2-pi", argument1, argument2, (char*)0 );
            if (exec == -1)
            {
                printf ("Cant execute pi program!\n Exiting..\n");
                return -1;
            }

            exit (0);
        }
        case -1:
            printf ("Can't fork! Exiting...\n");
            return -1;
	}


    sleep (1);

    printf ("Sending signals...\n");
    signal(SIGUSR1, DoNoting);
    signal(SIGUSR2, DoNoting);
    printf("\nPARENT: sending SIGQUIT\n\n");

    int grp = getpgrp ();
    kill(-1*grp, SIGUSR1);
    printf ("grp is %d\n", grp);
    //printf ("child process is %d\n", pid);
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

    printf ("end program!\n");
}

