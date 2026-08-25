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
    char *p = strchr(source_line, '%');

    if (p != NULL)
        *p = '\0';
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

    for (int i = 0; i < sizeof(log); i++)
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

    return atoi(reg+1);
}

enum map
{
    ADD = 1,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MEMORY_READ,
    MEMORY_WRITE,
    DATA_MOVEMENT,
    BRANCH
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
                                    {"MEMORY_READ", MEMORY_READ},
                                    {"MEMORY_WRITE", MEMORY_WRITE},
                                    {"DATA_MOVEMENT", DATA_MOVEMENT},
                                    {"BRANCH", BRANCH}};

bool isConstant(char *temp)
{
    if (temp[1] == 'x' || temp[0]=='x')
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
    case ADD:;
        break;

    case SUBTRACT:;
        break;

    case MULTIPLY:;
        break;

    case DIVIDE:

        break;

    case MEMORY_READ:
        fprintf(log, "Compiling Memory Read\n");
        return isConstant(tokens[2]) ? 0x0C : 0x05;
        break;

    case MEMORY_WRITE:
        fprintf(log, "Compiling Memory Write\n");
        return isConstant(tokens[2]) ? 0x0E : 0x06;
        break;

    case DATA_MOVEMENT:;
        break;

    case BRANCH:;
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

    else if (p[0] == 'x')
    {

        return atoi(p + 1);
    }
}

void compile_Memory_Read(FILE *target)
{

    annotate.opcode = getOpcode("MEMORY_READ");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);

    printf("writing into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

void compile_Memory_Write(FILE *target)
{

    annotate.opcode = getOpcode("MEMORY_WRITE");

    annotate.dest = getRegisterNumber(tokens[0]);

    annotate.operand_1 = 0x00;

    annotate.operand_2 = getOperand(tokens[2]);

    printf("writing into program.byte file\n");
    fprintf(target, "%02X %02X %02X %02X\n", annotate.opcode, annotate.dest, annotate.operand_1, annotate.operand_2);
}

void compile_Data_Movement(FILE *target)
{
}

bool secondPass(char *source)
{
    FILE *input_file = fopen(source, "r");
    if (input_file == NULL)
    {
        printf("Error: Could not open source file %s\n", source);
        return false;
    }

    FILE *output_file = fopen("program.byte", "w");
    if (output_file == NULL)
    {
        printf("Error: Could not open source file %s\n", source);
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        removeComments(line);
        trim(line);
        tokenize(line);

        if (token_count == 3)
        {

            if (tokens[2][0] == '[')
                compile_Memory_Read(output_file);

            else if (tokens[0][0] == '[')
                compile_Memory_Write(output_file);

            else
                compile_Data_Movement(output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
    return true;
}

void compiler(char *source)
{

    if (!firstPass(source))
    {
        printf("Compilation failed: Syntax error in source code.\n");
        return;
    }

    if (!secondPass(source))
    {
        printf("Compilation failed: Semantic error in source code.\n");
        return;
    }
}
