#include <stdio.h>
int main(){
    FILE *file = fopen("test.txt", "r");
   char c = fgetc(file);
   printf("%c: %d %d %c\n", c , c, sizeof(c), -1 );
  
   fclose(file);
}