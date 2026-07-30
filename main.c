#include <stdio.h>
#include "compiler.h"
#include "memory.h"
#include "processor.h"

int main(int argc, char *argv[]){

    compiler(argv[1]);

    initialize(argv[2],argv[3]);

    reset();

    while(end_of_simulation==0)
    {   
        fetch();
        decode();
        execute();
    }


}