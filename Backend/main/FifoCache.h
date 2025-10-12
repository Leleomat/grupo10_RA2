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
    std::unordered_map<int, std::string> cache; // id -> conteúdo
    std::list<int> ordemInsercao;               // controle FIFO
    static constexpr size_t MAX_CACHE = 10;

public:
    
    // A responsabilidade de achar a pasta base é do Core
    FifoCache() = default;

    std::string getTexto(int id) override {
        // Se já estiver no cache
        if (cache.find(id) != cache.end()) {
            std::cout << "\n\n" << "TEXTO NO CACHE..." << "\n\n";
            return cache[id];
        }

        std::cout << "\n[CACHE MISS] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se cache cheio, remover o mais antigo
        if (cache.size() == MAX_CACHE) {
            int idRemover = ordemInsercao.front();
            cache.erase(idRemover);
            ordemInsercao.pop_front();
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (FIFO)." << std::endl;
        }

        // Carregar do disco
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);
        
        // Inserir no cache e na fila FIFO
        cache[id] = textoDoDisco;
        ordemInsercao.push_back(id);

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache." << std::endl;

        return cache[id];
    }

    std::string getNome() const override {
        return "FIFO";
    }

    void printStatus() const override {
        std::cout << "\n--- STATUS DO CACHE FIFO ---" << std::endl;
        std::cout << "Cache FIFO atual (do mais antigo ao mais novo):\n";
        for (int id : ordemInsercao)
            std::cout << " - ID: " << id << "\n";
        std::cout << "---------------------------\n" << std::endl;
    }
};