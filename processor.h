#ifndef PROCESSOR_H
#define PROCESSOR_H

extern int Register[256];
extern unsigned char instruction_memory[256];
extern unsigned char data_memory[256];
extern int end_of_simulation;
void reset();
void fetch();
void decode();
void execute();

#endif