#include <stdio.h>
#include "compiler.h"
#include "memory.h"
#include "processor.h"
#include "os.h"

int main(int argc, char *argv[])
{

    start_os(argv[1],argv[2]);
    

    // compiler(argv[1], argv[2]);

    // initialize(argv[2], argv[3]);

    // reset();

    // while (end_of_simulation == 0)
    // {
    //     fetch();
    //     decode();
    //     execute();
    // }
}