#include <stdio.h>
#include <string.h>
struct annotations
{
    char instruction[15];
    char destination_Reg[3];
    char operand_1[3];
    char operand_2[3];
};

void produce_byte_code(struct annotations temp)
{
    FILE *f;
    int operation;
    f = fopen("program.byte", "a");
    enum map {Add=1,Subtract,Multiply,Divide,Read,Write,Move};
    if (f == NULL)

    {
        printf("Could not open file.\n");
        return;
    }

    if(strcmp(temp.instruction , "Add")==0) operation = Add;
    else if(strcmp(temp.instruction , "Subtract")==0) operation = Subtract;
    else if(strcmp(temp.instruction , "Multiply")==0) operation = Multiply;
    else if(strcmp(temp.instruction , "Divide")==0) operation = Divide;
    else if(strcmp(temp.instruction , "Read")==0) operation = Read;
    else if(strcmp(temp.instruction , "Write")==0) operation = Write;
    else if(strcmp(temp.instruction,"Move")==0)  operation = Move;
    else 
      {
        printf("Invalid Instruction!!");
        fclose(f);
        return;
      }
      
    if(operation == Read || operation == Write)
     fprintf(f,"%d %c %s %s\n",operation,temp.destination_Reg[1],temp.operand_1,temp.operand_2);
     else if(operation!=Move)
     fprintf(f,"%d %c %c %c\n",operation,temp.destination_Reg[1],temp.operand_1[1],temp.operand_2[1]); 
     else
     fprintf(f,"%d %c %s %s\n",operation,temp.destination_Reg[1],temp.operand_1,temp.operand_2); 
    fclose(f);
    

}

void parser(char tokens[10][20], int token_count)
{
    struct annotations a1;

    if (token_count == 5)
    {
        char operation = tokens[3][0];

        switch (operation)
        {
        case '+':
            strcpy(a1.instruction, "Add");
            strcpy(a1.destination_Reg, tokens[0]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, tokens[4]);

            break;
        case '-':
            strcpy(a1.instruction, "Subtract");
            strcpy(a1.destination_Reg, tokens[0]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, tokens[4]);

            break;
        case '*':
            strcpy(a1.instruction, "Multiply");
            strcpy(a1.destination_Reg, tokens[0]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, tokens[4]);

            break;

        case '/':
            strcpy(a1.instruction, "Divide");
            strcpy(a1.destination_Reg, tokens[0]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, tokens[4]);

            break;

        default:
            break;
        }
    }

    else
    {
        int operation;
    
        if (strcmp(tokens[0],"Read")==0)
            operation = 0;
        else if (strcmp(tokens[0],"Write")==0)
            operation = 1;
        else
            operation = 2;

        switch (operation)
        {
        case 0:
            strcpy(a1.instruction, "Read");
            strcpy(a1.destination_Reg, tokens[1]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, "0");

            break;
        case 1:
            strcpy(a1.instruction, "Write");
            strcpy(a1.destination_Reg, tokens[1]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, "0");

            break;
        case 2:
            strcpy(a1.instruction, "Move");
            strcpy(a1.destination_Reg, tokens[0]);
            strcpy(a1.operand_1, tokens[2]);
            strcpy(a1.operand_2, "0");

            break;

        default:
            break;
        }
    }

    produce_byte_code(a1);
}

void tokenize(char *line)
{
    char tokens[10][20];
    int i = 0, tokenCount = 0, charCount = 0;

    for (; line[i] != '\0'; i++)
    {
        if (line[i] == ' ' || line[i] == '\n')
        {
            tokens[tokenCount][charCount] = '\0';
            tokenCount++;
            charCount = 0;
        }
        else
        {  
            tokens[tokenCount][charCount++] = line[i];
        }
    }
    parser(tokens, tokenCount);
}

void compiler(char source[50])
{
   char line[100];

    FILE *fp;
    fp = fopen(source, "r");

    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    FILE *OUT = fopen("program.byte", "w");
    if (OUT == NULL)
    {
        printf("Error opening the output file!\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        tokenize(line);
    };

    fclose(fp);
    fclose(OUT);


}
