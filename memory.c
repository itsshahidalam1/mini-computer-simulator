#include <stdio.h>
char instruction[64][4];
char data[256];

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


   
  char inst_Buff[20];
  char data_Buff[20];
  int i = 0;
  struct help {
    int op;
    int dest;
    int operand1;
    int operand2;

  }h;

   int p=0,q=0;

  while(fgets(inst_Buff,sizeof(inst_Buff),inst_File)!=NULL)
   {
       for(int i=0;inst_Buff[i]!='\0';i++){
          in

       }


   }
    printf("%c",instruction[14]);







};
void finalize(){





};

void memory(char data_file[20]){






}

int main(int argc, char **argv)
{
  
   

    initialize(argv[1], argv[2]);
  



}