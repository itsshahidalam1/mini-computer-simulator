#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <stdbool.h>
#define NP 4
extern bool N;
extern bool Z;
extern bool C;
extern bool V;
extern int Register[NP][256];
extern unsigned char instruction_memory[NP][256];
extern unsigned char data_memory[NP][4096];
extern int end_of_simulation[NP];
void reset();
void resetIND(int);
void fetch(int);
void decode();
void execute(int);
#endif