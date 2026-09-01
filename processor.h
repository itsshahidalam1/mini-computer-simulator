#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <stdbool.h>
extern bool N;
extern bool Z;
extern bool C;
extern bool V;
extern int Register[256];
extern unsigned char instruction_memory[256];
extern unsigned char data_memory[4096];
extern int end_of_simulation;
void reset();
void fetch();
void decode();
void execute();
#endif