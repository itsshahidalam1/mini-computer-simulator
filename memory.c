#include <stdio.h>
unsigned char instruction[256];
unsigned char data[256];

void initialize(char inst[20], char data[20]){
  
  FILE *inst_File = fopen(inst,"r");
    if(inst_File==NULL){
        printf("error opeing the file %s\n",inst);
        return ;
    }

     FILE *data_File = fopen(data,"r");
    if(data_File
        ==NULL){
        printf("error opeing the file %s\n",data);
        return;
    }

    int value;
    int i = 0;

    while (i < 256 && fscanf(inst_File, "%d", &value) == 1) {
        if (value < 0 || value > 255) {
            printf("Invalid instruction byte: %d\n", value);
            break;
        }
        instruction[i++] = (unsigned char)value;
    }
    
   




};
void finalize(){





};

void memory(char data_file[20]){






}

int main(int argc, char **argv)
{
  
   

    initialize(argv[1], argv[2]);
  



}