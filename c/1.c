#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>

double CalculateExp(double value, int maxNumberOfOperations)
{

    printf ("argument is %lf", value);
    double exp = 1;
    int i;
    double factorial = 1;
    double power;

    for(i = 0; i < maxNumberOfOperations; i++)
    {
        power = i + 1;
        factorial *= power;
        /*if (i > 300) {
            printf("%lf", power);
            printf("\n");
        }*/
        exp += pow(value, power)/factorial;

    }

    return exp;
}

double CalculatePI (int maxNumberOfOperations)
{
    double pi = 1.0;
    int numerator = 2;
    int denominator = 1;

    int numerator_counter = 0;
    int denominator_counter = 1;

    unsigned int i;
    for (i = 0; i<maxNumberOfOperations; i++)
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

    return 2*pi;
}

void ClearFile ()
{
    FILE *f = fopen("/home/codeserfer/Dropbox/Code_Blocks/HW-11/file", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    fclose(f);
}

void WriteFile (char* content)
{
    FILE *f = fopen("/home/codeserfer/Dropbox/Code_Blocks/HW-11/file", "a+");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    fprintf(f, content);
    fclose(f);
}


int main ()
{
    int x = 1;
    ClearFile();

    int proc1;

    switch (fork())
    {
        case 0:
            printf ("first fork()\n");
            double exp = CalculateExp(-(pow(x, 2.0))/2, 180);
            printf ("exp = %f\n", exp);

            char text [40];
            sprintf(text, "%d exp %f", getpid(), exp);

            WriteFile(text);

            exit (0);
        case -1:
            printf ("Error!!!!!! Close program!");
            return -1;
    }

    int proc2;
    switch (fork())
    {
        case 0:
            printf ("second fork()\n");
            //double pi = CalculatePI (1000000000);
            double pi = CalculatePI (1000000000);
            printf ("pi = %f\n", pi);

            char text [40];
            sprintf(text, "@%d pi %f", getpid(), pi);

            WriteFile(text);

            exit (0);
        case -1:
            printf ("Error!!!!!! Close program!");
            return -1;
    }

    wait(&proc1);
    wait(&proc2);

    FILE* file = fopen("/home/codeserfer/Dropbox/Code_Blocks/HW-11/file", "r");
    if (file == NULL)
    {
        printf ("Can't open file! file is %d\n", file);
    }

    while (1)
    {

        char buffer [1024];
        size_t nread;

        rewind (file);

        if (nread = fread(buffer, 1, sizeof buffer, file) <= 0)
        {
            continue;
        }
        else
        {
            char* e = strchr (buffer, '@');

            if (e == NULL)
            {
                continue;
            }
            else
            {
                int index = (int)(e - buffer);
                printf ("found!\n");


                int pID1 = 0;
                int pID2 = 0;
                char fname1 [4];
                char fname2 [4];
                double result1 = 0;
                double result2 = 0;

                rewind (file);

                int fscanf_result = fscanf(file, "%d %s %lf@%d %s %lf", &pID1, fname1, &result1, &pID2, fname2, &result2);

                if (fscanf_result < 6)
                {
                    printf ("Something went wrong...\n");
                    return 0;
                }

                //printf ("first:  %d  %s  %lf \n", pID1, fname1, result1);
                //printf ("second: %d  %s  %lf \n", pID2, fname2, result2);

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

                break;
            }
        }
    }

    printf ("Close program\n");

    return 0;
}
