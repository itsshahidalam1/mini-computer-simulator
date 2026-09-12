#include <stdio.h>
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

int end_of_simulation[NP];

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

    for (int i = 0; i < NP; i++)
    {
        end_of_simulation[i] = 0;
        PC[i] = 0;
    }
}

void resetIND(int process)
{
    for (int i = 0; i < 256; i++)
    {
        Register[process][i] = 0;
    }
    for (int i = 0; i < 4096; i++)
    {
        data_memory[process][i] = 0;
    }
    for (int i = 0; i < 256; i++)
    {
        instruction_memory[process][i] = 0;
    }

    for (int i = 0; i < 32; i++)
        for (int j = 0; j < 8; j++)
        {
            vectorRegister[process][i][j];
        }
    PC[process] = 0;
    end_of_simulation[process] = 0;
}
void fetch(int process)
{
    if (instruction_memory[process][PC[process]] == 0)
    {
        end_of_simulation[process] = 1;
        return;
    }
    opcode = instruction_memory[process][PC[process]];
    dest = instruction_memory[process][PC[process] + 1];
    src1 = instruction_memory[process][PC[process] + 2];
    src2 = instruction_memory[process][PC[process] + 3];

    PC[process] = PC[process] + 4;
}

void decode()
{
    return;
};

void execute(int process)
{
    if (end_of_simulation[process] == 1)
        return;

    switch (opcode)
    {
        /* ================= SCALAR ALU ================= */

    case 0x01: // ADD

        /* 2D register access: Register[process][register_number]. */
        Register[process][dest] = Register[process][src1] + Register[process][src2];
        setFlags(Register[process][dest], Register[process][src1], Register[process][src2], 0);
        break;

    case 0x02: // SUB

        /* 2D register access: all operands belong to the same process. */
        Register[process][dest] = Register[process][src1] - Register[process][src2];
        setFlags(Register[process][dest], Register[process][src1], Register[process][src2], 1);
        printf("R1=%d R4=%d R15=%d Z=%d\n",
               Register[process][src1], Register[process][src2],
               Register[process][dest], Z);
        break;

    case 0x03: // MUL

        Register[process][dest] = Register[process][src1] * Register[process][src2];
        break;

    case 0x04: // DIV

        Register[process][dest] = Register[process][src1] / Register[process][src2];
        break;

        /* ================= MEMORY ================= */

    case 0x05: // MEMORY READ

    {
        int j = Register[process][src2];
        int value =
            ((int)data_memory[process][j] << 24) | ((int)data_memory[process][j + 1] << 16) | ((int)data_memory[process][j + 2] << 8) | (int)data_memory[process][j + 3];
        Register[process][dest] = value;
    }
    break;

    case 0x06: // MEMORY WRITE

    {
        int value = Register[process][src2];
        data_memory[process][Register[process][dest]] = (unsigned char)(value >> 24) & 0xFF;
        data_memory[process][Register[process][dest] + 1] = (unsigned char)(value >> 16) & 0xFF;
        data_memory[process][Register[process][dest] + 2] = (unsigned char)(value >> 8) & 0xFF;
        data_memory[process][Register[process][dest] + 3] = (unsigned char)value & 0xFF;
    }
    break;

    case 0x07: // DATA MOVEMENT[process]

        Register[process][dest] = Register[process][src2];
        break;

        /* ================= SCALAR CONSTANT ================= */

    case 0x09: // ADD CONSTANT

        Register[process][dest] = Register[process][src1] + src2;
        setFlags(Register[process][dest], Register[process][src1], src2, 0);
        break;

    case 0x0A: // SUB CONSTANT

        Register[process][dest] = Register[process][src1] - src2;
        setFlags(Register[process][dest], Register[process][src1], src2, 1);
        break;

    case 0x0B: // MUL CONSTANT

        Register[process][dest] = Register[process][src1] * src2;
        break;

    case 0x0C: // DIV CONSTANT

        Register[process][dest] = Register[process][src1] / src2;
        break;

    case 0x0D: // MEMORY READ CONSTANT ADDRESS

    {
        int j = src2;
        int value =
            ((int)data_memory[process][j] << 24) | ((int)data_memory[process][j + 1] << 16) | ((int)data_memory[process][j + 2] << 8) | (int)data_memory[process][j + 3];
        Register[process][dest] = value;
    }

    ;
    break;

    case 0x0E: // MEMORY WRITE CONSTANT

    {
        int value = src2;
        data_memory[process][Register[process][dest]] = (unsigned char)(value >> 24) & 0xFF;
        data_memory[process][Register[process][dest] + 1] = (unsigned char)(value >> 16) & 0xFF;
        data_memory[process][Register[process][dest] + 2] = (unsigned char)(value >> 8) & 0xFF;
        data_memory[process][Register[process][dest] + 3] = (unsigned char)value & 0xFF;
    }
    break;

    case 0x0F: // DATA MOVEMENT CONSTANT
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

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] + vectorRegister[process][src2][i];
        }
        break;

    case 0x22: // V_SUB

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] - vectorRegister[process][src2][i];
        }
        break;

    case 0x23: // V_MUL

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] * vectorRegister[process][src2][i];
        }
        break;

        // /* ================= VECTOR MEMORY ================= */

    case 0x25: // VECTOR READ

    {
        int j = Register[process][src2];
        for (int i = 0; i < 8; i++)
        {
            int value =
                ((int)data_memory[process][j] << 24) | ((int)data_memory[process][j + 1] << 16) | ((int)data_memory[process][j + 2] << 8) | (int)data_memory[process][j + 3];
            vectorRegister[process][dest][i] = value;
            j += 4;
        }

       
    }
    break;

    case 0x26: // VECTOR WRITE

    {
        int j = Register[process][dest];

        for (int i = 0; i < 8; i++)
        {
            int value = vectorRegister[process][src2][i];
            data_memory[process][j] = (unsigned char)(value >> 24) & 0xFF;
            data_memory[process][j + 1] = (unsigned char)(value >> 16) & 0xFF;
            data_memory[process][j + 2] = (unsigned char)(value >> 8) & 0xFF;
            data_memory[process][j + 3] = (unsigned char)value & 0xFF;

            j += 4;
        }
    }
    break;

        // /* ================= VECTOR CONSTANT ================= */

    case 0x29: // V_ADD CONSTANT

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] + src2;
        }
        break;

    case 0x2A: // V_SUB CONSTANT

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] - src2;
        }
        break;

    case 0x2B: // V_MUL CONSTANT

        for (int i = 0; i < 8; i++)
        {
            vectorRegister[process][dest][i] = vectorRegister[process][src1][i] * src2;
        }

        break;

    case 0x2C: // VECTOR READ CONSTANT
    {
        int j = src2;
        for (int i = 0; i < 8; i++)
        {
            int value =
                ((int)data_memory[process][j] << 24) | ((int)data_memory[process][j + 1] << 16) | ((int)data_memory[process][j + 2] << 8) | (int)data_memory[process][j + 3];
            vectorRegister[process][dest][i] = value;
            j += 4;
        }
    }
    break;

    case 0x2E: // VECTOR WRITE CONSTANT
    {
        int j = Register[process][dest];

        for (int i = 0; i < 8; i++)
        {
            int value = src2;
            data_memory[process][j] = (value >> 24) & 0xFF;
            data_memory[process][j + 1] = (value >> 16) & 0xFF;
            data_memory[process][j + 2] = (value >> 8) & 0xFF;
            data_memory[process][j + 3] = value & 0xFF;

            j += 4;
        }
    }

    break;

        /* ================= INVALID ================= */

    default:
        printf("\n[ERROR] Invalid Operation! Opcode = 0x%02X\n", opcode);
        return;
    }
}
