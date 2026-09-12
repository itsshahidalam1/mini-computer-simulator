#ifndef MEMORY_H
#define MEMORY_H
#include "processor.h"
void initialize(char inst[20], char data[20],int processorID);
void finalize(int,char*);
extern unsigned char instruction_memory[NP][256];
extern unsigned char data_memory[NP][4096];

#endif