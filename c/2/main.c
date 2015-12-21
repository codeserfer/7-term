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




	int dataToChild1[2];
	int dataFromChild1[2];
	int dataToChild2[2];
	int dataFromChild2[2];

	pipe(dataToChild1);
	pipe(dataFromChild1);
	pipe(dataToChild2);
	pipe(dataFromChild2);



	switch (fork())
    {
        case 0:
        {
            printf ("First child process\n");

            dup2(dataToChild1[0], 0);
            dup2(1, 2);
            dup2(dataFromChild1[1], 1);

            if (execl ("/home/codeserfer/Dropbox/Code_Blocks/LAB-1-PI/bin/Debug/LAB-1-PI", 0) == -1)
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

            dup2(dataToChild2[0], 0);
            dup2(1, 2);
            dup2(dataFromChild2[1], 1);

            write(dataToChild2[1], &x, sizeof(int));

            if (execl ("/home/codeserfer/Dropbox/Code_Blocks/LAB-1-EXP/bin/Debug/LAB-1-EXP", 0) == -1)
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

	read(dataFromChild1[0], &pi, sizeof(double));
	read(dataFromChild2[0], &exp, sizeof(double));

    printf ("exp = %f\n", exp);
	printf ("pi = %f\n", pi);

	double result = exp / sqrt(2.0 * pi);


	printf ("result: %lf",result);

	return 0;
}

