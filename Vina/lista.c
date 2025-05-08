#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "lz.h"
#include "lista.h"

void corrige_ordem(struct lista_t* l){
  struct item* aux = l->ini;
  int i = 1;
  while(aux){
    aux->arquivo->ordem = i;
    i++;
  }
  return;
}

struct lista_t* lista_cria(){
  struct lista_t* aux = malloc(sizeof(struct lista_t));
  if (!aux)
    return NULL;
  aux->ini = NULL;
  aux->fim = NULL;
  aux->N_itens = 0;
  aux->posi_fim = sizeof(struct diretorio);
  return aux;
}

void lista_insere(struct lista_t* l, struct membro* arquivo){

  struct item* aux = malloc(sizeof(struct item));
  aux->prox = NULL;
  aux->arquivo = arquivo;

  if (!l->ini){
    l->ini = aux;
    l->fim = aux;
  } else {
    l->fim->prox = aux;
    l->fim = aux;
  }
  l->N_itens++;
  l->posi_fim = l->posi_fim + sizeof(struct membro);

  corrige_ordem(l);
  return;
}

struct membro* lista_busca_nome(struct lista_t* l, char* nome){
  if(!l){
    return NULL;
  }
  if(!nome){
    return NULL;
  }

  struct item* aux = l->ini;
  struct membro* temp;

  while(aux->prox){
    if((strcmp(aux->arquivo->nome, nome)) == 0)
      return aux->arquivo;
    aux = aux->prox;
  }
  return NULL;
}

struct membro* lista_busca_posi(struct lista_t* l, long pos){
  if(!l)
    return NULL;

  struct item* aux = l->ini;

  if (pos == -1)
    return l->fim;

  for (int i = 1; i < pos; i++){
    aux = aux->prox;
  }
  return aux;
}

struct membro* lista_retira(struct lista_t* l, char nome[]){

  if(!l)
    return NULL;
  
  struct item* aux = l->ini;
  struct item* corrige = l->ini;
  struct membro* temp;

  while((aux)&&(strcmp(nome,aux->arquivo->nome)) != 0){
  corrige = aux;
  aux = aux->prox;
  }
  if(!aux)
    return NULL;
  if (aux = l->ini){
    l->ini = aux->prox;
  } else if(aux = l->fim) {
    l->fim = corrige; 
  }
  corrige->prox = aux->prox;
  while(corrige->prox){
    corrige=corrige->prox;
  }
  temp = aux->arquivo;
  aux->arquivo = NULL;
  free(aux);

  l->N_itens--;
  l->posi_fim = l->posi_fim - sizeof(struct membro);
  corrige_ordem(l);
  return temp;
}

void lista_imprime(struct lista_t* l){ 
  if(!l)
    return NULL;
  struct item* aux = l->ini;
  while (aux)
  {
    puts(aux->arquivo->nome);
    printf("\n UID: %f", aux->arquivo->UID);
    printf("  Tamanho Original: %f", aux->arquivo->tam_ini);
    printf("  Tamanho Compactado: %f", aux->arquivo->tam_atual);
    printf("  Data de alteração: %f \n", aux->arquivo->mod_data);
  }
}

void lista_move(struct lista_t* l, struct membro* arquivo, struct membro* destino){

  if((!l)||(!arquivo))
    return NULL;

  struct item* aux = l->ini;
  struct item* temp = l->ini;

  while ((aux)&&(strcmp(arquivo->nome,aux->arquivo->nome) != 0)){
    temp = aux;
    aux = aux->prox;
  }
  if (!aux)
    return;
  temp->prox = aux->prox;
  if (aux->prox = NULL)
    l->fim = temp;
  temp = l->ini;
  if (destino){
    while ((temp)&&(strcmp(destino->nome,temp->arquivo->nome) != 0))
      temp = temp->prox;
    if (!temp)
      return;
    aux->prox = temp ->prox;
    temp->prox = aux;
    return;
  }
  aux->prox = l->ini;
  l->ini = aux;
  corrige_ordem(l);
  return;
}

void lista_destroi(struct lista_t* l){
  if (!l)
    return NULL;
  
  struct item* aux = l->ini;
  struct item* retira = l->ini;
  while(aux->prox != NULL){
    aux = aux->prox;
    free(retira);
    retira = aux;
  }
  free(aux);
  free(l);
  return;
}