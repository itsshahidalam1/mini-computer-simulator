#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

struct Label
{
    char labelName[50];
    int labelAddress;
};

struct Label labels[256];

void removeComments(char *source_line)
{
    char *z = strchr(source_line, '%');

    if (z != NULL)
        *z = '\0';
}

void trim(char *source_line)
{
    int i = 0;
    int j = strlen(source_line) - 1;
    while (isspace(source_line[i]))
        i++;
    while ((isspace(source_line[j])))
        j--;
    source_line[j + 1] = '\0';
    char trimmed[j - i + 2];
    strcpy(trimmed, source_line + i);
    strcpy(source_line, trimmed);
}

bool islabel(char *line)
{
    if (line[0] != '.')
        return false;
    return true;
}

bool firstPass(char source[50])
{

    FILE *input_file = fopen(source, "r");
    if (input_file == NULL)
    {
        printf("Error: Could not open source file %s\n", source);
        return false;
    }

    char line[256];
    int instAdd = 0;
    int labelcount = 0;
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        removeComments(line);
        trim(line);
        if (islabel(line))
        {
            strcpy(labels[labelcount].labelName, line + 1);
            labels[labelcount].labelAddress = instAdd;
            labelcount++;
        }
        else

            instAdd++;
    }

    // ======== Printing label table to log file ================ //
    FILE *log = fopen("log.txt", "w");

    for (int i = 0; i < labelcount; i++)
        fprintf(log, "label %s and labelAdd %d\n", labels[i].labelName, labels[i].labelAddress);
    fclose(log);
    fclose(input_file);
    return true;
}

char tokens[10][20]; // Global array tokens
int token_count;
void tokenize(char *line)
{
    char temp[strlen(line)];
    strcpy(temp, line);
    token_count = 0;
    char *token = strtok(temp, " ,\0\n");
    while (token != NULL)
    {
        strcpy(tokens[token_count++], token);
        token = strtok(NULL, " ,\0\n");
    }

    // ======== Printing tokens to log file ================ //
    FILE *log = fopen("log.txt", "a");
    for (int i = 0; i < token_count; i++)
        fprintf(log, "%s\n", tokens[i]);
    fprintf(log, "token count: %d\n", token_count);
    fclose(log);
}

// ================= Second Pass ======================= //
struct byteCodeAnnotations
{
    int opcode;
    int dest;
    int operand_1;
    int operand_2;
} annotate;

int getRegisterNumber(char *reg)
{
    if (reg[0] == '[')
    {
        char buff[5];
        strcpy(buff, reg + 2);
        for (int i = 0; i < sizeof(buff); i++)
            if (buff[i] == ']')
            {
                buff[i] = '\0';
                break;
            }
        return atoi(buff);
    }

    return atoi(reg + 1);
}

enum map
{
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    V_ADD,
    V_SUBTRACT,
    V_MULTIPLY,
    MEMORY_READ,
    MEMORY_WRITE,
    DATA_MOVEMENT,
    BRANCH,
    V_READ,
    V_WRITE
};

struct operationMap
{
    const char *name;
    enum map op;
};

struct operationMap operations[] = {{"ADD", ADD},
                                    {"SUBTRACT", SUBTRACT},
                                    {"MULTIPLY", MULTIPLY},
                                    {"DIVIDE", DIVIDE},
                                    {"VECTOR_ADD", V_ADD},
                                    {"VECTOR_SUBTRACT", V_SUBTRACT},
                                    {"VECTOR_MULTIPLY", V_MULTIPLY},
                                    {"MEMORY_READ", MEMORY_READ},
                                    {"MEMORY_WRITE", MEMORY_WRITE},
                                    {"DATA_MOVEMENT", DATA_MOVEMENT},
                                    {"BRANCH", BRANCH},
                                    {"VECTOR_READ", V_READ},
                                    {"VECTOR_WRITE", V_WRITE}};

bool isConstant(char *temp)
{
    if (temp[1] == 'x' || temp[0] == 'x')
        return false;
    return true;
}

struct branchSuffix
{
    const char *Suffix;
    char *code;
};

struct branchSuffix bMap[] = {
    {"EQ", "0000"},
    {"NE", "0001"},
    {"CS", "0010"},
    {"CC", "0011"},
    {"MI", "0100"},
    {"PL", "0101"},
    {"VS", "0110"},
    {"VC", "0111"},
    {"HI", "1000"},
    {"LS", "1001"},
    {"GE", "1010"},
    {"LT", "1011"},
    {"GT", "1100"},
    {"LE", "1101"},
    {"AL", "1110"}};

int binaryToInt(char *binary)
{
    int value = 0;

    for (int i = 0; binary[i] != '\0'; i++)
        value = value * 2 + (binary[i] - '0');

    return value;
}
int getBranchCode(char *t)
{
    int index = -1;
    for (int i = 0; i < sizeof(bMap) / sizeof(bMap[0]); i++)
    {

        if (strcmp(bMap[i].Suffix, t) == 0)
        {
            index = i;
            break;
        }
    }

    return 0x10 + binaryToInt(bMap[index].code);
}

bool isVConstant(char v[10])
{
    if (v[0] == 'v')
        return false;
    return true;
}

int getOpcode(char *Operatn)
{
    int exp = -1;
    for (int i = 0; i < sizeof(operations) / sizeof(operations[0]); i++)
    {
        if (strcmp(operations[i].name, Operatn) == 0)
        {
            exp = operations[i].op;
            break;
        }
    }

    FILE *log = fopen("log.txt", "a");

    switch (exp)
    {
    case ADD:
        fprintf(log, "Compiling Addition\n");
        return isConstant(tokens[4]) ? 0x09 : 0x01;
        break;

    case SUBTRACT:
        fprintf(log, "Compiling Subtraction\n");
        return isConstant(tokens[4]) ? 0x0A : 0x02;
        break;
    case MULTIPLY:
        fprintf(log, "Compiling multiplication\n");
        return isConstant(tokens[4]) ? 0x0B : 0x03;
        break;
    
    case V_ADD:
        fprintf(log, "Compiling Addition\n");
        return isVConstant(tokens[4]) ? 0x29 : 0x21;
        break;

    case V_SUBTRACT:
        fprintf(log, "Compiling Subtraction\n");
        return isVConstant(tokens[4]) ? 0x2A : 0x22;
        break;
    case V_MULTIPLY:
        fprintf(log, "Compiling multiplication\n");
        return isVConstant(tokens[4]) ? 0x2B : 0x23;
        break;

    case DIVIDE:
        fprintf(log, "Compiling Division\n");
        return isConstant(tokens[4]) ? 0x0C : 0x04;
        break;

    case MEMORY_READ:
        fprintf(log, "Compiling Memory Read\n");
        return isConstant(tokens[2]) ? 0x0D : 0x05;
        break;

    case MEMORY_WRITE:
        fprintf(log, "Compiling Memory Write\n");
        return isConstant(tokens[2]) ? 0x0E : 0x06;
        break;

    case DATA_MOVEMENT:
        fprintf(log, "Compiling Data movement\n");
        return isConstant(tokens[2]) ? 0x0F : 0x07;
        break;

    case BRANCH:
        fprintf(log, "Compiling Branch Instruction\n");
        return getBranchCode(tokens[0] + 1);
        break;

    case V_READ:
        fprintf(log, "Compiling Vector Read Instruction\n");
        return isConstant(tokens[2]) ? 0x2C : 0x25;
        break;
    case V_WRITE:
        fprintf(log, "Compiling Vector Write Instruction\n");
        return isVConstant(tokens[2]) ? 0x2E : 0x26;
        break;
    default:
        printf("Invalid operation\n");
        break;
    }

    fclose(log);
}

int getOperand(char *p)
{

    if (p[0] == '[')
    {
        if (isConstant(p))
        {
            char buff[5];
            strcpy(buff, p + 1);
            for (int i = 0; i < sizeof(buff); i++)
                if (buff[i] == ']')
                {
                    buff[i] = '\0';
                    break;
                }
            return atoi(buff);
        }
        else
        {
            char buff[5];
            strcpy(buff, p + 2);
            for (int i = 0; i < sizeof(buff); i++)
                if (buff[i] == ']')
                {
                    buff[i] = '\0';
                    break;
                }
            return atoi(buff);
        }
    }

    else if (p[0] == 'x' || p[0] == 'v' || p[0] == 'i')
    {

        return atoi(p + 1);
    }

    else 
        return atoi(p);
}

int getOffset(char p[10], int inADD)
{

    for (int i = 0; i < sizeof(labels) / sizeof(labels[0]); i++)
    {
        if (strcmp(p, labels[i].labelName) == 0)
        {
            printf("inADD: %d labelADD: %d offset: %d\n", inADD, labels[i].labelAddress, labels[i].labelAddress - inADD + 1);
            return (labels[i].labelAddress - inADD + 1);
        }
    }
    return -1;
};

// token_count = 3

void compile_Memory_Read(FILE *target)
{

    annotate.opcode = getOpcode("MEMORY_READ");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);

    printf("writing MEM R into program.byte file\n");

    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

void compile_Memory_Write(FILE *target)
{

    annotate.opcode = getOpcode("MEMORY_WRITE");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);

    printf("writing MEMEORY W into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

void compile_Data_Movement(FILE *target)
{
    annotate.opcode = getOpcode("DATA_MOVEMENT");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);

    printf("writing Data Movement into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

// token_count = 2
void compile_Branch_Instruction(FILE *target, int inADD)
{

    annotate.opcode = getOpcode("BRANCH");

    annotate.dest = 0x00;

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOffset(tokens[1] + 1, inADD);

    printf("writing Branch Instruction into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2 & 0xFF);
}

// token_count = 5
struct arithmaticOP
{
    char *symbol;
    int op;
};
struct arithmaticOP arithmaticMap[] =
    {
        {"+", 1},
        {"-", 2},
        {"*", 3},
        {"/", 4},
};

void compile_Arithmetic_Instruction(FILE *target)
{
    int op;
    for (int i = 0; i < sizeof(arithmaticMap) / sizeof(arithmaticMap[0]); i++)
    {

        if (strcmp(arithmaticMap[i].symbol, tokens[3]) == 0)
        {
            op = arithmaticMap[i].op;
            break;
        }
    }

    switch (op)
    {

    case 1:
        annotate.opcode = getOpcode("ADD");
        break;

    case 2:
        annotate.opcode = getOpcode("SUBTRACT");
        break;
    case 3:
        annotate.opcode = getOpcode("MULTIPLY");
        break;
    case 4:
        annotate.opcode = getOpcode("DIVIDE");
        break;
    }

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = getOperand(tokens[2]);

    annotate.operand_2 = getOperand(tokens[4]);

    printf("writing Arithmetic Instruction into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

// Vector operations:
void compile_Vector_Read(FILE *target)
{
    annotate.opcode = getOpcode("VECTOR_READ");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);
    printf("writing vector read into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

void compile_Vector_Write(FILE *target)
{

    annotate.opcode = getOpcode("VECTOR_WRITE");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);
    printf("writing vector write into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}
// Vector Arithmetic operations
void compile_Vector_Arithmetic_Instruction(FILE *target)
{
    int op;
    for (int i = 0; i < sizeof(arithmaticMap) / sizeof(arithmaticMap[0]); i++)
    {

        if (strcmp(arithmaticMap[i].symbol, tokens[3]) == 0)
        {
            op = arithmaticMap[i].op;
            break;
        }
    }

    switch (op)
    {

    case 1:
        annotate.opcode = getOpcode("VECTOR_ADD");
        break;

    case 2:
        annotate.opcode = getOpcode("VECTOR_SUBTRACT");
        break;
    case 3:
        annotate.opcode = getOpcode("VECTOR_MULTIPLY");
        break;
    case 4:
        printf("Divide Operation is not supported for vectors \n");
        break;
    }

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = getOperand(tokens[2]);

    annotate.operand_2 = getOperand(tokens[4]);

    printf("writing Arithmetic Instruction into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

bool secondPass(char *source,char *target)
{
    FILE *input_file = fopen(source, "r");
    if (input_file == NULL)
    {
        printf("Error: Could not open source file %s\n", source);
        return false;
    }

    FILE *output_file = fopen(target, "w");
    if (output_file == NULL)
    {
        printf("Error: Could not open source file %s\n", source);
        return false;
    }

    int inADD = 0;
    char line[256];
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        removeComments(line);
        trim(line);
        tokenize(line);

        if (tokens[0][0] != '.')
            inADD++;

        if (token_count == 3) // I still have to write legacy read and write here only.
        {

            if (tokens[2][0] == '[')

            {
                if (tokens[0][0] == 'x')
                    compile_Memory_Read(output_file);
                else if (tokens[0][0] == 'v')
                    compile_Vector_Read(output_file);
                else
                    return false;
            }

            else if (tokens[0][0] == '[')
            {
                if (tokens[2][0] == 'v' || tokens[2][0] == 'i')
                    compile_Vector_Write(output_file);
                else
                    compile_Memory_Write(output_file);
            }

            else
                compile_Data_Movement(output_file);
        }

        else if (token_count == 2)
        { // Branch insturctions

            compile_Branch_Instruction(output_file, inADD);
        }

        else if (token_count == 5)
        { // Arithmetic instructions
            if (tokens[0][0] == 'x')
                compile_Arithmetic_Instruction(output_file);
            else if (tokens[0][0] == 'v')
                compile_Vector_Arithmetic_Instruction(output_file);
            else
                return false;
        }
    }

    fclose(input_file);
    fclose(output_file);
    return true;
}

void compiler(char *source,char *target)
{

    if (!firstPass(source))
    {
        printf("Compilation failed: Syntax error in source code.\n");
        return;
    }

    if (!secondPass(source,target))
    {
        printf("Compilation failed: Semantic error in source code.\n");
        return;
    }
}
