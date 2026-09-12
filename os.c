#include <stdio.h>
#include "compiler.h"
#include "memory.h"
#include "processor.h"
#include "os.h"
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

bool pid[256];
bool procID[NP];

struct PCB readyQueue[NP];
int readyFront = 0;
int readyRear = 0;
int readyCount = 0;

struct PCB waitingQueue[2 * NP];
int waitingFront = 0;
int waitingRear = 0;
int waitingCount = 0;

bool shell_enabled = true;
char shell_buffer[256];
int shell_index = 0;
bool shell_prompt = false;
bool shell_line_active = false;

bool readyEnQ(struct PCB x)
{
    if (readyCount == NP)
        return false;

    readyQueue[readyRear] = x;
    readyRear = (readyRear + 1) % NP;
    readyCount++;

    return true;
}

bool readyDeQ(struct PCB *x)
{
    if (readyCount == 0)
        return false;

    *x = readyQueue[readyFront];
    readyFront = (readyFront + 1) % NP;
    readyCount--;

    return true;
}

bool waitingEnQ(struct PCB x)
{
    if (waitingCount == 2 * NP)
        return false;

    waitingQueue[waitingRear] = x;
    waitingRear = (waitingRear + 1) % (2 * NP);
    waitingCount++;

    return true;
}

bool waitingDeQ(struct PCB *x)
{
    if (waitingCount == 0)
        return false;

    *x = waitingQueue[waitingFront];
    waitingFront = (waitingFront + 1) % (2 * NP);
    waitingCount--;

    return true;
}

int getPID()
{
    for (int i = 0; i < 256; i++)
    {
        if (pid[i] == 0)
        {
            pid[i] = 1;
            return i;
        }
    }

    return -1;
}

int getProcessorID()
{
    for (int i = 0; i < NP; i++)
    {
        if (procID[i] == 0)
        {
            procID[i] = 1;
            return i;
        }
    }

    return -1;
}

void loader(char *source, char *data)
{
    struct PCB newProcess;

    newProcess.pid = getPID();

    if (newProcess.pid == -1)
        return;

    strcpy(newProcess.source, source);

    char programFileName[50] = "Program_";
    char pidStr[20];

    itoa(newProcess.pid, pidStr, 10);
    strcat(programFileName, pidStr);
    strcat(programFileName, ".byte");

    strcpy(newProcess.target, programFileName);
    strcpy(newProcess.data_file, data);

    newProcess.processor_id = getProcessorID();

    if (newProcess.processor_id == -1)
    {
        waitingEnQ(newProcess);
        if (shell_line_active)
            printf("\n");

        printf("[SHELL] Process %d added to waiting queue.\n",
               newProcess.pid);
    }
    else
    {
        compiler(newProcess.source, newProcess.target);

        initialize(
            newProcess.target,
            newProcess.data_file,
            newProcess.processor_id
        );

        readyEnQ(newProcess);

        if (shell_line_active)
            printf("\n");

        printf("[SHELL] Process %d loaded on processor %d.\n",
               newProcess.pid,
               newProcess.processor_id);
    }
}

void shell_reset()
{
    shell_index = 0;
    shell_buffer[0] = '\0';
    shell_line_active = false;
}

bool shell_parse_and_load()
{
    char source[100];
    char data[100];

    shell_buffer[shell_index] = '\0';

    if (strcmp(shell_buffer, "exit") == 0)
    {
        shell_enabled = false;
        shell_reset();
        printf("[SHELL] Shell terminated.\n");
        return false;
    }

    if (shell_index == 0)
    {
        shell_prompt = true;
        return false;
    }

    if (sscanf(shell_buffer, "%99s %99s", source, data) != 2)
    {
        printf("[SHELL] Invalid command. Format: source.txt data.byte\n");
        shell_reset();
        shell_prompt = true;
        return false;
    }

    printf("[SHELL] Loading process: %s %s\n",
           source,
           data);

    loader(source, data);

    shell_reset();
    shell_prompt = true;

    return true;
}

void shell(bool blocking)
{
    if (!shell_enabled)
        return;

    if (!shell_line_active)
    {
        printf("$");
        fflush(stdout);
        shell_line_active = true;
    }

    if (blocking)
    {
        while (shell_enabled)
        {
            char ch = _getch();

            if (ch == '\r')
            {
                shell_buffer[shell_index] = '\0';
                printf("\n");

                if (strcmp(shell_buffer, "exit") == 0)
                {
                    shell_enabled = false;
                    shell_reset();
                    printf("[SHELL] Shell terminated.\n");
                    return;
                }

                if (shell_index == 0)
                {
                    shell_reset();
                    continue;
                }

                char source[100];
                char data[100];

                if (sscanf(shell_buffer, "%99s %99s", source, data) == 2)
                {
                    printf("[SHELL] Loading process: %s %s\n",
                           source,
                           data);

                    loader(source, data);
                }
                else
                {
                    printf("[SHELL] Invalid command. Format: source.txt data.byte\n");
                }

                shell_reset();
                return;
            }

            if (ch == '\b')
            {
                if (shell_index > 0)
                {
                    shell_index--;
                    shell_buffer[shell_index] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            }
            else if (ch >= 32 && ch <= 126)
            {
                if (shell_index < 255)
                {
                    shell_buffer[shell_index++] = ch;
                    putchar(ch);
                    fflush(stdout);
                }
            }
        }
    }

    while (_kbhit())
    {
        char ch = _getch();

        if (ch == '\r')
        {
            shell_buffer[shell_index] = '\0';
            printf("\n");

            if (strcmp(shell_buffer, "exit") == 0)
            {
                shell_enabled = false;
                shell_reset();
                printf("[SHELL] Shell terminated.\n");
                return;
            }

            if (shell_index == 0)
            {
                shell_reset();
                return;
            }

            char source[100];
            char data[100];

            if (sscanf(shell_buffer, "%99s %99s", source, data) == 2)
            {
                printf("[SHELL] Loading process: %s %s\n",
                       source,
                       data);

                loader(source, data);
            }
            else
            {
                printf("[SHELL] Invalid command. Format: source.txt data.byte\n");
            }

            shell_reset();
            return;
        }

        if (ch == '\b')
        {
            if (shell_index > 0)
            {
                shell_index--;
                shell_buffer[shell_index] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
        }
        else if (ch >= 32 && ch <= 126)
        {
            if (shell_index < 255)
            {
                shell_buffer[shell_index++] = ch;
                putchar(ch);
                fflush(stdout);
            }
        }
    }
}

void scheduler()
{
    struct PCB process;
    int n = readyCount;

    for (int loop = 0; loop < n; loop++)
    {
        if (readyDeQ(&process))
        {
            for (int i = 0; i < TIME_SLICE; i++)
            {
                fetch(process.processor_id);
                decode();
                execute(process.processor_id);

                if (end_of_simulation[process.processor_id] == 1)
                    break;
            }

            if (end_of_simulation[process.processor_id] == 1)
            {
                finalize(process.processor_id, process.data_file);
                resetIND(process.processor_id);

                pid[process.pid] = 0;
                procID[process.processor_id] = 0;

                if (shell_line_active)
                    printf("\n");

                printf("[SHELL] Process %d completed.\n",
                       process.pid);

                if (waitingDeQ(&process))
                {
                    process.processor_id = getProcessorID();

                    compiler(process.source, process.target);

                    initialize(
                        process.target,
                        process.data_file,
                        process.processor_id
                    );

                    readyEnQ(process);

                    if (shell_line_active)
                        printf("\n");

                    printf("[SHELL] Process %d moved from waiting queue "
                           "to processor %d.\n",
                           process.pid,
                           process.processor_id);
                }
            }
            else
            {
                readyEnQ(process);
            }
        }
    }

    /* Shell gets its own round-robin slot. */
    shell(false);

    /* Visualization delay. */
    Sleep(500);
}

void start_os(char *source, char *data)
{
    for (int i = 0; i < 256; i++)
        pid[i] = 0;

    for (int i = 0; i < NP; i++)
        procID[i] = 0;

    reset();

    readyFront = 0;
    readyRear = 0;
    readyCount = 0;

    waitingFront = 0;
    waitingRear = 0;
    waitingCount = 0;

    shell_enabled = true;
    shell_reset();
    shell_prompt = false;
    shell_line_active = false;

    (void)source;
    (void)data;

    /* The first process is entered through the shell. */
    shell(true);

    while (shell_enabled || readyCount > 0 || waitingCount > 0)
    {
        if (readyCount > 0)
        {
            scheduler();
        }
        else
        {
            shell_prompt = true;
            shell(true);
        }
    }
}
