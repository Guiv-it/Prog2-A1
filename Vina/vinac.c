#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

//começar pela movimentação de memória
/* move([u long inicio, ulong final], ulong ref) */

int main(){
  char buffer[10];
  FILE* teste = fopen("arquivo.txt", "a");
  int i=1;
  struct stat* dados;
  fread(buffer, sizeof(char), 10, teste);
  fstat(i, dados);
    printf("\n UID: %d", dados->st_uid);
    printf("  Tamanho Original: %ld", dados->st_size);
    printf("  Tamanho Compactado: %ld", dados->st_size);
    printf("  Data de alteração: %ld \n", dados->st_mtime);
  fclose(teste);
}