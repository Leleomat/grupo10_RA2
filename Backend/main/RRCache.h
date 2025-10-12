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
// Biblioteca C++ para gera��o de n�meros aleat�rios (std::mt19937, std::uniform_int_distribution).

class RRCache : public ICache {
private:
    std::unordered_map<int, std::string> cache;
    // Guarda pares (id -> conte�do). leitura/consulta: O(1).

    //Armazena os IDs dos textos
    std::vector<int> idsCache;
    // Mant�m a lista dos ids atualmente no cache para indexa��o aleat�ria.
    // Remover do meio do vetor � O(n)

    static constexpr size_t MAX_CACHE = 10;
    // Capacidade do cache (constante em tempo de compila��o). Segue o requisito do trabalho.

    //Gerador aleat�rio
    std::mt19937 rng{ std::random_device{}() };

public:
    RRCache() = default;
    // Construtor padr�o � tudo inicializado pelos membros.

    std::string getTexto(int id) override {
        // Fun��o principal: retorna o texto solicitado, carregando no cache se necess�rio.

        // Se o texto j� estiver no cache
        auto it = cache.find(id);
        // it aponta para elemento se existe, cache.end() caso contr�rio.

        if (it != cache.end()) {
            std::cout << "\n\n" << "[CACHE HIT] Texto no cache..." << "\n\n";
            // Log informativo � indica cache hit.
            return it->second;
            // Retorna diretamente a string armazenada (n�o l� do disco).
        }

        std::cout << "\n[CACHE MISS] O texto " << id << " nao foi encontrado. Solicitando ao Core..." << std::endl;
        // Log de miss � aqui seria o local para incrementar contador de miss/time measurement.

        // Se cache cheio, remove um elemento aleat�rio
        if (cache.size() == MAX_CACHE) {
            // Escolhe �ndice aleat�rio no intervalo v�lido [0, idsCache.size()-1]
            std::uniform_int_distribution<size_t> dist(0, idsCache.size() - 1);
            size_t indiceRemover = dist(rng);
            // Gera o �ndice pseudo-aleat�rio.
            int idRemover = idsCache[indiceRemover];
            // Recupera o id correspondente no vetor.

            cache.erase(idRemover);
            // Remove da tabela hash � O(1)

            idsCache.erase(idsCache.begin() + indiceRemover);
            // Remove do vetor por posi��o: custo O(n) deslocando elementos subsequentes.
            std::cout << "[CACHE INFO] Cache cheio. Removendo o texto " << idRemover << " (Random Replacement)." << std::endl;
            // Log informativo de remo��o.
        }

        // Carrega o texto do disco
        std::string textoDoDisco = Disco::lerTextoDoDisco(id);
        // Chamada delegada ao m�dulo Disco (Core).

        // Insere no cache e registra ele
        cache[id] = textoDoDisco;
        // Insere ou substitui a entrada no unordered_map (c�pia/move da string).

        idsCache.push_back(id);
        // Adiciona o id ao vetor para futuras sele��es aleat�rias.

        std::cout << "\n[CACHE] O texto " << id << " foi adicionado ao Cache via RR." << std::endl;
        // Log informando inser��o.

        return cache[id];
        // Retorna o conte�do rec�m-inserido.
    }

    std::string getNome() const override {
        return "Random Replacement (RR)";
        // Nome da pol�tica
    }

    void printStatus() const override {
        std::cout << "\n-------------------- STATUS DO CACHE RR -------------------" << std::endl;
        std::cout << "Cache RR atual (ordem n�o significativa):\n";
        for (int id : idsCache)
            std::cout << " - Texto Numero " << id << "\n";
        std::cout << "------------------------------------------------------------\n" << std::endl;
        // Imprime o estado atual do cache (�til para debug). Note que a ordem n�o tem significado sem�ntico.
    }
};
