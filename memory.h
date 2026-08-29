#ifndef MEMORY_H
#define MEMORY_H

void initialize(char inst[20], char data[20]);
void finalize();
extern unsigned char instruction_memory[256];
extern unsigned char data_memory[4096];

#endif