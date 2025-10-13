
#include "../main/simulation.h" // Inclui a própria declaração
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <numeric>
#include <map>

// Includes para os tipos de cache
#include "../main/FifoCache.h"
#include "../main/LruCache.h"
#include "../main/RRCache.h"
#include "../core/ICache.h"

// Alias para facilitar
using CachePtr = std::shared_ptr<ICache>;
using Texto = std::string;

// Funções de Geração Aleatória
int aleatorioPuro(int min, int max, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

int aleatorioPoisson(double lambda, std::mt19937& rng) {
    std::poisson_distribution<int> dist(lambda);
    return dist(rng);
}

int aleatorioPeso(int min, int max, std::mt19937& rng) {
    std::uniform_real_distribution<double> chance(0.0, 1.0);
    if (chance(rng) < 0.43) {
        std::uniform_int_distribution<int> dist(30, 40);
        return dist(rng);
    }
    else {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
}

// Função de simulação de usuário
ResultadoUsuario simularUsuario(CachePtr cache, int usuarioId) {
    ResultadoUsuario resultado;
    resultado.id_usuario = usuarioId;

    std::mt19937 rng(std::random_device{}());
    const int numSolicitacoes = 15; // 15 por Usuario por algoritmo para teste

    for (int i = 0; i < numSolicitacoes; ++i) {
        int idTexto;
        std::string modo_sorteio;

        if (i < numSolicitacoes / 3) {
            idTexto = aleatorioPuro(1, 100, rng);
            modo_sorteio = "Puro";
        }
        else if (i < (numSolicitacoes * 2) / 3) {
            idTexto = aleatorioPoisson(50.0, rng);
            if (idTexto < 1) idTexto = 1; if (idTexto > 100) idTexto = 100;
            modo_sorteio = "Poisson";
        }
        else {
            idTexto = aleatorioPeso(1, 100, rng);
            modo_sorteio = "Ponderado";
        }

        auto inicio = std::chrono::high_resolution_clock::now();
        Texto texto = cache->getTexto(idTexto);
        auto fim = std::chrono::high_resolution_clock::now();

        auto duracao_us = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count();
        bool foi_hit = duracao_us < 125; // Menor que 125ms significa que está no cache, pois para puxar do disco sempre demora mais de 200ms

        // Guarda os dados detalhados da solicitação
        resultado.solicitacoes.push_back({ idTexto, duracao_us, foi_hit, modo_sorteio });

        // Atualiza os contadores agregados
        if (foi_hit) resultado.total_hits++;
        else resultado.total_misses++;
    }

    return resultado;
}

// Relatorio 
void exibirRelatorioFinal(const std::vector<ResultadoAlgoritmo>& todos_resultados) {
    std::cout << "\n\n=========================================================\n";
    std::cout << "=== RELATORIO FINAL DA SIMULACAO ===" << std::endl;

    for (const auto& resultado_algo : todos_resultados) {
        std::cout << "\n=========================================================\n";
        std::cout << "[" << resultado_algo.nome_algoritmo << "]" << std::endl;

        // 1. Imprime o log detalhado de cada usuário
        for (const auto& resultado_user : resultado_algo.resultados_por_usuario) {
            std::cout << "\n[USER " << resultado_user.id_usuario << "]" << std::endl;
            for (size_t i = 0; i < resultado_user.solicitacoes.size(); ++i) {
                const auto& s = resultado_user.solicitacoes[i];
                std::cout << "[Solicitacao " << i + 1 << ": Arquivo " << s.id_texto << ".txt"
                    << " // Modo de sorteio: " << s.modo_sorteio
                    << " // Tempo: " << s.tempo_ms << " ms"
                    << " // " << (s.foi_hit ? "HIT" : "MISS") << "]" << std::endl;
            }
        
            // Adicionando o sumário por usuário que você tinha antes
            double tempo_total = 0;
            for (const auto& s : resultado_user.solicitacoes) {
                tempo_total += s.tempo_ms;
            }
            std::cout << "\n[SUMARIO USER " << resultado_user.id_usuario << " (" << resultado_algo.nome_algoritmo << ")]" << std::endl;
            std::cout << " -> Total Hits: " << resultado_user.total_hits << " | Total Misses: " << resultado_user.total_misses << std::endl;
            std::cout << " -> Tempo medio por solicitacao: " << tempo_total / resultado_user.solicitacoes.size() << " ms" << std::endl;
        }
        // 2. Calcula e imprime o resumo de hits/misses por arquivo
        std::map<int, std::pair<int, int>> stats_por_arquivo; // id -> {hits, misses}
        for (const auto& resultado_user : resultado_algo.resultados_por_usuario) {
            for (const auto& s : resultado_user.solicitacoes) {
                if (s.foi_hit) {
                    stats_por_arquivo[s.id_texto].first++;
                }
                else {
                    stats_por_arquivo[s.id_texto].second++;
                }
            }
        }

        std::cout << "\n[SUMARIO GERAL DO ALGORITMO: " << resultado_algo.nome_algoritmo << " (Todos os Usuarios)]" << std::endl;
        std::cout << "\n[Arquivos Texto:]" << std::endl;
        for (const auto& par : stats_por_arquivo) {
            int id = par.first;
            int hits = par.second.first;
            int misses = par.second.second;
            std::cout << "[" << id << ".txt: " << hits << " Hit // " << misses << " Miss]" << std::endl;
        }
    }
}


// Função principal do simulador
void executarSimulacao() {
    std::vector<ResultadoAlgoritmo> todos_os_resultados;
    const std::vector<std::string> nomesCache = { "FIFO", "LRU", "RR" };

    for (const auto& nome : nomesCache) {
        ResultadoAlgoritmo resultado_algo_atual;
        resultado_algo_atual.nome_algoritmo = nome;

        for (int usuario = 1; usuario <= 3; ++usuario) {
            CachePtr cache;
            if (nome == "FIFO") cache = std::make_shared<FifoCache>();
            else if (nome == "LRU") cache = std::make_shared<LruCache>();
            else if (nome == "RR") cache = std::make_shared<RRCache>();

            if (cache) {
                // Roda a simulação e guarda o resultado do usuário
                resultado_algo_atual.resultados_por_usuario.push_back(simularUsuario(cache, usuario));
            }
        }
        // Guarda o resultado consolidado do algoritmo
        todos_os_resultados.push_back(resultado_algo_atual);
    }

    // Ao final de TUDO, chama a função que imprime o relatório
    exibirRelatorioFinal(todos_os_resultados);

    std::cout << "\n\n=== Simulacao concluida ===\n";
    std::cout << "Pressione Enter para voltar ao menu principal..." << std::endl;
    std::cin.get(); // Pausa para o usuário ler o relatório
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}