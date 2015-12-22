#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int counting_exp = 1;
int x = 0;

double fact(double num)
{
	if(num <= 0.0)
		return 1.0;
	if(num == 1.0)
		return 1.0;
	else
		return fact(num-1.0)*num;
}

void end_counting_exp(int sig)
{
    counting_exp = 0;
}

void start_counting_exp(int sig)
{
    write(2, "Calculating exp has started started\n", strlen("Calculating exp has started started\n")+1);
    signal(SIGUSR2, end_counting_exp);

    double value = -(pow(x, 2.0))/2;

    // counting
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

    // end counting

    write(1, &exp, sizeof(double));


	// выводим на консоль информацию, для этого используем поток ошибок, перенаправленный на поток вывода
	write(2, "exp child was killed\n", strlen("exp child was killed\n")+1);

    exit (0);
}

int main(int argc, const char * argv[])
{
    signal(SIGUSR1, start_counting_exp);


	read(0, &x, sizeof(int));

	write(2, "second child started\n", strlen("second child started\n")+1);


    while (1);


    return 0;
}

