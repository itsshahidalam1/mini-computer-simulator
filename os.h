#ifndef OS_H
#define OS_H

#define TIME_SLICE 5

struct PCB
{
    int pid;
    int processor_id;
    char source[100];
    char target[100];
    char data_file[100];
};

extern struct PCB readyQueue[NP];
extern struct PCB waitingQueue[2 * NP];

extern int readyFront;
extern int readyRear;
extern int readyCount;

extern int waitingFront;
extern int waitingRear;
extern int waitingCount;

void scheduler();
void loader(char *source,char *data);
void start_os(char *source,char *data);

#endif