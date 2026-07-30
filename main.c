#include <stdio.h>
#include "compiler.h"
#include "memory.h"

int main(int argc, char *argv[]){

    compiler(argv[1]);
    initialize(argv[2],argv[3]);

}