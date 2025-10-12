#pragma once
// Garante que o header seja incluído apenas uma vez por unidade de compilação.
// Evita problemas de redefinição quando o arquivo é incluído múltiplas vezes.

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
    //  - segundo: o conteúdo do texto (std::string) e um iterator para a posição do id dentro de ordemUso (std::list<int>::iterator)
    std::unordered_map<int, std::pair<std::string, std::list<int>::iterator>> cache;

    // Lista que mantém a ordem de uso
    //  - início (front) = mais recentemente usado
    //  - final  (back)  = menos recentemente usado (candidato à remoção)
    std::list<int> ordemUso; 
    static constexpr size_t MAX_CACHE = 10; // Tamanho máximo do cache. static constexpr => constante em tempo de compilação.

public:
    LruCache() = default; // Construtor padrão da classe LruCache

    // Função que faz a busca e retorno do texto utilizando o id do texto informado pelo usuário
    // Override pois está sobrescrevendo uma função virtual definida na classe base (pai) ICache
    std::string getTexto(int id) override {
        auto itemHM = cache.find(id); // Procura no mapa Hash se encontra a chave id. .find() retorna o item solicitado do hash map
        if (itemHM != cache.end()) { // cache.end() é o iterador para caso não exista a chave id no mapa Hash. Se não existir, é um CACHE MISS
            std::cout << "\n\n[TEXTO NO CACHE - LRU HIT]\n\n";

            // Atualiza a ordem de uso: move o item para o topo da lista
            ordemUso.erase(itemHM->second.second); // .erase() remove da lista da ordem de uso o id encontrado. Utiliza o ponteiro do iterador do mapa Hash
            ordemUso.push_front(id); // Coloca o id do texto na primeira posição da lista de ordem de uso
            itemHM->second.second = ordemUso.begin(); // Atualiza o iterador que apontava para a posição do id antigo na lista de ordem de uso para a nova posição

            return itemHM->second.first; // Retorna o conteúdo (texto) do mapa hash atualizado
        }

        // CACHE MISS, ou seja, não encontrou no map o id do texto
        std::cout << "\n[CACHE MISS - LRU] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se cache cheio, remover o menos recentemente usado (final da lista)
        if (cache.size() == MAX_CACHE) { // .size() verifica o tamanho do cache (quantos textos estão armazenados)
            int idRemover = ordemUso.back(); // Armazena na variável idRemover o elemento da última posição da lista (o menos usado)
            ordemUso.pop_back(); // Elimina da lista o elemento da última posição
            cache.erase(idRemover); // Apaga do cache map o id do texto removido
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (LRU - menos usado)." << std::endl;
        }

        // Carrega do disco o texto com o id solicitado
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);

        // Insere no cache e registra como mais recentemente usado
        ordemUso.push_front(id); // Insere na primeira posição da lista de ordem de uso o id do novo texto adicionado ao cache
        cache[id] = { textoDoDisco, ordemUso.begin() }; // Insere no cache na posição do id do texto, o valor do texto carregado e um ponteiro a primeira posição da lista da ordem de uso 

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache via LRU." << std::endl;

        return cache[id].first; // Retorna o texto recém inserido
    }
    
    // Função que retorna o nome padrão do algortimo de cache utilizado
    std::string getNome() const override {
        return "LRU";
    }

    // Função que mostra o status atual do cache 
    void printStatus() const override {
        std::cout << "\n------------------- STATUS DO CACHE LRU -------------------" << std::endl;
        std::cout << "Cache LRU atual (do mais recente ao menos recente):\n";
        for (int id : ordemUso) // Itera sobre cada elemento da lista de ordem de uso, mostrando do mais recente ao mais antigo utilizado
            std::cout << " - Texto Numero " << id << "\n";
        std::cout << "-----------------------------------------------------------\n" << std::endl;
    }
};

