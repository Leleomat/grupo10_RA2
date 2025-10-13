#include "../main/simulation.h" // Inclui a própria declaração
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <numeric>
#include <map>
#define NOMINMAX
#include <windows.h>
#include <iomanip>

// Includes para os tipos de cache
#include "../main/FifoCache.h"
#include "../main/LruCache.h"
#include "../main/RRCache.h"
#include "../core/ICache.h"

#ifdef _WIN32
void setConsoleColorRed() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); }
void setConsoleColorYellow() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); }
void setConsoleColorGreen() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); }
void resetConsoleColor() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); }
#endif

void mostrarGraficoASCII(const std::vector<int>& ids, const std::vector<double>& tempos) {
    if (ids.empty() || tempos.empty()) {
        std::cout << "Nenhum dado para plotar.\n";
        return;
    }

    double maximoTempo = *std::max_element(tempos.begin(), tempos.end());

    std::cout << "\n=== GRAFICO DE TEMPOS (ASCII COLORIDO) ===\n\n";

    for (size_t i = 0; i < ids.size(); ++i) {
        int barra = static_cast<int>((tempos[i] / maximoTempo) * 50); // barra de até 50 caracteres

        std::cout << "Texto " << std::setw(3) << ids[i] << " [" << std::setw(4) << static_cast<int>(tempos[i]) << "ms] | ";
        double percentual = tempos[i] / maximoTempo;
        if (percentual > 0.7) setConsoleColorRed();
        else if (percentual > 0.4) setConsoleColorYellow();
        else setConsoleColorGreen();

        for (int j = 0; j < barra; ++j) std::cout << "#";
        resetConsoleColor();

        std::cout << "\n";
    }

    std::cout << "\n";
}

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
    const int numSolicitacoes = 100; // 100 por Usuario por algoritmo para teste

    for (int i = 0; i < numSolicitacoes; ++i) {
        std::cout << "\n~~~~~~~~~~ [ALGORITMO: " << cache->getNome() << "] - [USER " << usuarioId << "] - Solicitacao numero " << (i + 1) << " ~~~~~~~~~~";
        int idTexto;
        std::string modo_sorteio;

        if (usuarioId == 1) {
            idTexto = aleatorioPuro(1, 100, rng);
            modo_sorteio = "Puro";
        }
        else if (usuarioId == 2) {
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
        std::cout << "=========================================================\n";
        std::cout << "[ " << resultado_algo.nome_algoritmo << " ]" << std::endl;

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
CachePtr executarSimulacao() {
    std::vector<ResultadoAlgoritmo> todos_os_resultados;
    const std::vector<std::string> nomesCache = { "FIFO", "LRU", "RR" };

    // Mapa para guardar o desempenho de cada algoritmo (nome -> total de misses)
    std::map<std::string, double> performance; // mapeia nome -> tempo médio em ms

    for (const auto& nome : nomesCache) {
        ResultadoAlgoritmo resultado_algo_atual;
        resultado_algo_atual.nome_algoritmo = nome;

        // Vetor para guardar os tempos médios de cada um dos 3 usuários
        std::vector<double> tempos_medios_usuarios;

        for (int usuario = 1; usuario <= 3; ++usuario) {
            CachePtr cache;
            if (nome == "FIFO") cache = std::make_shared<FifoCache>();
            else if (nome == "LRU") cache = std::make_shared<LruCache>();
            else if (nome == "RR") cache = std::make_shared<RRCache>();

            if (cache) {
                ResultadoUsuario res_user = simularUsuario(cache, usuario);

                // Calcula o tempo total e médio para este usuário
                double tempo_total_usuario = 0;
                for (const auto& s : res_user.solicitacoes) {
                    tempo_total_usuario += s.tempo_ms;
                }

                // Calcula o tempo médio para ESTE usuário
                double tempo_medio_usuario = 0;
                if (!res_user.solicitacoes.empty()) {
                    tempo_medio_usuario = tempo_total_usuario / res_user.solicitacoes.size();
                }
                // Guarda o tempo médio deste usuário no vetor
                tempos_medios_usuarios.push_back(tempo_medio_usuario);

                // Roda a simulação e guarda o resultado do usuário
                resultado_algo_atual.resultados_por_usuario.push_back(res_user);
            }
        }
        // 1. Soma os 3 tempos médios dos usuários
        double soma_dos_medios = std::accumulate(tempos_medios_usuarios.begin(), tempos_medios_usuarios.end(), 0.0);
        // 2. Divide por 3 para obter a média geral do algoritmo
        double media_geral_algoritmo = 0;
        if (!tempos_medios_usuarios.empty()) {
            media_geral_algoritmo = soma_dos_medios / tempos_medios_usuarios.size();
        }

        // Guarda a performance final do algoritmo
        performance[nome] = media_geral_algoritmo;

        // Guarda o resultado consolidado do algoritmo
        todos_os_resultados.push_back(resultado_algo_atual);
    }

    // Ao final de TUDO, chama a função que imprime o relatório
    exibirRelatorioFinal(todos_os_resultados);

    // --- ALTERAÇÃO 3: LÓGICA PARA DECIDIR O VENCEDOR BASEADO NO MENOR TEMPO MÉDIO ---
    std::string melhor_algoritmo = "";
    double menor_tempo_medio = -1.0;

    for (const auto& par : performance) {
        std::cout << "\n\n[ANALISE] Tempo medio final para " << par.first << ": " << par.second << " ms" << std::endl;
        if (menor_tempo_medio < 0 || par.second < menor_tempo_medio) {
            menor_tempo_medio = par.second;
            melhor_algoritmo = par.first;
        }
    }

    std::cout << "\n===================================================================\n";
    std::cout << ">>> VENCEDOR DA SIMULACAO: " << melhor_algoritmo << " (com tempo medio de " << menor_tempo_medio << " ms) <<<" << std::endl;
    std::cout << "===================================================================\n";

    // --- Retorna uma nova instância do algoritmo vencedor ---
    if (melhor_algoritmo == "FIFO") return std::make_shared<FifoCache>();
    if (melhor_algoritmo == "LRU") return std::make_shared<LruCache>();
    if (melhor_algoritmo == "RR") return std::make_shared<RRCache>();

    return nullptr;

}