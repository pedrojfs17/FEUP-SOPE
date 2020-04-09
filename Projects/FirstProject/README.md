# Ferramenta para estimar a utilização de espaço em disco

Trabalho desenvolvido de forma a simular o comportamento o comando `du` da consola de Linux.

## Flags

| Flag                       | Descrição                                                 |
| :-------------------------:|:---------------------------------------------------------:|
| -a ou --all                | Mostra ficheiros                                          |
| -b ou --bytes              | Apresenta o tamanho em bytes                              |
| -B [SIZE] --block.size=SIZE| Define o tamanho dos blocos                               |
| -l ou --count-links        | Contabiliza múltiplas vezes o mesmo ficheiro              |
| -L ou --dereference        | Segue links simbólicos                                    |
| -S ou --separate-dirs      | Não inclui o tamanho de subdiretórios                     |
| -max-depth=N               | Limita a informação apresentada a N níveis de profundidade|

###### A flag -l/--count-links encontra-se sempre ativa por predefinição

## Funcionalidades implementadas

O programa desenvolvido apresenta todas as funcionalidades do comando `du`, com algumas modificações:

- Não é permitida a repetição de flags, por exemplo, `./simpledu -a -a` não é aceite pelo programa;
- A execução do programa cria um registo "log.txt", onde é possível analisar a sua execução;
- É possível interromper a execução do programa com **CTRL+C** e, posteriormente, retomar ou terminar a execução deste;

## Detalhes

Inicialmente, o programa estava estruturado de forma a usar `fork()` e `execv()`, no entanto, devido a alguns problemas,
decidimos reestruturá-lo de modo a usar `fork()` com recursividade, diminuindo assim a complexidade.