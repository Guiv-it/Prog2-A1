
#include "lz.h"
#include "vina.h"

struct diretorio criadir(){
  struct diretorio archi;

  archi.N_membros = 0;
  archi.tamdir=(2*sizeof(long));

  return archi;
}

struct membro criamembro(char* nome){
  struct membro Novo_membro;
  FILE* file = fopen(nome, "rb");
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

  fclose(file);
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
  if(!buffer)
  {
    fprintf(stderr,"buffer não alocado");
    return;
  }

  fseek(file, inicio, SEEK_SET);
  fread(buffer, tam, 1, file);
  fseek(file, ref, SEEK_SET);
  fwrite(buffer, tam, 1, file); 

  free(buffer);
  return;
}

void insere(FILE* file, char* N_membro, char* nome,long tam_ini ,long tam_c){

  struct diretorio* p_dir = dirfetch(file);
  struct diretorio dir = *p_dir;
  free(p_dir);
  struct membro novo = criamembro(nome);
  long aumento;

  long tam_new_membro = tam_c;
  novo.tam_ini = tam_ini;
  novo.tam_atual = tam_c;
  
  struct membro* busca = membrofetch_nome(file, nome);

  //checa se é um novo arquivo ou arquivo existente
  if(busca){
    fprintf(stderr, "arquivo repetido\n");
    aumento = (novo.tam_atual - busca->tam_atual);
    fprintf(stderr, "valor de aumento: %ld\n", aumento);
    novo.posi_ini = busca->posi_ini;

    //se o arquivo não mudou de tamanho, apenas escreve por cima do arquivo;
    if(aumento == 0){
      fprintf(stderr, "não aumentou\n");
      fseek(file, novo.posi_ini, SEEK_SET);
      fwrite(N_membro, tam_new_membro, 1, file);
      free(busca);
      free(N_membro);
      return;
    }
    if(aumento > 0){
      fprintf(stderr, "aumentou\n");
      for (long i = dir.N_membros ; i > busca->ordem; i--){
        struct membro* aux  = membrofetch_posi(file, i);
        long f_ini = aux->posi_ini;
        long tam = (aux->tam_atual);
        long f_dest = (f_ini + aumento);
    
        move_bloco(file, f_ini, tam, f_dest);
        free(aux);
      }
    } else {
      fprintf(stderr, "diminuiu\n");
      for (long i = busca->ordem+2 ; i <= dir.N_membros ; i++){
        struct membro* aux  = membrofetch_posi(file, i);
        long f_ini = aux->posi_ini;
        long tam = (aux->tam_atual);
        long f_dest = (f_ini + aumento);
    
        move_bloco(file, f_ini, tam, f_dest);
        free(aux);
      }
      fseek(file, 0, SEEK_END);
      long fim = ftell(file);
      fim = fim+aumento;
      printf("fim %ld\n" ,fim);
      
      ftruncate(fileno(file),fim);
    }
    //corrige a posição inicial nos structs
    for(int i = busca->ordem+1 ; i <= dir.N_membros; i++){
      struct membro* aux  = membrofetch_posi(file, i);
      if(aux->ordem == busca->ordem){
        aux->tam_ini = tam_ini;
        aux->tam_atual = tam_c;
      }
      if(i > busca->ordem+1){
      aux->posi_ini = aux->posi_ini + aumento;
      }
      long offset = sizeof(struct diretorio) + (i-1)*sizeof(struct membro);

      fprintf(stderr,"nome: %s\n",aux->nome);
      fprintf(stderr,"i: %d \n",i);
      fprintf(stderr,"tamanho: %ld\n",aux->tam_atual);
      fprintf(stderr,"inicio: %ld\n",aux->posi_ini);
      fprintf(stderr,"offset: %ld\n",offset);
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);
      free(aux);
      
    }

    fseek(file, busca->posi_ini, SEEK_SET);
    fwrite(N_membro, tam_new_membro, 1, file);
    free(busca);

  } else {

    free(busca);
    aumento = sizeof(struct membro);
    char* expandir = (char *)calloc(1, aumento);
    //encontra o final do arquivo após aumentar o diretório
    
    fseek(file , 0, SEEK_END);
    fwrite(expandir, aumento, 1, file);
    free(expandir);
    fseek(file, 0, SEEK_END);
    novo.posi_ini = ftell(file);
    novo.ordem = dir.N_membros;

    //insere o novo membro após o final do arquivo 
    fwrite(N_membro, tam_new_membro, 1, file);

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

    fseek(file, 0,SEEK_SET);
    fwrite(&dir,sizeof(struct diretorio),1, file);
    //corrige a posição inicial nos structs
    for(int i = 1 ; i < dir.N_membros; i++){
    struct membro* aux  = membrofetch_posi(file, i);
    aux->posi_ini = aux->posi_ini+aumento;
    long offset = sizeof(struct diretorio) + (i-1)*sizeof(struct membro);
    fseek(file, offset, SEEK_SET);
    fwrite(aux, sizeof(struct membro), 1, file);
    free(aux);
  }
    
  }
  

  free(N_membro);
  return;
}

void inserecomp(FILE* file, FILE* N_membro, char* nome){

  
  fseek(N_membro, 0, SEEK_END);
  long tam_new_membro = ftell(N_membro);
  unsigned char* buffer_i = ( unsigned char *)malloc(tam_new_membro);
  fseek(N_membro, 0, SEEK_SET);

  //carregar Buffer_in
  fread(buffer_i, tam_new_membro, 1, N_membro);

  //criar Buffer_out
  unsigned char* buffer_o =( unsigned char *)malloc(tam_new_membro * 1.004 + 1);

  //comprime arquivo 
  long int tam_c = LZ_Compress(buffer_i, buffer_o, tam_new_membro);

  
  char* c_nome = malloc(1025);
  strcpy(c_nome, nome);
  strcat(c_nome, ".lz");
  printf("tam nmembro:%ld\ntam comp:%ld\n",tam_new_membro, tam_c);

  //checa se o arquivo é maior que a sua versão compactada
  if (tam_new_membro > tam_c){
    printf("arquivo maior que compactado");
    insere(file, (char*)buffer_o, nome,tam_new_membro, tam_c);
    free(buffer_i);
  } else { //se o arquivo compactado for maior ou igual ao arquivo sem compactação
    printf("arquivo menor ou igual que compactado");
    insere(file, (char*)buffer_i, nome,tam_new_membro, tam_new_membro);
    free(buffer_o);
  }
  
  free(c_nome);
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

  //colocar origem/objeto no fim do arquivo.
  move_bloco(file, objeto->posi_ini,objeto->tam_atual,fim);

  //encontrar o destino, se destino == NULL vira primeiro da fila
  if (!destino){
    fprintf(stderr,"destino NULL\n");
    //mover todos os blocos antes do objeto (tam do objeto) para frente
    for(int i = (ordem_objeto); i > 0; i--){
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
    //corrige lista
    long alvo_membro = dir->tamdir + ordem_objeto*sizeof(struct membro);
    move_bloco(file, alvo_membro, sizeof(struct membro), fim);

    for(int i = (ordem_objeto); i > 0; i--){
      struct membro* aux = membrofetch_posi(file, i);
      long offset =  sizeof(struct diretorio) + i * sizeof(struct membro);
      aux->posi_ini = aux->posi_ini+objeto->tam_atual;
      aux->ordem++;
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);
      free(aux);
    }
    long offset =  sizeof(struct diretorio) + dir->N_membros * sizeof(struct membro);

    objeto->posi_ini = offset;
    objeto->ordem = 0;
    fseek(file, sizeof(struct diretorio), SEEK_SET);
    fwrite(objeto, sizeof(struct membro), 1, file);
    
    free(objeto);

    ftruncate(fileno(file),fim);
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

      for(int i = (objeto->ordem); i > alvo->ordem; i--){
      struct membro* aux = membrofetch_posi(file, i);
      long offset =  sizeof(struct diretorio) + i * sizeof(struct membro);
      aux->posi_ini = aux->posi_ini+objeto->tam_atual;
      aux->ordem++;
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);
      free(aux);
    }
    long offset = sizeof(struct diretorio) + (alvo->ordem+1)*sizeof(struct membro);
    objeto->posi_ini = alvo->posi_ini+alvo->tam_atual;
    objeto->ordem = alvo->ordem + 1;
    fseek(file, offset, SEEK_SET);
    fwrite(objeto, sizeof(struct membro), 1, file);

    ftruncate(fileno(file),fim);
    } else {
      //move os dados entre alvo e objeto para esquerda (tam objeto) bytes
      for(int i = objeto->ordem+2; i <= alvo->ordem+1; i++){
        struct membro* aux = membrofetch_posi(file, i);
        long dest = aux->posi_ini - objeto->tam_atual; 
        move_bloco(file, aux->posi_ini, aux->tam_atual, dest);
        free(aux);
      }

      move_bloco(file, fim, objeto->tam_atual, alvo->posi_ini + alvo->tam_atual - objeto->tam_atual);
      //corrige membros
      long alvo_membro = dir->tamdir + ordem_objeto*sizeof(struct membro);

      move_bloco(file, alvo_membro, sizeof(struct membro), fim);

      for(int i = (objeto->ordem)+2; i <= alvo->ordem+1; i++){
      struct membro* aux = membrofetch_posi(file, i);
      long offset =  sizeof(struct diretorio) + (i-1) * sizeof(struct membro);
      aux->posi_ini = aux->posi_ini - objeto->tam_atual;
      aux->ordem--;
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);
      move_bloco(file, offset, sizeof(struct membro), (sizeof(struct diretorio) + (i-2) * sizeof(struct membro)));
      free(aux);
    }
    long offset = sizeof(struct diretorio) + (alvo->ordem)*sizeof(struct membro);
    alvo->posi_ini = alvo->posi_ini - objeto->tam_atual;
    objeto->posi_ini = alvo->posi_ini+alvo->tam_atual;
    objeto->ordem = alvo->ordem;
    fseek(file, offset, SEEK_SET);
    fwrite(objeto, sizeof(struct membro), 1, file);

    ftruncate(fileno(file),fim);
    }
    free(objeto);
    free(alvo);
  }
  free(dir);
  free(ultimo);
}

void extrai(FILE* file, char* nome){
  struct diretorio* dir = dirfetch(file);

  if(file == NULL){
    printf("arquivo não existe");
    return;
  }

  //extrai todos
  if(nome == NULL){
    for(int i = 1; i <= dir->N_membros; i++){
      struct membro* aux = membrofetch_posi(file, i);
      FILE* membro = fopen(aux->nome,"wb+");
      if (aux->tam_atual == aux->tam_ini){
        char* buffer = calloc(1, aux->tam_atual);
        fseek(file, aux->posi_ini, SEEK_SET);
        fread(buffer, aux->tam_atual, 1, file);
        fwrite(buffer, aux->tam_atual, 1, membro);
        free(buffer);
      } else {
        unsigned char* buffer_i = calloc(1, aux->tam_atual);
        unsigned char* buffer_o = calloc(1, aux->tam_ini);
        fseek(file, aux->posi_ini, SEEK_SET);
        fread(buffer_i, aux->tam_atual, 1, file);
        LZ_Uncompress(buffer_i, buffer_o, aux->tam_atual);
        fwrite(buffer_o, aux->tam_ini, 1, membro);
        free(buffer_i);
        free(buffer_o);
      }
      fclose(membro);
      free(aux);
    }
  }else{
    struct membro* aux = membrofetch_nome(file, nome);
    FILE* membro = fopen(aux->nome,"wb+");
    if (aux->tam_atual == aux->tam_ini){
      char* buffer = calloc(1, aux->tam_atual);
      fseek(file, aux->posi_ini, SEEK_SET);
      fread(buffer, aux->tam_atual, 1, file);
      fwrite(buffer, aux->tam_atual, 1, membro);
      free(buffer);
    } else {
      unsigned char* buffer_i = calloc(1, aux->tam_atual);
      unsigned char* buffer_o = calloc(1, aux->tam_ini);
      fseek(file, aux->posi_ini, SEEK_SET);
      fread(buffer_i, aux->tam_atual, 1, file);
      LZ_Uncompress(buffer_i, buffer_o, aux->tam_atual);
      fwrite(buffer_o, aux->tam_ini, 1, membro);
      free(buffer_i);
      free(buffer_o);
     }
     free(aux);
  }
  free(dir);
  return;
}

/*-r : remove os membros indicados de archive;*/
void remover(FILE* file, char* nome){

  struct diretorio* dir = dirfetch(file);
  struct membro* ultimo = membrofetch_posi(file, dir->N_membros);
  struct membro* objeto = membrofetch_nome(file, nome);
  long fim = (ultimo->posi_ini)+(ultimo->tam_atual)-(sizeof(struct membro)+objeto->tam_atual);
  
  //arruma as posições e dados dos structs
  fprintf(stderr, "arruma as posições e dados: \n");
  for(int i = 1; i <= dir->N_membros;i++){
    struct membro* aux = membrofetch_posi(file, i);
    long offset = sizeof(struct diretorio) + (i-1)*sizeof(struct membro);
    if(aux->ordem < objeto->ordem){
      aux->posi_ini = aux->posi_ini - sizeof(struct membro);
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);

    }else if(aux->ordem > objeto->ordem){
      aux->posi_ini = aux->posi_ini - (sizeof(struct membro) + objeto->tam_atual);
      aux->ordem--;
      fseek(file, offset, SEEK_SET);
      fwrite(aux, sizeof(struct membro), 1, file);
      move_bloco(file, offset, sizeof(struct membro), offset - sizeof(struct membro));
    }
      free(aux);
    }

    for(int i = 1 ; i < dir->N_membros;i++){
    struct membro* aux = membrofetch_posi(file, i);
    if(aux->ordem < objeto->ordem){
      move_bloco(file, aux->posi_ini+sizeof(struct membro), aux->tam_atual, aux->posi_ini);

    } else if(aux->ordem >= objeto->ordem){
      if(strcmp(aux->nome, objeto->nome) != 0){
        long inicio = aux->posi_ini + sizeof(struct membro) + objeto->tam_atual;
        move_bloco(file, inicio, aux->tam_atual, aux->posi_ini);
      }
    }
    free(aux);
  }
  dir->N_membros--;
  fseek(file, 0, SEEK_SET);
  fwrite(dir, sizeof(struct diretorio), 1, file); 
  free(dir);
  free(objeto);
  free(ultimo);
  ftruncate(fileno(file), fim);
}

void listagem(FILE* file, char* nome){
  struct diretorio* dir = dirfetch(file);
  printf("ARQUIVOS EM %s: \n\n", nome);
  for (int i = 1; i <= dir->N_membros; i++ ){
    struct membro* aux = membrofetch_posi(file, i);
    printf("======================================\n");
    printf("nome: %s\n", aux->nome );
    printf("posição inicial do arquivo: %ld\n", aux->posi_ini );
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
