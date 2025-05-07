#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz.h"
#include "vina.h"

struct diretorio* criadir(){
  struct diretorio* archi = malloc(sizeof(struct diretorio));
  FILE* algo;

  archi->dir = algo;
  archi->membros = lista_cria();
  archi->tamdir=sizeof(struct diretorio);

  return archi;
}

//move o grupo de blocos que vai do "inicio" até o "final" para que seu início fique na posição ref
void move_bloco(FILE* file, long inicio, long final, long ref){

  long tam = (final - inicio);
  char *buffer = (char *)malloc(tam);

  fseek(file, inicio, SEEK_SET);
  fread(buffer, tam, 1, file);
  fseek(file, ref, SEEK_SET);
  fwrite(buffer, tam, 1, file);

  //atualizar a posição_ini dentro da lista (arquivo->dir->membros)
  return;
}

/*-p : insere/acrescenta um ou mais membros sem compressão ao archive. Caso
o membro já exista no archive, ele deve ser substituído. Novos membros são
inseridos respeitando a ordem da linha de comando, ao final do archive;*/
void insere(struct arquivo* arquivo, FILE* N_membro){

  
  long dir_increase_tam ;
  long tam_N_membro = ftell(N_membro);
  char* buffer = (char *)malloc(tam_N_membro);
  FILE* file = arquivo->membro ;
  
  //carrega o buffer com o membro a ser inserido
  fread(buffer, tam_N_membro, 1, N_membro);

  //encontra o final do arquivo após aumentar o diretório
  fseek(file, dir_increase_tam, SEEK_END);

  //insere o novo membro (tamanho que o diretório aumenta) bytes após o final do arquivo 
  fwrite(buffer, tam_N_membro, 1, file);

  //move os outros arquivos (tamanho que o diretório aumenta) bytes para a frente;
  for (int i = (arquivo->dir->Numero_membros); i < 0 ; i--){

    struct item* aux;

    aux = lista_busca(arquivo->dir->membros, i);

    long f_ini = aux->arquivo->posi_ini;
    long f_fim = (f_ini + (aux->arquivo->tam_atual));
    long f_dest = (f_ini + dir_increase_tam);
    
    move_bloco(file, f_ini, f_fim, f_dest);

  }

  //aumenta o tamanho do diretório

}

void inserecomp(){
}

void move(){

}

void unpack(){

}

void remover(){

}

void list(){

}
