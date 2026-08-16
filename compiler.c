#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

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
    fclose(input_file);
    return true;
}

char tokens[10][20];
void tokenize(char *line)
{    
    char temp[strlen(line)];
    strcpy(temp, line);
    int token_count =0;
    char *token = strtok(temp," ,\0\n");
    while(token != NULL)
    {
        strcpy(tokens[token_count++],token);
         token = strtok(NULL," ,\0\n");
    }

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
        parse (tokens);
        
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
