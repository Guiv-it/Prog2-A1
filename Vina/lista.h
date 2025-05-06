#include <stdio.h>
#include <stdlib.h>

#ifndef lista
#define lista

struct membro{
  /*nome, UID, tamanho original, tamanho em disco e data de modificação*/
  char nome[1024];
  int UID;
  int tam_ini;
  int tam_atual;
  int mod_data;
  int posi_ini;

}membro;

struct item{
  struct membro* arquivo;
  struct item* prox;
};

struct lista_t {
  struct item* ini;
  struct item* fim;
};

struct lista_t listacria(){
}

void lista_insere(struct lista_t* l, struct membro* arquivo){
}

struct membro* lista_retira(struct lista_t* l, struct membro* arquivo){
}

void imprime(struct lista_t* l){  
}

void lista_move(struct lista_t* l, struct membro* arquivo, struct membro* destino){
}

void listadestroi(struct lista_t* l){
}
#endif