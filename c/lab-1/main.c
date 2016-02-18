#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>


// Суть: это главная программа. Мы запускаем две дочерние программы: для вычисления экспоненты и ПИ разложением в ряд.
// Наши дочерние программы запускают вычисления по сигналу SIGUSR1 и заканчивают по сигналу SIGUSR2.
// Потом мы просто выводим результат.
// Эти сигналы для простоты мы посылаем всей группе процессов. В том числе главный процесс его тоже поймает.
// Чтобы при этом главный процесс ничего не делал, написана тупая функция do_nothing ().
// В дочерник программах перенаправлены потоки вывода.

void do_nothing ()
{
    printf ("pass\n");
}


int main(int argc, const char * argv[]) {
	int k = 0;

	// for example
	int x = 2;

	int fd [2];
	int fd2 [2];

	pipe(fd);
	pipe(fd2);




	switch (fork())
    {
        case 0:
        {
            printf ("First child process\n");

            dup2 (fd[0], 0);
            dup2 (1, 2);
            dup2 (fd[1], 1);
			close (fd[0]);

            if (execl ("pi", 0) == -1) //path to pi program
            {
                printf ("Can't execute program for pi!\n");
                return 0;
            }

            exit (0);
        }
        case -1:
            printf ("Error!!!!!! Close program!");
            return -1;
    }


    switch (fork())
    {
        case 0:
        {
            printf ("Second child process\n");

            dup2 (fd2[0], 0);
            dup2 (1, 2);
            dup2 (fd2[1], 1);
			close (fd[0]);

            write(fd2[1], &x, sizeof(int));

            if (execl ("exp", 0) == -1) //path to exp program
            {
                printf ("Can't launch second child!\n");
                return 0;
            }

            exit (0);

        }
        case -1:
            printf ("Error!!!!!! Close program!");
            return -1;
    }

    sleep (2);


	int grp = getpgrp ();


	printf ("Starting calculating!\n");

    signal(SIGUSR1, do_nothing);
    signal(SIGUSR2, do_nothing);

    kill(-1*grp, SIGUSR1);

	printf ("Sleep for 3 sec...\n");
	sleep(3);
	printf ("Wake up!\n");

	kill(-1*grp, SIGUSR2);

	double exp = 0;
	double pi = 0;

	read(fd[0], &pi, sizeof(double));
	read(fd2[0], &exp, sizeof(double));

    printf ("exp = %f\n", exp);
	printf ("pi = %f\n", pi);

	double result = exp / sqrt(2.0 * pi);


	printf ("result: %lf",result);

	return 0;
}

