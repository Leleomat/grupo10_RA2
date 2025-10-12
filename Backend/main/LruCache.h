#pragma once
// Garante que o header seja inclu�do apenas uma vez por unidade de compila��o.
// Evita problemas de redefini��o quando o arquivo � inclu�do m�ltiplas vezes.

#include "../core/ICache.h" // Inclui a interface ICache
#include "../core/core.h" // Inclui funcionalidades do "Core" do projeto
#include <unordered_map> // Mapa hash para acessar rapidamente itens do cache por id
#include <list> // Lista duplamente ligada para manter a ordem de uso
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class LruCache : public ICache {
private:
    // Cada entrada no unordered_map armazena:
    //  - primeiro: o id do mapa Hash
    //  - segundo: o conte�do do texto (std::string) e um iterator para a posi��o do id dentro de ordemUso (std::list<int>::iterator)
    std::unordered_map<int, std::pair<std::string, std::list<int>::iterator>> cache;

    // Lista que mant�m a ordem de uso
    //  - in�cio (front) = mais recentemente usado
    //  - final  (back)  = menos recentemente usado (candidato � remo��o)
    std::list<int> ordemUso; 
    static constexpr size_t MAX_CACHE = 10; // Tamanho m�ximo do cache. static constexpr => constante em tempo de compila��o.

public:
    LruCache() = default; // Construtor padr�o da classe LruCache

    // Fun��o que faz a busca e retorno do texto utilizando o id do texto informado pelo usu�rio
    // Override pois est� sobrescrevendo uma fun��o virtual definida na classe base (pai) ICache
    std::string getTexto(int id) override {
        auto itemHM = cache.find(id); // Procura no mapa Hash se encontra a chave id. .find() retorna o item solicitado do hash map
        if (itemHM != cache.end()) { // cache.end() � o iterador para caso n�o exista a chave id no mapa Hash. Se n�o existir, � um CACHE MISS
            std::cout << "\n\n[TEXTO NO CACHE - LRU HIT]\n\n";

            // Atualiza a ordem de uso: move o item para o topo da lista
            ordemUso.erase(itemHM->second.second); // .erase() remove da lista da ordem de uso o id encontrado. Utiliza o ponteiro do iterador do mapa Hash
            ordemUso.push_front(id); // Coloca o id do texto na primeira posi��o da lista de ordem de uso
            itemHM->second.second = ordemUso.begin(); // Atualiza o iterador que apontava para a posi��o do id antigo na lista de ordem de uso para a nova posi��o

            return itemHM->second.first; // Retorna o conte�do (texto) do mapa hash atualizado
        }

        // CACHE MISS, ou seja, n�o encontrou no map o id do texto
        std::cout << "\n[CACHE MISS - LRU] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se cache cheio, remover o menos recentemente usado (final da lista)
        if (cache.size() == MAX_CACHE) { // .size() verifica o tamanho do cache (quantos textos est�o armazenados)
            int idRemover = ordemUso.back(); // Armazena na vari�vel idRemover o elemento da �ltima posi��o da lista (o menos usado)
            ordemUso.pop_back(); // Elimina da lista o elemento da �ltima posi��o
            cache.erase(idRemover); // Apaga do cache map o id do texto removido
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (LRU - menos usado)." << std::endl;
        }

        // Carrega do disco o texto com o id solicitado
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);

        // Insere no cache e registra como mais recentemente usado
        ordemUso.push_front(id); // Insere na primeira posi��o da lista de ordem de uso o id do novo texto adicionado ao cache
        cache[id] = { textoDoDisco, ordemUso.begin() }; // Insere no cache na posi��o do id do texto, o valor do texto carregado e um ponteiro a primeira posi��o da lista da ordem de uso 

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache via LRU." << std::endl;

        return cache[id].first; // Retorna o texto rec�m inserido
    }
    
    // Fun��o que retorna o nome padr�o do algortimo de cache utilizado
    std::string getNome() const override {
        return "LRU";
    }

    // Fun��o que mostra o status atual do cache 
    void printStatus() const override {
        std::cout << "\n------------------- STATUS DO CACHE LRU -------------------" << std::endl;
        std::cout << "Cache LRU atual (do mais recente ao menos recente):\n";
        for (int id : ordemUso) // Itera sobre cada elemento da lista de ordem de uso, mostrando do mais recente ao mais antigo utilizado
            std::cout << " - Texto Numero " << id << "\n";
        std::cout << "-----------------------------------------------------------\n" << std::endl;
    }
};

