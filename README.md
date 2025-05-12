# Prog2-A1

Por: Guilherme Vitoriano Santana de Oliveira  - GRR:20245396

Arquivos:

- Vina.c, Arquivo onde a maior parte do código está presente;
- Vina.h, cabeçalho com as funções do vina.c;
- lz.c, compactador disponibilazado para realização do trabalho;
- lz.h, cabeçalho com as funções do lz.c;
- main.c, execução principal do código, chama as funções do vina.c para cada caso de entrada;
- Makefile, Arquivo de comandos chaves para compilação do código;
- Readme.md, este arquivo.


● uma seção descrevendo os algoritmos e as estruturas de dados
utilizadas, as alternativas de implementação consideradas e/ou
experimentadas e os motivos que o levaram a optar pela versão
entregue, as dificuldades encontradas e as maneiras pelas quais
foram contornadas.

Inicialmente começei o trabalho com um TAD de lista para guardar os membros, mas pouco tempo antes da entrega percebi que devido ao fato de ponteiros apontarem para uma posição na memória eu não posso salvar eles como membro, então decidi refazer o trabalho do zero sem o TAD de lista, pois da maneira que eu havia implementado usava um guarda com dois ponteiros, um para o início e outro para o fim da lista, e cada componente da lista tinha mais dois ponteiros, um para as informações membro e outro para o próximo elemento.


● bugs conhecidos;
- quando insere um novo arquivo ao 
Não realiza nenhuma operação com compactação;