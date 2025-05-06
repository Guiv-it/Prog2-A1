#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz.h"
#include "vina.h"

void move_bloco(long inicio, long final, long ref){
}


struct diretorio* criadir(){
    struct diretorio* dir = malloc(sizeof(dir));
    

}

/*-p : insere/acrescenta um ou mais membros sem compressão ao archive. Caso
o membro já exista no archive, ele deve ser substituído. Novos membros são
inseridos respeitando a ordem da linha de comando, ao final do archive;*/
void insere(){
}

/*-i : insere/acrescenta um ou mais membros com compressão ao archive. Caso
o membro já exista no archive, ele deve ser substituído. Novos membros são
inseridos respeitando a ordem da linha de comando, ao final do archive;*/
void inserecomp(){
}

/*-m membro : move o membro indicado na linha de comando para imediatamente
depois do membro target existente em archive. A movimentação deve ocorrer
na seção de dados do archive;*/
void move(){

}

/*-x : extrai os membros indicados de archive. Se os membros não forem
indicados, todos devem ser extraídos. A extração consiste em ler o membro
de archive e criar um arquivo correspondente, com conteúdo idêntico, em
disco;*/
void unpack(){

}

/*-r : remove os membros indicados de archive;*/
void remove(){

}

/*-c : lista o conteúdo de archive em ordem, incluindo as propriedades de
cada membro (nome, UID, tamanho original, tamanho em disco e data de
modificação) e sua ordem no arquivo.*/
void list(){

}
