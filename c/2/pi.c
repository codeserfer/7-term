#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>

// Само вычисляемое ПИ
double pi = 0;

// "Переменная-флажок" для цикла вычисления
int counting_pi = 1;

void end_counting_pi(int sig)
{
	// Завершаем цикл
    counting_pi = 0;
}


void start_counting_pi(int sig)
{
	// "Вешаем" на сигнал SIGUSR2 окончание вычисления ПИ
    signal(SIGUSR2, end_counting_pi);

    write(2, "Calculating PI has started started\n", strlen("Calculating PI has started started\n")+1);

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

    // end counting pi

    pi = 2*pi;


    // пишем данные в выходной поток, который перехватывается родительским процессом
	write(1, &pi, sizeof(double));

	// выводим на консоль информацию, для этого используем поток ошибок, перенаправленный на поток вывода
	write(2, "PI child was killed\n", strlen("PI child was killed\n")+1);

    exit (0);

}

int main(int argc, const char * argv[])
{
	// "Вешаем" на сигнал SIGUSR1 начало вычисления ПИ
	signal(SIGUSR1, start_counting_pi);

	// Я не придумал ничего умнее. Это некое ожидание сигнала SIGUSR1 :DDD
    while (1);

    return 0;
}

