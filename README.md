# grupo10_RA2
## Aplicativo de leitura de textos no terminal do Windows que minimize o tempo de carregamento de textos físicos em memória utilizando três sistemas de cache.

## Algoritmo FIFO 
Ele mantém os textos em uma **fila de inserção**, e quando o cache atinge sua capacidade máxima, o **primeiro texto que foi adicionado é removido** para dar espaço ao novo.

### Funcionamento
1. Quando um texto é solicitado:
   - Se ele já está no cache, ocorre um *cache hit* e o texto é retornado da memória.
   - Caso contrário (*cache miss*), ele é adicionado ao cache e então lido.
2. Se o cache estiver cheio, o texto mais antigo é removido.
3. A ordem de inserção é controlada por uma lista (`std::list<int>`).
4. O conteúdo de cada texto é armazenado em um `unordered_map` no formato `id -> conteúdo`.

### Testes 
Os testes realizados foram simples, primeiramente o cache de 10 textos foi preenchido e o algoritmo escolhido foi o FIFO, após preenchido ao escolher um texto não presente no cache ele corretamente remove o primeiro elemento inserido no cache, o que é controlado internamente por uma lista. Ao selecionar textos dentro do cache ele corretamente aponta cache hit e mostra na tela.

## Algoritmo Random Replacement
O algoritmo Random Replacement (RR) remove um elemento aleatório do cache quando a capacidade é atingida, é completamente pseudo-aleatório

### Funcionamento
1. Quando um texto é solicitado:
   - Se ele já está no cache, ocorre um *cache hit* e o texto é retornado da memória.
   - Caso contrário (*cache miss*), o texto é lido do disco.
2. Se o cache estiver cheio, um texto aleatório é escolhido e removido.
3. O controle dos IDs armazenados é feito por um vetor (`std::vector<int>`), e a seleção do item a ser removido utiliza um gerador aleatório (`std::mt19937` + `std::uniform_int_distribution`).

### Testes 
Novamente o cache de 10 textos foi preenchido e o algoritmo escolhido desta vez foi o RR, após preenchido ao escolher um texto não presente no cache ele corretamente utiliza o gerador pseudoaleatório para gerar um id entre os presentes no cache, e após gerado ele remove o texto com este indíce. Ao selecionar textos dentro do cache ele corretamente aponta cache hit e mostra na tela.

