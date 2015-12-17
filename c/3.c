#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define FIFO_FILE "MYFIFO"
#define SECONDS 2

// Функция записи в FIFO
// В нее передаются:
// fifo_file - файл ФИФО
// sending_string - записываемая строка
// random_priority - по задумке типа необязательный параметр. Если не 0 - то при каждой отправке меняется приоритет процесса
// process_id - опять же типа необязательный параметр, для смены приоретета процесса нам как бы надо знать его id, вот мы его и передали
// Короче это может быть немного странно так использовать типа необязательные параметры, но в си нет рантайма, так что либо так, либо дублирование кода.
void WriteToFIFO (char* fifo_file, char* sending_string, int random_priority, int process_id)
{
    FILE* fp;

    if((fp = fopen(fifo_file, "w")) == NULL)
    {
        perror("open error");
        exit(1);
    }

	// Если параметр не 0, то меняем приоритет у процесса случайным образом
    if (random_priority)
    {
        int priority = rand()%11;
        setpriority(priority, process_id);
        //printf ("Process priority has been changed: %d\n", priority);
    }

    // send data to server
	// Записываем строку в FIFO
    fputs(sending_string, fp);
    fclose(fp);
}


// Получает данные из FIFO в бесконечном цикле - прерывается он по таймауту
// Считаем количество полученных байтов
void ListenFIFO (char* fifo_file, int timeout)
{
    printf ("CHILD: Listening child FIFO!\n");
	
	// Собственно счетчик байтов
    int received_count = 0;

	// Для подсчета времени
    time_t start = NULL;
    if (timeout)
    {
        start = time(0);
    }

    unlink(fifo_file); // remove FIFO file, if exist
	int fp, n;
	char readbuf[80];

	umask(0); // set file mask for creating files
	mknod(fifo_file, S_IFIFO | 0777, 0); // create FIFO file

	while(1)
	{
		// если счетчик времени проициализирован
        if (start)
        {
            double seconds_since_start = difftime( time(0), start);

			// Если мы превысили таймаут - выходим из цикла
            if (seconds_since_start >=timeout)
                break;
        }

		// read from client
		fp = open(fifo_file, O_RDONLY); // open FIFO file
		while((n = read(fp, readbuf, 80)) > 0) // read while data exists
		{
			// Считаем количество полученных байтов
			received_count+=n;
        }
		close(fp);
	}

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

	
	// Можно было бы написать еще программу-клиент. Но я ленивый, поэтому у меня просто два форка - типа два клиента.
	
	// Первый клиент
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

			// Я решил использовать для каждого процесса файл ФИФО следующего вида: <путь до папки>/<ID процесса>
			// Так точно будет уникальное имя.
            // current directory plus file name of FIFO (pid)
            char fifo_file [1024];
            sprintf (fifo_file, "%s/%d", cwd, getpid());

            //printf ("fifo_file: %s\n", fifo_file);

            //WRITE
			// Третий и четвертый параметры - это типа наши необязательные, поэтому 0
            WriteToFIFO (FIFO_FILE, fifo_file, 0, 0);

			// Получаем байты от сервера. И там же и считаем.
            ListenFIFO(fifo_file, SECONDS);

            unlink (fifo_file);

            exit(0);
        }
        case -1:
            printf ("Can't fork!\n");
            return 0;
    }
    // first child process end

    // second child process start
	// Точно так же как первый клиент.
	// Второй клиент
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

            // current directory plus file name of FIFO (pid)
            char fifo_file [1024];
            sprintf (fifo_file, "%s/%d", cwd, getpid());

            //printf ("fifo_file: %s\n", fifo_file);

            // Write
            WriteToFIFO (FIFO_FILE, fifo_file, 0, 0);

            // Listen
            ListenFIFO(fifo_file, SECONDS);


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
	// А это наш сервер собственно
    printf ("main process has been started\n");

	unlink(FIFO_FILE); // remove FIFO file, if exist
	int fp, n;
	char readbuf[80];

	umask(0); // set file mask for creating files
	mknod(FIFO_FILE, S_IFIFO | 0777, 0); // create FIFO file
	while(1)
	{
		// read from client

		// Собственно здесь к серверу могут по его общеизвестному ФИФО подключиться клиенты. Для работы с каждым из них надо создать еще один процесс (fork()) и 
		// Собственно осуществлять передачу данных уже по FIFO клиента. Как раз клиент и отправляем нам путь к его ФИФО. И по нему мы и будем с ним общаться.
		
		fp = open(FIFO_FILE, O_RDONLY); // open FIFO file
		while((n = read(fp, readbuf, 80)) > 0) // read while data exists
		{
            // here are received data
			//write (1, readbuf, n);
			printf ("Child FIFO file: '%s'\n", readbuf);

			// Получили файл ФИФО от клиента - форкаемся, чтобы создать новый процесс для общения с клиентом по его ФИФО.
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

                        WriteToFIFO(readbuf, sending_symbol, 1, getpid());
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

