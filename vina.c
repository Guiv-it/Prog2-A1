
#include "lz.h"
#include "vina.h"

struct diretorio criadir(){
  struct diretorio archi;

  archi.N_membros = 0;
  archi.tamdir=(2*sizeof(long));

  return archi;
}

struct membro criamembro(FILE* file, char* nome){
  struct membro Novo_membro;
  struct stat* buffer = malloc(sizeof(struct stat));
  int fd = fileno(file);

  fstat(fd, buffer);
    
  strcpy(Novo_membro.nome, nome);
  Novo_membro.UID = buffer->st_uid;
  Novo_membro.tam_ini = buffer->st_size;
  Novo_membro.tam_atual = buffer->st_size;
  Novo_membro.mod_data = buffer->st_mtime;
  Novo_membro.ordem = 0;
  Novo_membro.posi_ini = 0;

  free(buffer);
  return Novo_membro;
}

struct diretorio* dirfetch(FILE* archi){

  fseek(archi,0,SEEK_SET);
  struct diretorio* dir = calloc(1, sizeof(struct diretorio));
  long* buffer_tam = malloc(sizeof(long));

  fread(buffer_tam, sizeof(long), 1, archi);
  dir->tamdir = *buffer_tam;

  fseek(archi,sizeof(long),SEEK_SET);
  fread(buffer_tam, sizeof(long), 1, archi);
  dir->N_membros = *buffer_tam;
  free(buffer_tam);
  
  return dir;
}

struct membro* membrofetch_posi(FILE* file, int posi){

  long offset =  sizeof(struct diretorio) + (posi-1)*sizeof(struct membro);
  fseek(file, offset, SEEK_SET);
  struct membro* alvo = calloc(1, sizeof(struct membro));
  fread(alvo, sizeof(struct membro), 1, file);
  return alvo;
}

struct membro* membrofetch_nome(FILE* file, char* nome){

  long offset = sizeof(struct diretorio);
  struct membro* alvo = calloc(1, sizeof(struct membro));
  fseek(file, offset, SEEK_SET);
  fread(alvo, sizeof(struct membro), 1, file);
  struct diretorio* dir = dirfetch(file);
  long fim_membro = offset+(dir->N_membros * sizeof(struct membro));

  while((strcmp(nome,alvo->nome)!=0)&&(offset<fim_membro)){
    offset = offset + sizeof(struct membro);
    fseek(file, offset, SEEK_SET);
    fread(alvo, sizeof(struct membro), 1, file);
  }

  if(offset>=fim_membro){
    free(dir);
    free(alvo);
    return NULL;
  }
    free(dir);
    return alvo;
}

//move o grupo de blocos que vai do "inicio" até o "final" para que seu início fique na posição ref
void move_bloco(FILE* file, long inicio, long tam, long ref){

  char *buffer = (char *)malloc(tam);

  fseek(file, inicio, SEEK_SET);
  fread(buffer, tam, 1, file);
  fseek(file, ref, SEEK_SET);
  fwrite(buffer, tam, 1, file); 

  free(buffer);
  return;
}

void insere(FILE* file, FILE* N_membro, char* nome, long tam_c){

  struct diretorio* p_dir = dirfetch(file);
  struct diretorio dir = *p_dir;
  free(p_dir);
  struct membro novo = criamembro(N_membro, nome);

  fseek(N_membro, 0, SEEK_END);
  long tam_new_membro = ftell(N_membro) ;
  char* buffer = (char *)malloc(tam_new_membro);
  fseek(N_membro, 0, SEEK_SET);
  novo.tam_atual = tam_c;
  
  fread(buffer, tam_new_membro, 1, N_membro);
  struct membro* busca = membrofetch_nome(file, nome);

  //checa se é um novo arquivo ou arquivo existente
  if(busca){
    long aumento = (novo.tam_atual - busca->tam_atual);
    novo.posi_ini = busca->posi_ini;

    //se o arquivo não mudou de tamanho, apenas escreve por cima do arquivo;
    if(aumento == 0){
      fseek(file, novo.posi_ini, SEEK_SET);
      fwrite(buffer, tam_new_membro, 1, file);
      free(busca);
      free(buffer);
      return;
    }
    if(aumento > 0){
      for (long i = dir.N_membros-1 ; i > busca->ordem; i--){

      struct membro* aux  = membrofetch_posi(file, i);
      long f_ini = aux->posi_ini;
      long tam = (aux->tam_atual);
      long f_dest = (f_ini + aumento);
    
      move_bloco(file, f_ini, tam, f_dest);
      aux->posi_ini = f_dest;
    }
    free(busca);
    } else {
      for (long i = busca->ordem+1 ; i < dir.N_membros-1 ; i++){

      struct membro* aux  = membrofetch_posi(file, i);
      long f_ini = aux->posi_ini;
      long tam = (aux->tam_atual);
      long f_dest = (f_ini + aumento);
    
      move_bloco(file, f_ini, tam, f_dest);
      aux->posi_ini = f_dest;
    }
    free(busca);
  }

  } else {

    free(busca);
    long aumento = sizeof(struct membro);
    char* expandir = (char *)calloc(1, aumento);
    //encontra o final do arquivo após aumentar o diretório
    
    
    fwrite(expandir, aumento, 1, file);
    free(expandir);
    fseek(file, 0, SEEK_END);
    novo.posi_ini = ftell(file);
    novo.ordem = dir.N_membros;

    //insere o novo membro após o final do arquivo 
    fwrite(buffer, tam_new_membro, 1, file);

    //move os outros arquivos (tamanho que o diretório aumenta) bytes para a frente;
    for (long i = (dir.N_membros); i > 0 ; i--){

      struct membro* aux  = membrofetch_posi(file, i);
      long f_ini = aux->posi_ini;
      long tam = (aux->tam_atual);
      long f_dest = (f_ini + aumento);
    
      move_bloco(file, f_ini, tam, f_dest);
      aux->posi_ini = f_dest;
      free(aux);
    }
    //insere o novo membro no diretório
    long membro_posi = (sizeof(struct diretorio) + dir.N_membros*sizeof(struct membro));

    fseek(file, membro_posi, SEEK_SET);
    fwrite(&novo, sizeof(struct membro), 1, file);
    //aumenta o tamanho do diretório
    dir.N_membros++;
    
  }
  fseek(file, 0,SEEK_SET);
  fwrite(&dir,sizeof(struct diretorio),1, file);
 

  free(buffer);
  return;
}

void inserecomp(FILE* file, FILE* N_membro, char* nome){

  
  fseek(N_membro, 0, SEEK_SET);
  fseek(N_membro, 0, SEEK_END);
  long tam_new_membro = ftell(N_membro);
  unsigned char* buffer_i = ( unsigned char *)malloc(tam_new_membro);
  fseek(N_membro, 0, SEEK_SET);

  //carregar Buffer_in
  fread(buffer_i, tam_new_membro, 1, N_membro);

  //criar Buffer_out
   unsigned char* buffer_o =( unsigned char *)malloc(tam_new_membro * 1.004 + 1);

  //comprime arquivo 
  int tam_c = LZ_Compress(buffer_i, buffer_o, tam_new_membro);

  
  char* c_nome = malloc(1025);
  strcpy(c_nome, nome);
  strcat(c_nome, ".lz");

  FILE* N_membro_c = fopen(c_nome, "wb+");
  fwrite(buffer_o, tam_c, 1, N_membro_c);

  free(buffer_o);
  free(buffer_i);
  free(c_nome);

  //checa se o arquivo é maior que a sua versão compactada
  if (tam_new_membro > tam_c){
    insere(file, N_membro_c, nome, tam_c);
  } else { //se o arquivo compactado for maior ou igual ao arquivo sem compactação
    insere(file, N_membro, nome, tam_new_membro);
  }
  fclose(N_membro_c);
}

void move(FILE* file, char* origem, char* destino){

  struct diretorio* dir = dirfetch(file);
  struct membro* ultimo = membrofetch_posi(file, dir->N_membros);
  long fim = (ultimo->posi_ini)+(ultimo->tam_atual);

  //encontrar a origem
  struct membro* objeto = membrofetch_nome(file, origem);
  if(!objeto){
    fprintf(stderr, "alvo do mover não existe");
    free(ultimo);
    free(dir);
    return;
  }
  int ordem_objeto = objeto->ordem;

  //colocar origem no fim do arquivo.
  move_bloco(file, objeto->posi_ini,objeto->tam_atual,fim);

  //encontrar o destino, se destino == "NULL" vira primeiro da fila
  if (!destino){

    //mover todos os blocos antes do objeto (tam do objeto) para frente
    for(int i = (ordem_objeto-1); i > 0; i--){
      struct membro* aux = membrofetch_posi(file, i);
      aux->ordem++;
      long dest = (aux->posi_ini)+(objeto->tam_atual);
      move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
      free (aux);
    }
    //colocar origem após os membros
    long fim_dir = dir->tamdir + dir->N_membros*sizeof(struct membro);
    move_bloco(file, fim, objeto->tam_atual, fim_dir);
    struct membro* aux = membrofetch_posi(file, 1);
    aux->ordem = 0;
    free(aux);
    //corrige membros
    long alvo_membro = dir->tamdir + ordem_objeto*sizeof(struct membro);

    move_bloco(file, alvo_membro, sizeof(struct membro), fim);

    for(int i = (ordem_objeto-1); i > 0; i--){
      long inicio_membro = dir->tamdir + i*sizeof(struct membro);
      long dest = inicio_membro + sizeof(struct membro);
      move_bloco(file, inicio_membro, sizeof(struct membro), dest);
    }
    move_bloco(file, fim, sizeof(struct membro), dir->tamdir);
  } else { 
    //encontrar o destino
    struct membro* alvo = membrofetch_nome(file, destino);
    if (alvo->ordem < objeto->ordem){
      //move os dados entre alvo e objeto para direita (tam objeto) bytes
      for(int i = (objeto->ordem-1); i > alvo->ordem; i--){

        struct membro* aux = membrofetch_posi(file, i);
        long dest = aux->posi_ini + objeto->tam_atual;
        aux->ordem++;
        move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
        aux->posi_ini = dest;
        free(aux);
      }
      move_bloco(file, fim, objeto->tam_atual, alvo->posi_ini+alvo->tam_atual);
      //corrige membros
      long alvo_membro = dir->tamdir + ordem_objeto*sizeof(struct membro);

      move_bloco(file, alvo_membro, sizeof(struct membro), fim);

      for(int i = (ordem_objeto-1); i > 0; i--){
        long inicio_membro = dir->tamdir + i*sizeof(struct membro);
        long dest = inicio_membro + sizeof(struct membro);
        move_bloco(file, inicio_membro, sizeof(struct membro), dest);
      }
    move_bloco(file, fim, sizeof(struct membro), dir->tamdir);
      
    } else {
      struct membro* alvo = membrofetch_posi(file, 1);
    
      //move os dados entre alvo e objeto para esquerda (tam objeto) bytes
      for(int i = (objeto->ordem+1); i <= alvo->ordem; i--){

        struct membro* aux = membrofetch_posi(file, i);
        long dest = aux->posi_ini - objeto->tam_atual;

        move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
        aux->posi_ini = dest;
      }
      
    }
    //coloca o alvo na nova posição
    move_bloco(file, fim, objeto->tam_atual, (alvo->posi_ini+alvo->tam_atual));
    objeto->posi_ini = (alvo->posi_ini+alvo->tam_atual);
    //arruma a lista
    

  }
  fim = (ultimo->posi_ini)+(ultimo->tam_atual);
  free(dir);
  free(ultimo);
  //concatena o fim do arquivo
  ftruncate(fileno(file), fim);
}

void extrai(FILE* file, char* nome){

  //extrai todos
  if(nome == NULL){

  }
  if(file == NULL){

  }

  return;
}

/*-r : remove os membros indicados de archive;*/
void remover(FILE* file, char* nome){

  struct diretorio* dir = dirfetch(file);
  struct membro* ultimo = membrofetch_posi(file, dir->N_membros);
  long fim = (ultimo->posi_ini)+(ultimo->tam_atual);

  struct membro* objeto = membrofetch_nome(file, nome);
 
  //colocar struct no fim do arquivo
  long ini_membro = dir->tamdir + objeto->ordem*sizeof(struct diretorio);
  move_bloco(file, ini_membro, sizeof(struct membro), fim);

  //arruma as posições dos structs
  for(int i = objeto->ordem+1; i <= dir->N_membros; i++){
    long inicio_membro = dir->tamdir + i*sizeof(struct membro);
    long dest = inicio_membro + sizeof(struct membro);
    move_bloco(file, inicio_membro, sizeof(struct membro), dest);
  }

  //colocar objeto no fim do arquivo
  move_bloco(file, objeto->posi_ini,objeto->tam_atual,fim);

  //trazer arquivos a direita do alvo (tamanho do alvo) para esquerda
  for(int i = objeto->ordem+1; i < dir->tamdir ; i++){

    struct membro* aux = membrofetch_posi(file, i);
    long dest = aux->posi_ini - objeto->tam_atual;
    aux->ordem--;
    move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
    aux->posi_ini = dest;
    free(aux);
  }

  //trazer arquivos (tam que o diretório diminui) para esquerda
  for(int i = 1; i < dir->tamdir ; i++){
    struct membro* aux = membrofetch_posi(file, i);
    long dest = aux->posi_ini - sizeof(struct membro);
    move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
    aux->posi_ini = dest;
    free(aux);
  }

  ftruncate(fileno(file), fim);

}

void listagem(FILE* file, char* nome){
  struct diretorio* dir = dirfetch(file);
  printf("ARQUIVOS EM %s: \n\n", nome);
  for (int i = 1; i <= dir->N_membros; i++ ){
    struct membro* aux = membrofetch_posi(file, i);
    printf("======================================\n");
    printf("nome: %s\n", aux->nome );
    printf("UID: %lu\n", (unsigned long int)aux->UID);
    printf("tamanaho original: %ld\n", aux->tam_ini);
    printf("tamanho em disco: %ld\n", aux->tam_atual);
    printf("data de modificação: %ld\n", aux->mod_data);
    printf("posição: %d\n", aux->ordem);
    printf("======================================\n");
    free(aux);
  }
  free(dir);
}
