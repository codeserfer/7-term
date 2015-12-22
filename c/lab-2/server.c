#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/resource.h>

#define FIFO_FILE "SERVERFIFO"
#define TIMEOUT 3

int main()
{

    printf ("Server has been started!\n");
	srand(time(0));

	unlink(FIFO_FILE);
	int fp;
	char readbuf[80];

	umask(0);
	mkfifo(FIFO_FILE, 0777);
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

                    char temp [200];

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

