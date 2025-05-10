#include <getopt.h>

#include "vina.h"

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
    
    //Colocando o diretorio no arquivo
    
    fwrite(&dir, 1, sizeof(long), arquivo);
}


int main(int argc, char *argv[]){
  //int funct;
  //getopt(argc, argv, ":p:i:m:x:r:c");
  char* arq_nome = calloc(1024, sizeof(char));
  FILE* arquivo_bin;
  FILE* arquivo;
  int tam;


  CriaArquivo("Josias");
  /*arquivo_bin = fopen("Josias", "ab+");

  strcpy(arq_nome, "teste.txt");
  arquivo = fopen("teste.txt", "ab+");
  fseek(arquivo, 0, SEEK_END);
  tam = ftell(arquivo);

  insere(arquivo_bin, arquivo, arq_nome, tam);

  fclose(arquivo);
  fclose(arquivo_bin);*/
  return 0;
  /*arq_nome = strdup(optarg); 
  if(arq_nome == NULL){
    fprintf(stderr, "Erro ao pegar argumento\n");
    return 1;
    }
                
    //Se o arquivo nao existe, crie ele
    if(access(arq_nome, F_OK) == -1)
      CriaArquivo(arq_nome);*/
}