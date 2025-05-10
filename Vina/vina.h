#ifndef vina
#define vina

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lz.h"
#include "lista.h"


struct diretorio criadir();

struct membro* criamembro(FILE* file, char* nome);

struct diretorio* dirfetch(FILE* file);

struct lista_t* lista_membros(struct diretorio* dir);

void move_bloco(FILE* file, long inicio, long tam, long ref);

/*-p : insere/acrescenta um ou mais membros sem compressão ao archive. Caso
o membro já exista no archive, ele deve ser substituído. Novos membros são
inseridos respeitando a ordem da linha de comando, ao final do archive;*/
void insere(FILE* file, FILE* N_membro, char* nome, long tam_c);

/*-i : insere/acrescenta um ou mais membros com compressão ao archive. Caso
o membro já exista no archive, ele deve ser substituído. Novos membros são
inseridos respeitando a ordem da linha de comando, ao final do archive;*/
void inserecomp(FILE* file, FILE* N_membro, char* nome);

/*-m membro : move o membro indicado na linha de comando para imediatamentWSe
depois do membro target existente em archive. A movimentação deve ocorrer
na seção de dados do archive;*/
void move(FILE* file, char* origem, char* destino);

/*-x : extrai os membros indicados de archive. Se os membros não forem
indicados, todos devem ser extraídos. A extração consiste em ler o membro
de archive e criar um arquivo correspondente, com conteúdo idêntico, em
disco;*/
FILE* extrai(FILE* file, char* nome);

/*-r : remove os membros indicados de archive;*/
void remover(FILE* file, char* nome);

/*-c : lista o conteúdo de archive em ordem, incluindo as propriedades de
cada membro (nome, UID, tamanho original, tamanho em disco e data de
modificação) e sua ordem no arquivo.*/
void listagem(FILE* file);

#endif