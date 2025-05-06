#include <stdio.h>
#include <stdlib.h>

//começar pela movimentação de memória
/* move([u long inicio, ulong final], ulong ref) */

int main(){
  FILE* teste = fopen("arquivo.txt", "a");
  char* string = (" Bom dia"); 
  fputs(string, teste);
  fclose(teste);
}