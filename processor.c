#include <stdio.h>

int Register[256];
int PC = 0, opcode, dest, src1, src2;
extern unsigned char instruction_memory[256];
extern unsigned char data_memory[256];

int end_of_simulation = 0;

void reset()
{
    for (int i = 0; i < 256; i++)
        Register[i] = 0;
}

void fetch()
{
    if (instruction_memory[PC] == 0)
    {   
        printf("\n****************Program executed successfully***************\n");
        end_of_simulation = 1;
        return;
    }
    opcode = instruction_memory[PC];
    dest = instruction_memory[PC + 1];
    src1 = instruction_memory[PC + 2];
    src2 = instruction_memory[PC + 3];

    PC = PC + 4;
}

void decode(){
    return;
};

void execute()
{
    if (end_of_simulation == 1)
        return;
    
    switch (opcode)
    {
    case 1:
        printf("\n[EXECUTE] ADD   R%d = R%d + R%d\n", dest, src1, src2);
        Register[dest] = Register[src1] + Register[src2];
        break;

    case 2:
        printf("\n[EXECUTE] SUB   R%d = R%d - R%d\n", dest, src1, src2);
        Register[dest] = Register[src1] - Register[src2];
        break;

    case 3:
        printf("\n[EXECUTE] MUL   R%d = R%d * R%d\n", dest, src1, src2);
        Register[dest] = Register[src1] * Register[src2];
        break;

    case 4:
        printf("\n[EXECUTE] DIV   R%d = R%d / R%d\n", dest, src1, src2);
        Register[dest] = Register[src1] / Register[src2];
        break;

    case 5:
        printf("\n[EXECUTE] LOAD  R%d = MEM[%d]\n", dest, src1);
        Register[dest] = data_memory[src1];
        break;

    case 6:
        printf("\n[EXECUTE] STORE MEM[%d] = R%d\n", dest, src1);
        data_memory[src1] = Register[dest];
        break;

    case 7:
        printf("\n[EXECUTE] MOV   R%d = %d\n", dest, src1);
        Register[dest] = src1;
        break;

    default:
        printf("\n[ERROR] Invalid Operation!\n");
        return;
    }
}
