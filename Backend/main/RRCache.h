#pragma once
#include "../core/ICache.h"
#include "../core/core.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

class RRCache : public ICache {
private:
    std::unordered_map<int, std::string> cache;
    //Armazena os IDs dos textos
    std::vector<int> idsCache;
    static constexpr size_t MAX_CACHE = 10;
    //Gerador aleatório
    std::mt19937 rng{ std::random_device{}() };

public:
    RRCache() = default;

    std::string getTexto(int id) override {
        // Se o texto já estiver no cache
        auto it = cache.find(id);
        if (it != cache.end()) {
            std::cout << "\n\n" << "[CACHE HIT] Texto no cache..." << "\n\n";
            return it->second;
        }

        std::cout << "\n[CACHE MISS] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;

        // Se cache cheio, remove um elemento aleatório
        if (cache.size() == MAX_CACHE) {
            std::uniform_int_distribution<size_t> dist(0, idsCache.size() - 1);
            size_t indiceRemover = dist(rng);
            int idRemover = idsCache[indiceRemover];

            cache.erase(idRemover);
            idsCache.erase(idsCache.begin() + indiceRemover);

            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (Random Replacement)." << std::endl;
        }

        // Carrega o texto do disco
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);

        // Insere no cache e registra ele
        cache[id] = textoDoDisco;
        idsCache.push_back(id);

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache." << std::endl;

        return cache[id];
    }

    std::string getNome() const override {
        return "Random Replacement (RR)";
    }

    void printStatus() const override {
        std::cout << "\n--- STATUS DO CACHE RR ---" << std::endl;
        std::cout << "Cache RR atual (ordem não significativa):\n";
        for (int id : idsCache)
            std::cout << " - ID: " << id << "\n";
        std::cout << "---------------------------\n" << std::endl;
    }
};
