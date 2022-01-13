#include <sys/cdefs.h>
/*
 * ASSIGNMENT - 2
 * SOURCE CODE
 *
 * Student Name: ARAL SEN
 * ITU Number : 150170217
 *
 *
 * The following commands can be used to compile and run the program:
 * $ gcc assignment2.c -o assignment2 -pthread -w
 * $ ./a.out 5 6
 */

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Cond;

typedef struct {
    int value, wakeup;
    Mutex *mutex;
    Cond *cond;
} Semaphore;

int oxygen=0, hydregen=0;

Semaphore *mutex, *barrier, *oxyQueue, *hydroQueue;
pthread_t oxyThread, hydroThread1, hydroThread2;

Mutex* make_mutex(void);

Mutex* make_mutex(void)
{
    int n;
    Mutex* mutex = (Mutex*)malloc(sizeof(Mutex));
    n = pthread_mutex_init(mutex, NULL);
    if (n != 0)
        perror("make_lock failed");

    return(mutex);
}

void mutex_lock(Mutex *mutex)
{
    int n= pthread_mutex_lock(mutex);
    if (n != 0)
        perror("lock failed");
}

void mutex_unlock(Mutex *mutex)
{
    int n= pthread_mutex_unlock(mutex);
    if (n != 0)
        perror("unlock failed");
}

Cond* make_cond(void)
{
    int n;
    Cond* cond = (Cond*)malloc(sizeof(Cond));
    n= pthread_cond_init(cond, NULL);
    if (n != 0)
        perror("Make_cond failed");
    return (cond);
}

void cond_wait(Cond* cond, Mutex* mutex)
{
    int n= pthread_cond_wait(cond,mutex);
    if (n != 0)
        perror("cond_wait failed");
}

void cond_signal(Cond* cond)
{
    int n= pthread_cond_signal(cond);
    if (n != 0)
        perror("cond_signal failed");
}

Semaphore* make_semaphore(int value)
{
    Semaphore *semaphore = (Semaphore*)malloc(sizeof(Semaphore));
    semaphore -> value=value;
    semaphore -> wakeup=0;
    semaphore -> mutex = make_mutex();
    semaphore -> cond = make_cond();
    return (semaphore);
}

void sem_wait(Semaphore *semaphore)
{
    mutex_lock(semaphore->mutex);
    semaphore->value--;
    if (semaphore-> value < 0) {
        do {
            cond_wait(semaphore->cond, semaphore->mutex);
        } while (semaphore->wakeup < 1);
        semaphore->wakeup--;
    }
    mutex_unlock(semaphore->mutex);
}

void sem_signal(Semaphore *semaphore)
{
    mutex_lock(semaphore->mutex);
    semaphore->value++;
    if (semaphore->value <= 0)
    {
        semaphore->wakeup++;
        cond_signal(semaphore->cond);
    }
    mutex_unlock(semaphore->mutex);
}

int bond(void)
{
    static  int i = 0;
    i++;
    if ((i%3) == 0) printf("** Molecule no. %d created**\n\n", i/3);
    sleep(2);
    return(0);
}

_Noreturn void* oxyFn(void* arg)
{
    while (1) {
        sem_wait(mutex);
        oxygen+=1;
        if (hydregen>=2)
        {
            sem_signal(hydroQueue);
            sem_signal(hydroQueue);
            hydregen-=2;
            sem_signal(oxyQueue);
            oxygen-=1;
        }
        else {
            sem_signal(mutex);
        }
        sem_wait(oxyQueue);
        printf("oxygen bond\n");
        bond();

        sem_signal(mutex);
    }
}

_Noreturn void* hydroFn(void* arg)
{
    while (1) {
        sem_wait(mutex);
        hydregen +=1;
        if (hydregen>=2 && oxygen >= 1)
        {
            sem_signal(hydroQueue);
            sem_signal(hydroQueue);
            hydregen-=2;
            sem_signal(oxyQueue);
            oxygen-=1;
        }
        else {
            sem_signal(mutex);
        }
        sem_wait(hydroQueue);
        printf("hydregen bond\n");
        bond();
    }
}


main(__attribute__((unused)) int argc,char* argv[])
{
//    int n = atoi(argv[1]); //number of oxygen atoms
//    int m = atoi(argv[2]); //number of hydrogen atoms

    void *arg;

    mutex = make_semaphore(1);
    barrier = make_semaphore(0);
    oxyQueue = make_semaphore(0);
    hydroQueue = make_semaphore(0);

    pthread_create(&oxyThread, NULL, oxyFn,arg);
    pthread_create(&hydroThread1,NULL,hydroFn,arg);
    pthread_create(&hydroThread2,NULL,hydroFn,arg);
    for(;;);
}