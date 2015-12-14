#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_FILE "MYFIFO"
#define SECONDS 2


void WriteToPipe (char* fifo_file, char* sending_string, int random_priority, int process_id)
{
    FILE* fp;

    if((fp = fopen(fifo_file, "w")) == NULL)
    {
        perror("open error");
        exit(1);
    }

    if (random_priority)
    {
        int priority = rand()%11;
        setpriority(priority, process_id);
        //printf ("Process priority has been changed: %d\n", priority);
    }

    // send data to server
    fputs(sending_string, fp);
    fclose(fp);
}

void ListenPipe (char* fifo_file, int timeout)
{
    printf ("CHILD: Listening child pipe!\n");

    int received_count = 0;

    time_t start = NULL;
    if (timeout)
    {
        printf ("CHILD: timeout = %d\n", timeout);
        start = time(0);
    }

    unlink(fifo_file); // remove FIFO file, if exist
	int fp, n;
	char readbuf[80];

	umask(0); // set file mask for creating files
	mknod(fifo_file, S_IFIFO | 0777, 0); // create FIFO file

	while(1)
	{
        if (start)
        {
            double seconds_since_start = difftime( time(0), start);
            if (seconds_since_start >=timeout)
                break;
        }

		// read from client
		fp = open(fifo_file, O_RDONLY); // open FIFO file
		while((n = read(fp, readbuf, 80)) > 0) // read while data exists
		{
            //printf ("CHILD RECEIVED: ");
			////write(1, readbuf, n); // show received data
			//printf (readbuf);
			//printf ("\n");
			//printf ("has reseived: %d symbols\n", n);
			received_count++;
        }
		close(fp);
	}

	// ?
	if (timeout)
	{
        close(fifo_file);
        unlink(fifo_file);
    }

    printf ("CHILD: Has been received %d bytes\n", received_count);

}

main()
{
    printf ("program has started!\n");


    // first child process start
    switch (fork())
    {
        case 0:
        {
            printf ("Child process started! pid = %d\n", getpid());

            // get current directory
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                fprintf(stdout, "Current working dir: %s\n", cwd);
            else
                perror("getcwd() error");

            // current directory plus file name of pipe (pid)
            char fifo_file [1024];
            sprintf (fifo_file, "%s/%d", cwd, getpid());

            //printf ("fifo_file: %s\n", fifo_file);

            //WRITE
            WriteToPipe (FIFO_FILE, fifo_file, 0, 0);

            ListenPipe(fifo_file, SECONDS);


            unlink (fifo_file);

            exit(0);
        }
        case -1:
            printf ("Can't fork!\n");
            return 0;
    }
    // first child process end

    // second child process start
    switch (fork())
    {
        case 0:
        {
            printf ("Child process started! pid = %d\n", getpid());

            // get current directory
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                fprintf(stdout, "Current working dir: %s\n", cwd);
            else
                perror("getcwd() error");

            // current directory plus file name of pipe (pid)
            char fifo_file [1024];
            sprintf (fifo_file, "%s/%d", cwd, getpid());

            //printf ("fifo_file: %s\n", fifo_file);

            // Write
            WriteToPipe (FIFO_FILE, fifo_file, 0, 0);

            // Listen
            ListenPipe(fifo_file, SECONDS);


            printf ("unlinling file '%s'\n", fifo_file);
            close (fifo_file);
            int rrrr = unlink (fifo_file);
            printf ("Unlink result = %d\n", rrrr);

            exit(0);
        }
        case -1:
            printf ("Can't fork!\n");
            return 0;
    }
    // second child process end



    // main process start
    printf ("main process has been started\n");

	unlink(FIFO_FILE); // remove FIFO file, if exist
	int fp, n;
	char readbuf[80];

	umask(0); // set file mask for creating files
	mknod(FIFO_FILE, S_IFIFO | 0777, 0); // create FIFO file
	while(1)
	{
		// read from client
		fp = open(FIFO_FILE, O_RDONLY); // open FIFO file
		while((n = read(fp, readbuf, 80)) > 0) // read while data exists
		{
            // here are received data
			//write (1, readbuf, n);
			printf ("Child pipe file: '%s'\n", readbuf);


			switch (fork())
            {
                case 0:
                {
                    // working with client
                    // send some data

                    srand(time(NULL));

                    int i = 0;
                    //for (; i < 10; i++)
                    for (;;)
                    {
                        char r = rand() % 100 + 50;
                        char sending_symbol [2];
                        sprintf (sending_symbol, "%c%c", r, '\0');
                        //printf ("%s ", sending_symbol);

                        WriteToPipe(readbuf, sending_symbol, 1, getpid());
                    }



                    exit (0);
                }
                case -1:
                    printf ("Can't fork!\n");
                    return 0;
            }
        }

		close(fp);

		// write echo replay to client
		fp = open(FIFO_FILE, O_WRONLY);
		write(fp, readbuf, strlen(readbuf));
		close(fp);
	}

	// main process end
}

