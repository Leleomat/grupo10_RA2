# grupo9_RA2

## Aplicativo de leitura de textos no terminal do Windows que minimize o tempo de carregamento de textos físicos em memória utilizando três sistemas de cache.

## Main
É o coração do projeto, contém a interface que acopla os diferentes tipos de cache, além do laço principal da aplicação que interage com o usuário no terminal, lendo o número do texto desejado e tratando as entradas especiais.

Ao rodar pela primeira vez, será necessário rodar o Modo de Simulação para decidir qual o melhor algoritmo para armazenar os textos em cache, após isso, deve-se fechar o programa e iniciá-lo novamente, agora o algoritmo de cache estará ativo e o programa perguntará se você:
* Gostaria de ver determinado texto a partir do algoritmo de cache atual, perguntando qual o número que identifica o texto desejado;
* Caso a entrada do terminal esteja entre 1 e 100, o programa abre o texto para leitura e fica aguardando o número de um novo texto ou de um novo comando;
* Se o número for zero o programa é encerrado;
* Se o número for -1 o programa entra em modo de simulação, que determinará qual algoritmo de cache será usado pelo programa na próxima vez que ele for inicializado.

## Core
É a estrutura base do projeto, inclui a leitura e armazenamento dos 100 textos a partir do disco (armazenamento lento), retornando o conteúdo lido ao algoritmo de cache que o solicitou.

## Algoritmo FIFO 
Ele mantém os textos em uma **fila de inserção**, e quando o cache atinge sua capacidade máxima, o **primeiro texto que foi adicionado é removido** para dar espaço ao novo.

### Funcionamento
1. Quando um texto é solicitado:
   - Se ele já está no cache, ocorre um *cache hit* e o texto é retornado da memória.
   - Caso contrário (*cache miss*), ele é adicionado ao cache e então lido.
2. Se o cache estiver cheio, o texto mais antigo é removido.
3. A ordem de inserção é controlada por uma lista (`std::list<int>`).
4. O conteúdo de cada texto é armazenado em um `unordered_map` no formato `id -> conteúdo`.

### Algoritmo FIFO: Testes
Os testes realizados foram simples, primeiramente o cache de 10 textos foi preenchido e o algoritmo escolhido foi o FIFO, após preenchido ao escolher um texto não presente no cache ele corretamente remove o primeiro elemento inserido no cache, o que é controlado internamente por uma lista. Ao selecionar textos dentro do cache ele corretamente aponta cache hit e mostra na tela.

## Algoritmo Random Replacement
O algoritmo Random Replacement (RR) remove um elemento aleatório do cache quando a capacidade é atingida, é completamente pseudo-aleatório

### Funcionamento
1. Quando um texto é solicitado:
   - Se ele já está no cache, ocorre um *cache hit* e o texto é retornado da memória.
   - Caso contrário (*cache miss*), o texto é lido do disco.
2. Se o cache estiver cheio, um texto aleatório é escolhido e removido.
3. O controle dos IDs armazenados é feito por um vetor (`std::vector<int>`), e a seleção do item a ser removido utiliza um gerador aleatório (`std::mt19937` + `std::uniform_int_distribution`).

### Algoritmo RR: Testes
Novamente o cache de 10 textos foi preenchido e o algoritmo escolhido desta vez foi o RR, após preenchido ao escolher um texto não presente no cache ele corretamente utiliza o gerador pseudoaleatório para gerar um id entre os presentes no cache, e após gerado ele remove o texto com este indíce. Ao selecionar textos dentro do cache ele corretamente aponta cache hit e mostra na tela.

## Algoritmo LRU
Este algoritmo faz a implementação do Least Recently Used (LRU). A ideia central desse algoritmo é gerenciar o armazenamento de textos no cache (limite de tamanho de 10 textos) através de uma lista de usos. Nesse caso, sempre ao acessar um texto já no cache (CACHE HIT) é adicionado na lista de ordem de uso na posição 1 o ID do texto buscado. Isso também serve para novas inserções no cache (CACHE MISS), onde também será adicionado como último texto usado. 

### Funcionamento
**Exemplo de uso**
* CACHE MISS texto 3 -> Adicionar texto 3 no Cache
  * Lista de uso:
  3
* CACHE MISS texto 4 -> Adicionar texto 4 no Cache
  * Lista de uso:
  4
  3
* CACHE MISS texto 5 -> Adicionar texto 5 no Cache
  * Lista de uso:
  5
  4
  3
* CACHE HIT texto 3 -> Ler texto 3 do Cache
  * Lista de uso:
  3
  5
  4

Como uma opção da otimização do tempo, foi utilizado uma estrutura de busca O(1) através de um Hash Map com um ponteiro para a lista da ordem de uso. Desse modo, a lista de ordem de uso sempre estará diretamente ligada ao mapeamento Hash, reduzindo a complexidade da pesquisa.  

### Algoritmo LRU: Teste
Dentro da classe LruCache, existe a função printstatus() responsável por mostrar ao usuário após cada intereação com o CACHE como está a situação da lista de ordem de uso. Desse modo, para teste do algoritmo, é alterado a sequência do código para que ao invés de iniciar no modo simulação, acesse diretamente o algoritmo LRU. Para fazer isso, adicione no início da função main() do arquivo da main o seguinte código:
```
CachePtr cacheAtual = std::make_shared<LruCache>();
```
Com isso, o programa iniciará diretamente com o algoritmo LRU para ser feito os testes. Inicialmente, são colocados vários IDs de textos aleatórios até preencher o CACHE com o limite de 10. Após, e feito a leitura de alguns que já estão no CACHE, e sempre após cada iteração é mostrado a lista de ordem de uso. Com a lista cheia, ao digitar o ID de algum texto que antes não estava no cache, é feito um CACHE MISS, removendo o texto com o ID último da lista de uso e adicionando o novo no começo. 

## Estrutura ICache 
A estrutura é relativamente simples porém essencial, ela é a classe genérica que é implementada nos algoritmos que herdam e implementam seus métodos, como o FIFO, Random Replacement e LRU, implementando o polimorfismo.

## Modo de Simulação
Para acessar o Modo de Simulação do projeto basta confirmar a entrada "-1" sem as aspas na interface principal. A principal função desse Modo de Simulação é indicar qual o algoritmo de cache mais eficiente dentre as implementadas (FIFO, RR e LRU), que será usado pelo programa na próxima vez que ele for inicializado.

Este modo simula o acesso de 3 usuários aleatórios por algoritmo de cache, onde cada usuário faz ao menos 200 solicitações de arquivos, e cada solicitação utiliza 3 modos diferentes de escolha do arquivo:
1. aleatório, puro e simples;
2. aleatório com distribuição de Poisson e;
3. aleatório de forma que textos numerados entre 30 e 40 tenham 43% de chance de serem sorteados.

Ou seja, cada algoritmo recebe 600 solicitações (200 de cada usuário/modelo de escolha de arquivo), totalizando ao final da simulação 1800 solicitações realizadas em conjunto pelos três algoritmos de cache. 

Ao final da simulação é informado:
* Cada um dos usuários;
* Cada um dos algoritmos de cache; 
* O tempo gasto entre a solicitação do arquivo e a apresentação deste arquivo no terminal (cache hit e time);
* O número de vezes que um determinado arquivo foi solicitado e não estava no cache (cache miss e time) considerando os usuários aleatórios e os modos de sorteio dos arquivos;
* Um conjunto de gráficos formatados em ASCII para apresentar as grandezas necessárias a tomada de decisão sobre a escolha do algoritmo de cache:
1. *Gráfico de resumo de performance, que abrange a média do tempo gasto de cada usuário em cada algoritmo;*
2. *Gráfico das médias de tempo gasto de cada algoritmo;*
3. *Gráfico de Hits e Misses de cada algoritmo;*
4. *Gráfico de Hits e Misses de cada usuário por algoritmo.*
* Relatório Final comparando os tempos médios de cada algoritmo ao fim da simulação, e declarando qual o vencedor que será utilizado na próxima inicialização.
