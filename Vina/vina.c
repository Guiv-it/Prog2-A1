#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>

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

struct membro* criamembro(FILE* file, char* nome){
    struct membro* Novo_membro = malloc(sizeof(struct membro));
    struct stat* buffer;
    int fd = fileno(file);

    fstat(fd, buffer);
    
    strcpy(Novo_membro->nome, nome);
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
void move_bloco(FILE* file, long inicio, long tam, long ref){

  char *buffer = (char *)malloc(tam);

  fseek(file, inicio, SEEK_SET);
  fread(buffer, tam, 1, file);
  fseek(file, ref, SEEK_SET);
  fwrite(buffer, tam, 1, file);

  //atualizar a posição_ini dentro da lista (arquivo->dir->membros)
  return;
}

void insere(struct arquivo* arquivo, FILE* N_membro, char* nome){

  struct diretorio* dir = dirfetch(arquivo);
  struct lista_t* membros = lista_membros(arquivo->dir);
  struct membro* novo = criamembro(N_membro, nome);

  FILE* file = arquivo->membro;

  fseek(N_membro, 0, SEEK_END);
  long tam_new_membro = ftell(N_membro);
  char* buffer = (char *)malloc(tam_new_membro);
  fseek(N_membro, 0, SEEK_SET);
  
  
  //carrega o buffer com o membro a ser inserido
  fread(buffer, tam_new_membro, 1, N_membro);

  //checa se é um novo arquivo ou arquivo existente
  if (novo == lista_busca_nome(membros, nome)){
    int posi = novo->ordem;
    long aumento = (tam_new_membro - novo->tam_atual);

    //se o arquivo não mudou de tamanho, apenas escreve por cima do arquivo;
    if(aumento == 0){
      fseek(file, novo->posi_ini, SEEK_SET);
      fwrite(buffer, tam_new_membro, 1, file);
      return;
    }
    //caso o arquivo mudou de tamanho, arruma a posição de todos os arquivo a sua frente
    for (int i = membros->N_itens; i > posi ; i--){

      struct membro* aux;

      aux = lista_busca_posi(membros, i);

      long f_ini = aux->posi_ini;
      long tam = (aux->tam_atual);
      long f_dest = (f_ini + aumento);
    
      move_bloco(file, f_ini, tam, f_dest);

      aux->posi_ini = f_dest;
    }
    //e então se sobrescreve com a nova versão salva
    fseek(file, novo->posi_ini, SEEK_SET);
    fwrite(buffer, tam_new_membro, 1, file);
  } else {

    long aumento = sizeof(struct membro);

    //encontra o final do arquivo após aumentar o diretório
    novo->posi_ini = fseek(file, aumento, SEEK_END);


    //insere o novo membro (tamanho que o diretório aumenta) bytes após o final do arquivo 
    fwrite(buffer, tam_new_membro, 1, file);

    //move os outros arquivos (tamanho que o diretório aumenta) bytes para a frente;
    for (int i = (dir->Numero_membros); i > 0 ; i--){

      struct membro* aux;

      aux = lista_busca_posi(membros, i);

      long f_ini = aux->posi_ini;
      long tam = (aux->tam_atual);
      long f_dest = (f_ini + aumento);
    
      move_bloco(file, f_ini, tam, f_dest);
      aux->posi_ini = f_dest;
    }
    //aumenta o tamanho do diretório
    lista_insere(membros,novo);
  }
  return;
}

void inserecomp(){
}

void move(struct arquivo* arquivo, char* origem, char* destino){

  struct diretorio* dir = dirfetch(arquivo);
  struct lista_t* membros = lista_membros(arquivo->dir);
  FILE* file = arquivo->membro;
  struct membro* ultimo = lista_busca_posi(membros, -1);
  long fim = (ultimo->posi_ini)+(ultimo->tam_atual);

  //encontrar a origem
  struct membro* objeto = lista_busca_nome(membros, origem);
  int posi_inicial = objeto->ordem;

  //colocar origem no fim do arquivo.
  move_bloco(file, objeto->posi_ini,objeto->tam_atual,fim);

  //encontrar o destino, se destino == "NULL" vira primeiro da fila
  if (!strcmp("NULL", destino)){

    //mover todos os blocos antes do objeto (tam do objeto) para frente
    for(int i = (posi_inicial-1); i > 0; i--){
      struct membro* aux = lista_busca_posi(membros, i);
      long dest = (aux->posi_ini)+(objeto->tam_atual);
      move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
    }

    lista_move(membros, objeto, NULL);

  } else { 
    //encontrar o destino
    struct membro* alvo = lista_busca_nome(membros, destino);
    if (alvo->ordem < posi_inicial){
      //move os dados entre alvo e objeto para direita (tam objeto) bytes
    } else {
      //move os dados entre alvo e objeto para esquerda (tam objeto) bytes
      
    }
    //coloca o alvo na nova posição
      //arruma a lista
      //concatena o fim do arquivo

  }

  //colocar origem após o destino 

  //arrumar fila



}

void unpack(){

}

/*-r : remove os membros indicados de archive;*/
void remover(struct arquivo* arquivo, char* nome){

  struct diretorio* dir = dirfetch(arquivo);
  struct lista_t* membros = lista_membros(arquivo->dir);
  FILE* file = arquivo->membro;

  //encontrar objeto a ser removido
  struct membro* alvo = lista_busca_nome(membros, nome);
  int posi = alvo->ordem;

  //colocar objeto no fim do arquivo
  fseek(file, 0, SEEK_END);
  long final = ftell(file);

  move_bloco(file, alvo->posi_ini,alvo->tam_atual,final);

  //arruma as posições na lista
  struct membro* ultimo = lista_busca_posi(membros, -1);
  lista_move(membros, alvo, ultimo);

  //trazer arquivos a direita do alvo (tamanho do alvo) para esquerda
  for(int i = posi ; i < dir->Numero_membros; i++){
    struct membro* aux = lista_busca_posi(membros, i);

    long tam = aux->tam_atual;
    long dest = (aux->posi_ini)-(alvo->tam_atual);
    move_bloco(file,aux->posi_ini, tam, dest);

    aux->posi_ini = dest;
  }

  //remover objeto da fila
  lista_retira(membros, nome);

  //trazer arquivos (tam que o diretório diminui) para esquerda
  long aumento = sizeof(struct membro);

  for(int i = 1 ; i <= dir->Numero_membros; i++){

    struct membro* aux = lista_busca_posi(membros, i);

    long tam = aux->tam_atual;
    long dest = (aux->posi_ini)-aumento;
    move_bloco(file,aux->posi_ini, tam, dest);

    aux->posi_ini = dest;
  }
  //diminuir o tamanho do diretório no struct
  dir->tamdir = (dir->tamdir)-aumento;

  //cortar os ultios bytes do arquivo
  int fd = fileno(file);
  ftruncate(fd, (ultimo->posi_ini)+(ultimo->tam_atual));
}

void listagem(struct arquivo* arquivo){
  struct diretorio* dir = dirfetch(arquivo);
  struct lista_t* membros = lista_membros(arquivo->dir);
  FILE* file = arquivo->membro;
  
  for (int i = 1; i <= dir->Numero_membros; i++ ){
    struct membro* aux = lista_busca_posi(membros, i);
    printf("======================================\n");
    printf("nome: %d\n", aux->nome );
    printf("UID: %d\n", aux->UID);
    printf("tamanaho original: %d\n", aux->tam_ini);
    printf("tamanho em disco: %d\n", aux->tam_atual);
    printf("data de modificação: %d\n", aux->mod_data);
    printf("posição: %d\n", aux->ordem);
    printf("======================================\n");
  }
}
