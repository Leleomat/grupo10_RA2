#pragma once
#include "../core/ICache.h"
#include "../core/core.h"  
#include <unordered_map>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class LruCache : public ICache {
private:
    // Cada item do cache: id -> (conteúdo + posição na lista de uso)
    std::unordered_map<int, std::pair<std::string, std::list<int>::iterator>> cache;
    std::list<int> ordemUso; // do mais recente ao menos recente
    static constexpr size_t MAX_CACHE = 10;

public:
    LruCache() = default;

    std::string getTexto(int id) override {
        // Se o texto já estiver no cache (HIT)
        auto it = cache.find(id);
        if (it != cache.end()) {
            std::cout << "\n\n[TEXTO NO CACHE - LRU HIT]\n\n";

            // Atualiza a ordem de uso: move o item para o topo da lista
            ordemUso.erase(it->second.second);
            ordemUso.push_front(id);
            it->second.second = ordemUso.begin();

            return it->second.first;
        }

        // CACHE MISS
        std::cout << "\n[CACHE MISS - LRU] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se cache cheio, remover o menos recentemente usado (final da lista)
        if (cache.size() == MAX_CACHE) {
            int idRemover = ordemUso.back();
            ordemUso.pop_back();
            cache.erase(idRemover);
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (LRU - menos usado)." << std::endl;
        }

        // Carregar do disco
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);

        // Inserir no cache e registrar como mais recentemente usado
        ordemUso.push_front(id);
        cache[id] = { textoDoDisco, ordemUso.begin() };

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache LRU." << std::endl;

        return cache[id].first;
    }

    std::string getNome() const override {
        return "LRU";
    }

    void printStatus() const override {
        std::cout << "\n------------------- STATUS DO CACHE LRU -------------------" << std::endl;
        std::cout << "Cache LRU atual (do mais recente ao menos recente):\n";
        for (int id : ordemUso)
            std::cout << " - Texto Numero " << id << "\n";
        std::cout << "-----------------------------------------------------------\n" << std::endl;
    }
};

