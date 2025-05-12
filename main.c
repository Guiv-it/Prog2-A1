#include <getopt.h>

#include "vina.h"

#define ARQUIVO "copag.vc"

void CriaArquivo(char* name)
{
    FILE* arquivo;
    struct diretorio dir = criadir();

    //Criando os arquivos
    arquivo = fopen(name, "wb+");
    if (arquivo == NULL)
    {
        fprintf(stderr, "Erro criando arquivo");
        return;
    }
    
    fwrite(&dir, 1, sizeof(struct diretorio), arquivo);
    fclose(arquivo);
}

void Destroiarquivo(FILE* arquivo){
  
  struct diretorio* dir = dirfetch(arquivo);
  ftruncate(fileno(arquivo), 0);

  free(dir);
  fclose(arquivo);
  return;
}
void imprimedir(FILE* file){
  struct diretorio* dir = dirfetch(file);
  printf("tamanho: %ld\n",dir->tamdir);
  printf("N_membros: %ld\n",dir->N_membros);
  free(dir);
}

int main(int argc, char *argv[]){

  char opçao;

  char* nome_b;
  FILE* arquivo_b;
  FILE* arquivo_novo;
  
  while ((opçao = getopt(argc, argv, ":p:i:m:x:r:c"))!= -1){
    switch (opçao)
    {
      case 'p':
        nome_b = strdup(optarg);
        if(access(nome_b, F_OK) == -1){
          CriaArquivo(nome_b);
        }
        arquivo_b = fopen( nome_b, "rb+");
        while (optind < argc){
          char nome_novo[1024];

          strncpy(nome_novo, argv[optind], (sizeof(nome_novo)-1));

          arquivo_novo = fopen(nome_novo, "r");
          fseek(arquivo_novo, 0, SEEK_END);
          long tam = ftell(arquivo_novo);
          fseek(arquivo_novo, 0, SEEK_SET);

          insere(arquivo_b, arquivo_novo, nome_novo, tam);
          fclose(arquivo_novo);
          optind++;
        }
      break;

      case 'i':
        nome_b = strdup(optarg);
        if(access(nome_b, F_OK) == -1){
          CriaArquivo(nome_b);
        } 
        arquivo_b = fopen( nome_b, "rb+");

        if(arquivo_b == NULL){
          fprintf(stderr, "arquivo não abriu");
          return -1;
        }

        while (optind < argc){
          char nome_novo[1024];

          strncpy(nome_novo, argv[optind], sizeof(nome_novo));

          arquivo_novo = fopen(nome_novo, "rb");

          inserecomp(arquivo_b, arquivo_novo, nome_novo);

          optind++;
        } 
      break;
      case 'm':

        nome_b = strdup(optarg);

        if(access(nome_b, F_OK) == -1){
          return -1;
        } 
    
        arquivo_b = fopen( nome_b, "rb+");

        char objeto[1024];
        char alvo[1024];

        fprintf(stderr, "argv[optind] %s\n", argv[optind]);
        fprintf(stderr, "argc %d\n", argc);
      
        strncpy(objeto, argv[optind], sizeof(objeto)-1);
        optind++;
        if (argc == 4){
          move(arquivo_b,objeto, NULL);
        } else {
        fprintf(stderr, "argv[optind] %s", argv[optind]);
        strncpy(alvo, argv[optind], sizeof(alvo)-1);
      
        move(arquivo_b,objeto, alvo);}

      break;
        case 'x':
        nome_b = strdup(optarg);

        if(access(nome_b, F_OK) == -1){
          return -1;
        } 
    
        arquivo_b = fopen( nome_b, "rb+");

        if(optind == argc){
          extrai(arquivo_b, NULL);

        }else{

          while(optind < argc){

            char nome_ext[1024];

            strncpy(nome_ext, argv[optind], sizeof(nome_ext)-1);

            extrai(arquivo_b, nome_ext);
            optind++;
          }
        }
      break;
      case 'r':
        nome_b = strdup(optarg);
        if(access(nome_b, F_OK) == -1){
          CriaArquivo(nome_b);
        } 
        arquivo_b = fopen( nome_b, "rb+");

        while (optind < argc){
          char nome_alvo[1024];

          strncpy(nome_alvo, argv[optind], sizeof(nome_alvo)-1);

          remover(arquivo_b, nome_alvo);
          optind++;          
        }
      break;
      case 'c':
        nome_b = strdup(argv[2]);
        if(access(nome_b, F_OK) == -1){
          CriaArquivo(nome_b);
        } 
        arquivo_b = fopen( nome_b, "rb+");

        listagem(arquivo_b, nome_b);
      break;
    }
  }
  fclose(arquivo_b);
  free(nome_b);
  return 0;
}