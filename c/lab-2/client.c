#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

#define FIFO_FILE "/home/codeserfer/Dropbox/Code_Blocks/ya-server/bin/Debug/SERVERFIFO"

#define TIMEOUT 3

int main()
{
	umask(0);
	srand(time(0));
	int fp;
    char readbuf[80];

    if((fp = open(FIFO_FILE, O_RDWR)) == -1)
    {
        perror("open error");
        exit(1);
    }

    // get current directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");

    char fifo_file [1024];
    sprintf (fifo_file, "%s/%d", cwd, getpid());

    printf ("fifo_file = %s\n", fifo_file);

	mkfifo(fifo_file, 0777);

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
			printf ("%c", ch);
		}
	}

	printf ("total received symbols: %d\n", transeferedCount);
	close(fp);
	unlink(fifo_file);
    return 0;
}

