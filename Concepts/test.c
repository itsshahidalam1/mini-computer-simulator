#include <stdio.h>
#include<string.h>

struct annotations
{
    char instruction[15];
    char destination_Reg[3];
    char operand_1[3];
    char operand_2[3];
};

int main(int argc, char **argv)

{  char buff[10];
   
   enum map{add=5};
  snprintf(buff,sizeof(buff),"%d",add);


   printf("%s",buff);
}
