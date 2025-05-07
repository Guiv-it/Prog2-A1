#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "vina.h"
#include "lz.h"
#include "lista.h"

//começar pela movimentação de memória


int main(){
  int a  =sizeof(struct diretorio);
  printf("tamanho diretório: %d\n", a);
  a  =sizeof(struct arquivo);
  printf("tamanho arquivo: %d\n", a);
  a  =sizeof(struct membro);
  printf("tamanho membro: %d\n", a);
  a  =sizeof(struct item);
  printf("tamanho item: %d\n", a);
  a  =sizeof(struct lista_t);
  printf("tamanho lista_t: %d\n", a);
}