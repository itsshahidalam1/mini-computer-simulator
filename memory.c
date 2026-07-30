
#include<stdio.h>


unsigned char instruction_memory[256];
unsigned char data_memory[256];

void initialize(char inst[20], char data[20])
{
   for(int i = 0;i<256;i++){
    instruction_memory[i] = 0;
    data_memory[i]=0;
    

   }

    FILE *inst_File = fopen(inst, "r");
    if (inst_File == NULL)
    {
        printf("error opeing the file %s\n", inst);
        return;
    }

    FILE *data_File = fopen(data, "r");
    if (data_File == NULL)
    {
        printf("error opening the file %s\n", data);
        return;
    }

    int value;
    int i = 0;

    while (i < 256 && fscanf(inst_File, "%d", &value) == 1)
    {
        if (value < 0 || value > 255)
        {
            printf("Invalid instruction byte: %d\n", value);
            break;
        }
        instruction_memory[i++] = (unsigned char)value;
    }
    // instruction_memory[i]=' ';


    int address;
    while (fscanf(data_File, "%d %d", &address, &value) == 2)
    {
        if (address < 0 || address >= 256)
        {
            printf("Invalid address: %d\n", address);
            continue;
        }
        if (value < 0 || value > 255)
        {
            printf("Invalid value %d\n", value);
            continue;
        }

        data_memory[address] = (unsigned char)value;
    }
    

    fclose(data_File);
    fclose(inst_File);
};


void finalize() {
        FILE *data = fopen("data.byte","w");
        int i=0;
        while(i<256){
            fprintf(data,"%d %d\n",i,data_memory[i]);
            i++;
        }
        fclose(data);
};
