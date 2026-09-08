#include <stdio.h>
#define NP 4
#include "processor.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
bool N = false;
bool Z = false;
bool C = false;
bool V = false;
int Register[NP][256];
int vectorRegister[NP][32][8];
int PC[NP], opcode, dest, src1, src2;
extern unsigned char instruction_memory[NP][256];
extern unsigned char data_memory[NP][4096];

int end_of_simulation = 0;
void setFlags(int result, int op1, int op2, bool operation)
{
    Z = (result == 0);
    N = (result >> 31) & 1;

    int s1 = (op1 >> 31) & 1;
    int s2 = (op2 >> 31) & 1;
    int sr = (result >> 31) & 1;

    if (operation == 0) // ADD
    {
        C = ((uint32_t)result < (uint32_t)op1);
        V = ((s1 == s2) && (sr != s1));
    }
    else // SUB
    {
        C = ((uint32_t)op1 > (uint32_t)op2);
        V = ((s1 != s2) && (sr == s2));
    }
}
void reset()
{
    /*
     * REGISTER IS 2D:
     * Register[process][register]
     *
     * Reset all NP process register files.
     */
    for (int p = 0; p < NP; p++)
        for (int i = 0; i < 256; i++)
            Register[p][i] = 0;
}

void fetch()
{
    if (instruction_memory[NP][PC] == 0)
    {
        printf("\n****************Program executed successfully***************\n");
        end_of_simulation = 1;
        return;
    }
    opcode = instruction_memory[NP][PC];
    dest = instruction_memory[NP][PC + 1];
    src1 = instruction_memory[NP][PC + 2];
    src2 = instruction_memory[NP][PC + 3];

    PC[NP] = PC[NP] + 4;
}

void decode()
{
    return;
};

void execute(int process)
{

    if (end_of_simulation == 1)
        return;

    switch (opcode)
    {
        /* ================= SCALAR ALU ================= */

    case 0x01: // ADD
        printf("\n[EXECUTE] ADD    R%d = R%d + R%d\n", dest, src1, src2);
        /* 2D register access: Register[process][register_number]. */
        Register[process][dest] = Register[process][src1] + Register[process][src2];
        setFlags(Register[process][dest], Register[process][src1], Register[process][src2], 0);
        break;

    case 0x02: // SUB
        printf("\n[EXECUTE] SUB    R%d = R%d - R%d\n", dest, src1, src2);
        /* 2D register access: all operands belong to the same process. */
        Register[process][dest] = Register[process][src1] - Register[process][src2];
        setFlags(Register[process][dest], Register[process][src1], Register[process][src2], 1);
        printf("R1=%d R4=%d R15=%d Z=%d\n",
       Register[process][src1], Register[process][src2],
       Register[process][dest], Z);
        break;

    case 0x03: // MUL
        printf("\n[EXECUTE] MUL    R%d = R%d * R%d\n", dest, src1, src2);
        Register[process][dest] = Register[process][src1] * Register[process][src2];
        break;

    case 0x04: // DIV
        printf("\n[EXECUTE] DIV    R%d = R%d / R%d\n", dest, src1, src2);
        Register[process][dest] = Register[process][src1] / Register[process][src2];
        break;

        /* ================= MEMORY ================= */

    case 0x05: // MEMORY READ
        printf("\n[EXECUTE] LOAD   R%d = MEM[%d]\n", dest, Register[process][src2]);
        {
            int j = Register[process][src2];
            int value =
                ((int)data_memory[j] << 24) | ((int)data_memory[j + 1] << 16) | ((int)data_memory[j + 2] << 8) | (int)data_memory[j + 3];
            Register[process][dest] = value;
        }
        break;

    case 0x06: // MEMORY WRITE
        printf("\n[EXECUTE] STORE  MEM[%d] = R%d\n", dest, src2);
        {
            int value = Register[process][src2];
            data_memory[Register[process][dest]] = (unsigned char)(value >> 24) & 0xFF;
            data_memory[Register[process][dest] + 1] = (unsigned char)(value >> 16) & 0xFF;
            data_memory[Register[process][dest] + 2] = (unsigned char)(value >> 8) & 0xFF;
            data_memory[Register[process][dest] + 3] = (unsigned char)value & 0xFF;
        }
        break;

    case 0x07: // DATA MOVEMENT
        printf("\n[EXECUTE] MOV    R%d = R%d\n", dest, src2);
        Register[process][dest] = Register[process][src2];
        break;

        /* ================= SCALAR CONSTANT ================= */

    case 0x09: // ADD CONSTANT
        printf("\n[EXECUTE] ADDI   R%d = R%d + %d\n",
               dest, src1, src2);
        Register[process][dest] = Register[process][src1] + src2;
        setFlags(Register[process][dest], Register[process][src1], src2, 0);
        break;

    case 0x0A: // SUB CONSTANT
        printf("\n[EXECUTE] SUBI   R%d = R%d - %d\n",
               dest, src1, src2);
        Register[process][dest] = Register[process][src1] - src2;
        setFlags(Register[process][dest], Register[process][src1], src2, 1);
        break;

    case 0x0B: // MUL CONSTANT
        printf("\n[EXECUTE] MULI   R%d = R%d * %d\n",
               dest, src1, src2);
        Register[process][dest] = Register[process][src1] * src2;
        break;

    case 0x0C: // DIV CONSTANT
        printf("\n[EXECUTE] DIVI   R%d = R%d / %d\n",
               dest, src1, src2);
        Register[process][dest] = Register[process][src1] / src2;
        break;

    case 0x0D: // MEMORY READ CONSTANT ADDRESS
        printf("\n[EXECUTE] LOAD   R%d = MEM[%d]\n", dest, src2);
        {
            int j = src2;
            int value =
                ((int)data_memory[j] << 24) | ((int)data_memory[j + 1] << 16) | ((int)data_memory[j + 2] << 8) | (int)data_memory[j + 3];
            Register[process][dest] = value;
        }

        ;
        break;

    case 0x0E: // MEMORY WRITE CONSTANT
        printf("\n[EXECUTE] STORE  MEM[%d] = %d\n", Register[process][dest], src2);
        {
            int value = src2;
            data_memory[Register[process][dest]] = (unsigned char)(value >> 24) & 0xFF;
            data_memory[Register[process][dest] + 1] = (unsigned char)(value >> 16) & 0xFF;
            data_memory[Register[process][dest] + 2] = (unsigned char)(value >> 8) & 0xFF;
            data_memory[Register[process][dest] + 3] = (unsigned char)value & 0xFF;
        }
        break;

    case 0x0F: // DATA MOVEMENT CONSTANT
        printf("\n[EXECUTE] MOVI   R%d = %d\n", dest, src2);
        Register[process][dest] = src2;
        break;

        /* ================= BRANCH ================= */

    case 0x10: // EQ
    {
        int offset = (int8_t)src2;

        if (Z)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }
    case 0x11: // NE
    {
        int offset = (int8_t)src2;

        if (!Z)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x12: // CS
    {
        int offset = (int8_t)src2;

        if (C)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x13: // CC
    {
        int offset = (int8_t)src2;

        if (!C)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x14: // MI
    {
        int offset = (int8_t)src2;

        if (N)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x15: // PL
    {
        int offset = (int8_t)src2;

        if (!N)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x16: // VS
    {
        int offset = (int8_t)src2;

        if (V)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x17: // VC
    {
        int offset = (int8_t)src2;

        if (!V)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x18: // HI
    {
        int offset = (int8_t)src2;

        if (C && !Z)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x19: // LS
    {
        int offset = (int8_t)src2;

        if (!C || Z)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x1A: // GE
    {
        int offset = (int8_t)src2;

        if (N == V)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x1B: // LT
    {
        int offset = (int8_t)src2;

        if (N != V)
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x1C: // GT
    {
        int offset = (int8_t)src2;

        if (!Z && (N == V))
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x1D: // LE
    {
        int offset = (int8_t)src2;

        if (Z || (N != V))
            PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

    case 0x1E: // AL
    {
        int offset = (int8_t)src2;

        PC[process] = PC[process] + 4 * (offset - 1);

        break;
    }

        /* ================= VECTOR ALU ================= */

    case 0x21: // V_ADD
        printf("\n[EXECUTE] VADD   V%d = V%d + V%d\n",
               dest, src1, src2);
        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] + vectorRegister[process][src2][i];
        }
        break;

    case 0x22: // V_SUB
        printf("\n[EXECUTE] VSUB   V%d = V%d - V%d\n",
               dest, src1, src2);
        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] - vectorRegister[process][src2][i];
        }
        break;

    case 0x23: // V_MUL
        printf("\n[EXECUTE] VMUL   V%d = V%d * V%d\n",
               dest, src1, src2);
        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] * vectorRegister[process][src2][i];
        }
        break;

        // /* ================= VECTOR MEMORY ================= */

    case 0x25: // VECTOR READ
        printf("\n[EXECUTE] VLOAD  V%d = VMEM[%d]\n",
               dest, src2);
        int j = Register[process][src2];
        for (int i = 0; i < 8; i++)
        {
            int value =
                ((int)data_memory[j] << 24) | ((int)data_memory[j + 1] << 16) | ((int)data_memory[j + 2] << 8) | (int)data_memory[j + 3];
            vectorRegister[process][dest][i] = value;
            j += 4;
        }

        for (int i = 0; i < 8; i++)
            printf("I : %d \n", vectorRegister[process][dest][i]);
        break;

    case 0x26: // VECTOR WRITE
        printf("\n[EXECUTE] VSTORE VMEM[%d] = V%d\n",
               Register[process][dest], src2);

        j = Register[process][dest];

        for (int i = 0; i < 8; i++)
        {
            int value = vectorRegister[process][src2][i];
            data_memory[j] = (unsigned char)(value >> 24) & 0xFF;
            data_memory[j + 1] = (unsigned char)(value >> 16) & 0xFF;
            data_memory[j + 2] = (unsigned char)(value >> 8) & 0xFF;
            data_memory[j + 3] = (unsigned char)value & 0xFF;

            j += 4;
        }

        break;

        // /* ================= VECTOR CONSTANT ================= */

    case 0x29: // V_ADD CONSTANT
        printf("\n[EXECUTE] VADD_C   V%d = V%d + %d\n",
               dest, src1, src2);
        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] + src2;
        }
        break;

    case 0x2A: // V_SUB CONSTANT
        printf("\n[EXECUTE] VSUBI  V%d = V%d - %d\n",
               dest, src1, src2);
        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] - src2;
        }
        break;

    case 0x2B: // V_MUL CONSTANT
        printf("\n[EXECUTE] VMULI  V%d = V%d * %d\n",
               dest, src1, src2);

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] * src2;
        }

        break;

    case 0x2C: // VECTOR READ CONSTANT
        printf("\n[EXECUTE] VLOAD  V%d = VMEM[%d]\n",
               dest, src2);

        j = src2;
        for (int i = 0; i < 8; i++)
        {
            int value =
                ((int)data_memory[j] << 24) | ((int)data_memory[j + 1] << 16) | ((int)data_memory[j + 2] << 8) | (int)data_memory[j + 3];
            vectorRegister[process][dest][i] = value;
            j += 4;
        }

        for (int i = 0; i < 8; i++)
            printf("I : %d \n", vectorRegister[process][dest][i]);

        break;

    case 0x2E: // VECTOR WRITE CONSTANT
        printf("\n[EXECUTE] VSTORE VMEM[%d] = %d\n",
               Register[process][dest], src2);
        j = Register[process][dest];

        for (int i = 0; i < 8; i++)
        {
            int value = src2;
            data_memory[j] = (value >> 24) & 0xFF;
            data_memory[j + 1] = (value >> 16) & 0xFF;
            data_memory[j + 2] = (value >> 8) & 0xFF;
            data_memory[j + 3] = value & 0xFF;

            j += 4;
        }

        break;

        /* ================= INVALID ================= */

    default:
        printf("\n[ERROR] Invalid Operation! Opcode = 0x%02X\n", opcode);
        return;
    }
}
