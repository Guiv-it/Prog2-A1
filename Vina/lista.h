#include <stdio.h>
#include <stdlib.h>

#ifndef lista
#define lista

struct membro{  //tamanho membro: 1044
  /*nome, UID, tamanho original, tamanho em disco e data de modificação*/
  char nome[1024];
  int UID;
  int tam_ini;
  int tam_atual;
  int mod_data;
  int posi_ini;

}membro;

struct item{ //tamanho item: 16
  struct membro* arquivo;
  struct item* prox;
};

struct lista_t { //tamanho lista_t: 16
  struct item* ini;
  struct item* fim;
};

struct lista_t* lista_cria(){
}

void lista_insere(struct lista_t* l, struct membro* arquivo){
}

struct membro* lista_busca(struct lista_t* l, long pos){
}

struct membro* lista_retira(struct lista_t* l, struct membro* arquivo){
}

void lista_imprime(struct lista_t* l){  
}

void lista_move(struct lista_t* l, struct membro* arquivo, struct membro* destino){
}

void lista_destroi(struct lista_t* l){
}
#endif