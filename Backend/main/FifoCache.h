
#pragma once
#include "../main/CacheGuardaChuva.h"
#include <unordered_map>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FifoCache : public CacheGuardaChuva {
private:
    std::unordered_map<int, std::string> cache; // id -> conteúdo
    std::list<int> ordemInsercao;               // controle FIFO
    std::unordered_map<int, std::string> idParaCaminho;
    static constexpr size_t MAX_CACHE = 10;

public:
    FifoCache(const std::string& pastaBase = "C:/Users/SPIRIT INDIGO/Documents/Projetos/CPP/RA2/grupo10_RA2/texts/") {
        for (int i = 1; i <= 100; ++i)
            idParaCaminho[i] = pastaBase + std::to_string(i) + ".txt";
    }

    std::string getTexto(int id) override {
        // Se já estiver no cache
        if (cache.find(id) != cache.end()) {
            std::cout << "\n\n" << "TEXTO NO CACHE..." << "\n\n";
            return cache[id];
        }

        // Se cache cheio, remover o mais antigo
        if (cache.size() == MAX_CACHE) {
            int idRemover = ordemInsercao.front();
            cache.erase(idRemover);
            ordemInsercao.pop_front();
        }

        // Carregar do disco
        const std::string& path = idParaCaminho[id];
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo: " << path << "\n";
            return "";
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();

        // Inserir no cache e na fila FIFO
        cache[id] = buffer.str();
        ordemInsercao.push_back(id);

        return cache[id];
    }

    void printStatus() const override {
        std::cout << "Cache FIFO atual (do mais antigo ao mais novo):\n";
        for (int id : ordemInsercao)
            std::cout << " - ID: " << id << "\n";
    }
};