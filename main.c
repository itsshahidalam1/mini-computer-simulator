#include <stdio.h>

#include "compiler.h"
#include "memory.h"
// #include "processor.h"

int main(int argc, char *argv[]){

    compiler(argv[1],argv[2]);

    initialize(argv[2],argv[3]);

   FILE* fd = fopen("log.txt","a");
   
   fprintf(fd,"\n===================Memory Portion starts here ========================\n");
   fprintf(fd,"instrcution memroy contents: \n");
   for(int i=0;i<256;i=i+4){
    fprintf(fd,"%02X %02X %02X %02X\n",instruction_memory[i],instruction_memory[i+1],instruction_memory[i+2],instruction_memory[i+3]);
   }

    fprintf(fd,"data memroy contents: \n");
   for(int i=0;i<4096;i=i+4){
    fprintf(fd,"%02X %02X %02X %02X\n",data_memory[i],data_memory[i+1],data_memory[i+2],data_memory[i+3]);
   }
   fclose(fd);   
    // reset();

    // while(end_of_simulation==0)
    // {   
    //     fetch();
    //     decode();
    //     execute();
    // }
    
    // finalize();


}