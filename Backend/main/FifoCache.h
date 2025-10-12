#pragma once
#include "../core/ICache.h"
#include "../core/core.h"  
#include <unordered_map>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class FifoCache : public ICache {
private:
    std::unordered_map<int, std::string> cache; // Armazena os textos no formato id -> conteúdo.
    std::list<int> ordemInsercao;               // Guarda a sequência em que os textos foram inseridos.
    static constexpr size_t MAX_CACHE = 10;     // Define o tamanho máximo do cache (10 textos).

public:

    // Construtor padrão (nenhuma inicialização especial).
    FifoCache() = default;

    std::string getTexto(int id) override {
        // Verifica se o texto solicitado já está no cache.
        if (cache.find(id) != cache.end()) {
            std::cout << "\n\n" << "[CACHE HIT] Texto no cache..." << "\n\n";
            // Caso esteja, imprime mensagem de acerto e retorna o texto da memória.
            return cache[id];
        }

        // Caso contrário, o texto não está no cache.
        std::cout << "\n[CACHE MISS] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se o cache já estiver cheio, remove o texto mais antigo.
        if (cache.size() == MAX_CACHE) {
            int idRemover = ordemInsercao.front();   // Obtém o ID mais antigo.
            cache.erase(idRemover);                  // Remove o conteúdo do cache.
            ordemInsercao.pop_front();               // Remove o ID da lista de ordem.
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (FIFO)." << std::endl;
        }

        // Carrega o texto solicitado do disco.
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);

        // Armazena o texto carregado no cache e registra sua ordem de inserção.
        cache[id] = textoDoDisco;
        ordemInsercao.push_back(id);

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache." << std::endl;

        // Retorna o conteúdo do texto (seja ele novo ou já existente).
        return cache[id];
    }

    std::string getNome() const override {
        // Retorna o nome do algoritmo de cache.
        return "FIFO";
    }

    void printStatus() const override {
        // Exibe o estado atual do cache no console.
        std::cout << "\n--- STATUS DO CACHE FIFO ---" << std::endl;
        std::cout << "Cache FIFO atual (do mais antigo ao mais novo):\n";

        // Percorre a lista na ordem de inserção e mostra os IDs armazenados.
        for (int id : ordemInsercao)
            std::cout << " - ID: " << id << "\n";

        std::cout << "---------------------------\n" << std::endl;
    }
};
