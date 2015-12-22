#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/resource.h>

#define FIFO_FILE "SERVERFIFO"
#define TIMEOUT 3

void MakeFIFO (char* fifoFile)
{
    unlink(fifoFile);
    umask(0);
    mkfifo(fifoFile, 0777);
}

int main()
{
    printf ("Server has been started!\n");
	srand(time(0));

	//unlink(FIFO_FILE);
	int fp;
	char readbuf[80];

	//umask(0);
	//mkfifo(FIFO_FILE, 0777);
	MakeFIFO(FIFO_FILE);


    switch (fork())
    {
        case 0:
        {
            printf ("Fork!\n");

            umask(0);
            srand(time(0));
            int fp;


            if((fp = open(FIFO_FILE, O_RDWR)) == -1)
            {
                perror("open error");
                exit(1);
            }


            char fifo_file [1024];
            sprintf (fifo_file, "%d", getpid());

            printf ("fifo_file = %s\n", fifo_file);

            ////
            MakeFIFO (fifo_file);
            //unlink(fifo_file);
            //umask(0);
            //mkfifo(fifo_file, 0777);

            write(fp, fifo_file, strlen(fifo_file)+1);
            close(fp);

            fp = open(fifo_file, O_NONBLOCK | O_RDWR);

            time_t t = time(NULL);
            int transeferedCount = 0;
            while((time(NULL) - t) < TIMEOUT)
            {
                char ch;
                if(read(fp, &ch, sizeof(char)))
                {
                    transeferedCount++;
                    //printf ("%c", ch);
                }
            }

            printf ("total received symbols: %d\n", transeferedCount);
            close(fp);
            unlink(fifo_file);
            return 0;



            exit(0);
        }
        case -1:
            printf ("Can't fork!\n");
            return 0;
    }



	fp = open(FIFO_FILE, O_RDONLY);

	//client fork




	while(1)
	{
		if(read(fp, readbuf, 80))
        {
            switch (fork())
            {
                case 0:
                {
                    close(fp);


                    int clientFifo = open(readbuf, O_RDWR);
                    char ch = 33 + (rand() % 94);


                    while((write(clientFifo, &ch, sizeof(char)) > 0))
                    {
                        usleep(rand() % 150);
                        ch = 33 + (rand() % 94);

                        if(rand() % 100 > 80 )
                            setpriority(PRIO_PROCESS, 0, rand()%11);
                    }

                    close(clientFifo);  // закрываем клиентский канал
                    exit (0);
                }
                case -1:
                    printf ("Can't fork!\n");
                    return 0;
            }
		}

	}
	close(fp);
	unlink(FIFO_FILE);
}

