#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
//#include <unistd.h>

#ifndef lista
#define lista

struct membro{  //tamanho membro: 1072
  /*nome, UID, tamanho original, tamanho em disco e data de modificação*/
  char nome[1024];
  uid_t UID;
  long tam_ini;
  long tam_atual;
  time_t mod_data;
  int ordem;
  long posi_ini;
};

struct item{ //tamanho item: 16
  struct membro* arquivo;
  struct item* prox;
};

struct lista_t { //tamanho lista_t: 24
  struct item* ini;
  struct item* fim;
  int N_itens;
};

struct diretorio{ //tamanho diretório: 32
  FILE* dir;
  struct lista_t* membros;
  long tamdir;
  long Numero_membros;
};

struct arquivo { //tamanho arquivo: 16
  struct diretorio* dir;
  FILE* membro;
};

struct lista_t* lista_cria(){
}

void lista_insere(struct lista_t* l, struct membro* arquivo){
}

struct membro* lista_busca_nome(struct lista_t* l, char* nome){
}

struct membro* lista_busca_posi(struct lista_t* l, long pos){
}

struct membro* lista_retira(struct lista_t* l, char nome[]){
}

void lista_imprime(struct lista_t* l){  
}

void lista_move(struct lista_t* l, struct membro* arquivo, struct membro* destino){
}

void lista_destroi(struct lista_t* l){
}

#endif