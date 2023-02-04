#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define FILENAME_SIZE 25

void sigHandler(int sigNum);
void getFileName(void);
void *dispatchFile(void *arg);
void *workerFileAccess(void *arg);

char filename[FILENAME_SIZE];
int fileRequests = 0;

int main(void)
{
    pthread_t dispatchThread;
    void *dispatchThreadResult;
    int dispatchThreadStatus;
    int joinStatus;

    srand(time(NULL));
    signal(SIGINT, sigHandler);

    while(1)
    {
        dispatchThreadStatus = pthread_create(&dispatchThread, NULL, dispatchFile, NULL);

        joinStatus = pthread_join(dispatchThread, &dispatchThreadResult);

        if (joinStatus != 0)
        {
            fprintf(stderr, "[main thread] - Join error %d: %s\n", joinStatus, strerror(joinStatus));
            exit(1);
        }
    }

    return 0;
}

void *workerFileAccess(void *arg)
{
    int nMin = 7, nMax = 10;
    fileRequests++;
    // simulates file access duration:
    // (80% of the time sleep for 1 second)
    // (20% of the time sleep for 7 to 10 seconds)
    (rand() % 10 < 8) ? sleep(1) : sleep(rand() % ((nMax + 1) - (nMin) + nMin) );

    printf("\nworker received filename: \"%s\"\n", (char *) arg);

    return NULL;
}

void *dispatchFile(void *arg)
{
    getFileName(); // prompt user for a filename

    pthread_t workerThread;
    void *workerThreadResult;
    int workerThreadStatus;
    int joinStatus;

    workerThreadStatus = pthread_create(&workerThread, NULL, workerFileAccess, filename);

    joinStatus = pthread_join(workerThread, &workerThreadResult);

    if (joinStatus != 0)
    {
        fprintf(stderr, "[dispatch thread] - Join error %d: %s\n", joinStatus, strerror(joinStatus));
        exit(1);
    }

    return workerThreadResult;
}

void getFileName(void)
{
    fputs("Enter a filename: ", stdout);
    fgets(filename, sizeof(filename), stdin);
    filename[strlen(filename) - 1] = '\0';  // adds null char at the end
}

void sigHandler(int sigNum)
{
    printf(" received. \n\nDisplaying program statistics...\n");
    printf("\n---------- Program Statistics ---------- \n");
    printf("File requests: %d\n", fileRequests);
    printf("---------------------------------------- \n");
    printf("Terminating program...\n");
    exit(0);
}
