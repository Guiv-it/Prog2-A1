#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

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

struct membro* criamembro(FILE* file){
    /*nome, UID, tamanho original, tamanho em disco, posição na lista e data de modificação*/
    struct membro* Novo_membro = malloc(sizeof(struct membro));
    struct stat* buffer;
    int fd = fileno(file);

    fstat(fd, buffer);

    //Novo_membro->nome = "1";
    Novo_membro->UID = buffer->st_uid;
    Novo_membro->tam_ini = buffer->st_size;
    Novo_membro->tam_atual = buffer->st_size;
    Novo_membro->mod_data = buffer->st_mtime;
    Novo_membro->ordem = 0;
    Novo_membro->posi_ini = 0;

    return Novo_membro;
}

struct diretorio* dirfetch(struct arquivo* archi){
  if(!archi)
    return NULL;
  return archi->dir;
}

struct lista_t* lista_membros(struct diretorio* dir){
  if(!dir)
    return NULL;
  return dir->membros;
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

  long aumento = sizeof(struct membro);
  struct diretorio* dir = dirfetch(arquivo);
  struct lista_t* membros = lista_membros(arquivo->dir);
  long tam_new_membro = ftell(N_membro);
  char* buffer = (char *)malloc(tam_new_membro);
  FILE* file = arquivo->membro;
  struct membro* novo = criamembro(N_membro);
  
  //carrega o buffer com o membro a ser inserido
  fread(buffer, tam_new_membro, 1, N_membro);

  //encontra o final do arquivo após aumentar o diretório
  fseek(file, aumento, SEEK_END);

  novo->posi_ini = membros->posi_fim;
  novo->ordem = (membros->N_itens) + 1;

  //insere o novo membro (tamanho que o diretório aumenta) bytes após o final do arquivo 
  fwrite(buffer, tam_new_membro, 1, file);

  //move os outros arquivos (tamanho que o diretório aumenta) bytes para a frente;
  for (int i = (dir->Numero_membros); i > 0 ; i--){

    struct membro* aux;

    aux = lista_busca(membros, i);

    long f_ini = aux->posi_ini;
    long f_fim = (f_ini + (aux->tam_atual));
    long f_dest = (f_ini + aumento);
    
    move_bloco(file, f_ini, f_fim, f_dest);
    aux->posi_ini = f_dest;
  }
      //aumenta o tamanho do diretório
    lista_insere(membros,novo);
}

void inserecomp(){
}

void move(){

}

void unpack(){

}

void remover(){

}

void listagem(){

}
