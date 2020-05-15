# Acesso informático aos Quartos de Banho

## Objetivos

- Criar programas _multithread_;
- Promover a intercomunicação entre processos através de canais com nome (_named pipes_ ou _FIFOs_);
- Evitar conflitos entre entidades concorrentes, por via de _mecanismos de sincronização_;

## Programa do cliente (_Un_)

Tal como descrito no enunciado, o programa _Un_ lança threads num intervalo curto de tempo (_10 ms_), de modo a promover uma condição de competição.

O programa funciona até terminar o tempo de execução fornecido pelo utilizador ou até o Quarto de Banho estar fechado. 
_O caso do Quarto de Banho já estar fechado quando se iniciam os pedidos dos clientes, encontra-se na situação referida anteriormente e como tal, deve-se ter o cuidado de iniciar o programa do servidor antes do programa do cliente._

Cada thread é responsável por um pedido, ou seja:
- Numa fase inicial, a thread irá compôr uma mensagem no formato
"[i,pid,tid,dur,pl]" (as variáveis são as mesmas do enunciado);
- De seguida, irá tentar abrir o FIFO público que foi fornecido pelo utilizador sendo que no caso desta comunicação não ser possível devido ao fecho do Quarto de Banho, o cliente receberá uma mensagem de que este se encontra fechado, _CLSD_. Além disso o ciclo de geração de threads de pedidos será terminado, através de uma flag;
- Estabelecendo-se uma ligação ao FIFO público, a thread criará um FIFO privado de comunicação com o servidor, de modo a obter uma resposta deste;
- Após o sucesso na criação do FIFO privado, a mensagem previamente composta será escrita para o FIFO público. Se não for possível a escrita para o FIFO público, o cliente acusará uma mensagem de _FAILD_ e o programa procede como se o Quarto de Banho estivesse fechado. No final, a ligação ao FIFO público é fechada;
- No caso da thread não conseguir ler uma resposta do servidor, após 5 tentativas, o cliente receberá uma mensagem da falha, _FAILD_;
- No caso de sucesso, a thread irá analisar tanto a posição como a duração que o servidor enviou. No caso de _ambas serem -1_, é uma indicação de que o tempo de execução do Quarto de Banho terminou e por isso é transmitida uma mensagem de fecho ao utilizador, _CLOSD_. No caso de ser possível pôr o cliente na fila de espera de utilização, o cliente receberá uma mensagem de que entrou, _IAMIN_;
- No final, a thread irá fechar o FIFO privado e destruí-lo;

## Programa do servidor (_Qn_)

Tal como descrito no enunciado, o programa _Qn_ lança threads por cada pedido dos clientes. Inicialmente, o programa cria o FIFO público com o nome fornecido pelo utilizador. Após esta abertura, está constatemente a ler o FIFO, de modo a receber um pedido. Quando o encontrar, cria uma thread.

O programa funciona até terminar o tempo de execução fornecido pelo utilizador. Após este tempo, o servidor certifica-se que envia uma mesnagem a confirmar que terminou a todos os pedidos restantes de forma a que todos os pedidos recebidos sejam atendidos.

Cada thread é responsável por um atender um pedido, ou seja:
- Numa fase inicial, a thread irá extrair a mensagem de formato "[i,pid,tid,dur,pl]", que foi enviada pelo cliente, _RECVD_;
- Após a leitura, a variável responsável pelo controlo dos lugares será alterada. Esta variável está protegida por processos de sincronização, de modo a que o incremento seja sequencial. No caso dos lugares serem limitados, o cliente ocupará um lugar numa fila;
- De seguida, irá tentar abrir o FIFO privado que foi fornecido pelo cliente sendo que no caso desta comunicação não ser possível, após 5 tentativas, o servidor acusará uma mensagem de que desistiu, _GAVUP_.
- Após este passo, a thread vai verificar se o tempo em que o cliente fez o pedido não excedeu o tempo de execução do Quarto de Banho. No caso de não exceder, compõe uma mensagem com o mesmo formato da que recebeu, com o devido lugar na fila e duração de utilização e acusa uma mensagem de entrada do cliente, _ENTER_. No caso de exceder, a mensagem irá retornar a duração e o lugar do cliente como _-1_ e acusa a mensagem de atraso, _2LATE_;
- De seguida tenta escrever a mensagem composta no FIFO privado. Se não o conseguir, o servidor acusa uma mensagem de _GAVUP_ e, no caso de haver um número máximo de lugares, desocupa o lugar do cliente;
- Finalmente, a thread espera o tempo da duração do cliente e, após esse intervalo, acusa a mensagem de término de tempo de utilização, _TIMUP_. Se houver um número máximo de lugares, o servidor desocupa o lugar;

### Notas sobre o funcionamento do programa

Existe um makefile no diretório principal do programa, que permite compilar os dois programas, simultaneamente.

Através da nossa interpretação do enunciado, o pedido de um cliente é atendido, independentemente do seu tempo de duração, se este foi feito anterior ao fecho da casa de banho (semelhante ao funcionamento de uma loja, quando um cliente entra antes de fechar). Assim, o FIFO só será encerrado após responder a todos os pedidos feitos até ao seu fecho.

Durante o desenvolvimento do programa, foi difícil testar se as funcionalidades estavam a ser executadas corretamente, visto que erros que apareciam a um colega de trabalho não apareciam ao outro e vice-versa. Tempos de execução, número de threads lançadas, posicionamento dos clientes, número de tentativas de leitura/escrita para um FIFO de modo a que funcionasse corretamente foram alguns dos problemas encontrados.

O tempo de execução do Quarto de Banho ou lançamento de pedidos pode variar com uma amplitude de ±0,005 s.

Recorremos à implementação de uma _queue_ de modo a facilitar a situação em que há limite de lugares. (https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/)
