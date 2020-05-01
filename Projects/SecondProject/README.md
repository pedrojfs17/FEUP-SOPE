# Acesso informático aos Quartos de Banho

## Objetivos

- Criar programas _multithread_;
- Promver a intercomunicação entre processos através de canais com nome (_named pipes_ ou _FIFOs_);
- Evitar conflitos entre entidades concorrentes, por via de _mecanismos de sincronização_;

## Programa do cliente (_Un_)

Tal como descrito no enunciado, o programa _Un_ lança threads num intervalo curto de tempo (_5 ms_), de modo a promover uma condição de competição.

O programa funciona até terminar o tempo de execução fornecido pelo utilizador ou até o Quarto de Banho estar fechado. 
_O caso do Quarto de Banho já estar fechado quando se iniciam os pedidos dos clientes, encontra-se na situação referida anteriormente e como tal, deve-se ter o cuidado de iniciar o programa do servidor antes do programa do cliente._

Cada thread é responsável por um pedido, ou seja:
- Numa fase inicial, a thread irá compôr uma mensagem no formato
"[i,pid,tid,dur,pl]" (as variáveis são as mesmas do enunciado);
- De seguida, irá tentar abrir o FIFO público que foi fornecido pelo utilizador sendo que no caso desta comunicação não ser possível devido ao fecho do Quarto de Banho, o cliente receberá uma mensagem de que este se encontra fechado, _CLSD_. Além disso o ciclo de geração de threads de pedidos será terminado, através de uma flag;
- Estabelecendo-se uma ligação ao FIFO público, a mensagem previamente composta será escrita neste e é fechada a ligação ao FIFO público;
- Após este passo, a thread criará um FIFO privado de comunicação com o servidor, de modo a obter uma resposta deste;
- No caso da thread não conseguir obter uma resposta do servidor, o cliente receberá uma mensagem da falha, _FAILD_;
- No caso de sucesso, a thread irá analisar tanto a posição como a duração que o servidor enviou. No caso de _ambas serem -1_, é uma indicação de que o tempo de utilização do cliente irá exceder o tempo de execução do Quarto de Banho e por isso é transmitida uma mensagem de fecho ao utilizador, _CLOSD_. No caso de ser possível pôr o cliente na fila de espera de utilização, o cliente receberá uma mensagem de que entrou, _IAMIN_;
- No final, a thread irá fechar o FIFO privado e destruí-lo;


## Programa do servidor (_Qn_)

Tal como descrito no enunciado, o programa _Qn_ lança threads por cada pedido dos clientes. Inicialmente, o programa cria o FIFO público com o nome fornecido pelo utilizador. Após esta abertura, está constatemente a ler o FIFO, de modo a receber um pedido. Quando o encontrar, cria uma thread.

O programa funciona até terminar o tempo de execução fornecido pelo utilizador.

Cada thread é responsável por um atender um pedido, ou seja:
- Numa fase inicial, a thread irá extrair a mensagem de formato
"[i,pid,tid,dur,pl]", que foi enviada pelo cliente;
- De seguida, irá tentar abrir o FIFO privado que foi fornecido pelo cliente sendo que no caso desta comunicação não ser possível , o servidor acusará uma mensagem de que desistiu, _GAVUP_.
- Estabelecendo-se uma ligação ao FIFO privado, a variável responsável pelo controlo dos lugares será alterada. Esta variável está protegida por processos de sincronização, de modo a que o incremento seja sequencial;
- Após este passo, a thread vai verificar se a duração que o cliente quer não excede o tempo de execução do Quarto de Banho. No caso de não exceder, compõe uma mensagem com o mesmo formato da que recebeu, com o devido lugar na fila e duração de utilização e acusa uma mensagem de entrada do cliente, _ENTER_. No caso de exceder, a mensagem irá retornar a duração e o lugar do cliente como _-1_ e acusa a mensagem de atraso, _2LATE_;
- De seguida escreve a mensagem composta no FIFO privado e fecha-o;
- Finalmente, a thread espera o tempo da duração do cliente e, após esse intervalo, acusa a mensagem de término de tempo de utilização, _TIMUP_;

### Notas
Existe um make file no diretório principal do programa, que permite compilar os dois programas.
Alguns erros: o fecho do quarto de banho pode dar-se em 5,000000±0,000500 s; há erros no cálculo de lugares devido a falhas de comunicação entre FIFOS (só ocorre se o tempo de execução dos dois programas for igual)
É necessário dar make dentro das pastas client e server, antes de correr os dois programas, em dois terminais separados.
