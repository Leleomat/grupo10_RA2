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
#include <iomanip>    // Para std::setw, std::left
#include <algorithm>  // Para std::max_element
#include <fstream> // Para manipulação de arquivos

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

void exibirGraficoResumoASCII(const std::vector<ResultadoAlgoritmo>& todos_resultados) {
    if (todos_resultados.empty()) return;
    std::vector<std::string> legendas;
    std::vector<double> tempos_medios;

    // 1. Extrai os dados, a legenda (ex: "FIFO-U1") e o tempo médio de cada teste
    for (const auto& resultado_algo : todos_resultados) {
        for (const auto& resultado_user : resultado_algo.resultados_por_usuario) {
            std::string legenda = " " + resultado_algo.nome_algoritmo + "-U" + std::to_string(resultado_user.id_usuario);
            legendas.push_back(legenda);

            double tempo_total = 0;
            for (const auto& s : resultado_user.solicitacoes) {
                tempo_total += s.tempo_us;
            }
            double tempo_medio = resultado_user.solicitacoes.empty() ? 0 : tempo_total / resultado_user.solicitacoes.size();
            tempos_medios.push_back(tempo_medio);
        }
    }

    if (tempos_medios.empty()) return;

    // 2. Desenha o gráfico ASCII
    double maximoTempo = *std::max_element(tempos_medios.begin(), tempos_medios.end());
    std::cout << "\n\n ~~~~~~~~~~~~~~~~~~ GRAFICO DE RESUMO DE PERFORMANCE ~~~~~~~~~~~~~~~~~~ \n\n";

    for (size_t i = 0; i < legendas.size(); ++i) {
        int barra = 0;
        if (maximoTempo > 0) {
            barra = static_cast<int>((tempos_medios[i] / maximoTempo) * 50); // barra de até 50 caracteres
        }

        // Imprime a legenda e o valor
        std::cout << std::left << std::setw(12) << legendas[i]
            << "[" << std::right << std::setw(5) << static_cast<int>(tempos_medios[i]) << " ms] | ";

        if (tempos_medios[i] > 8000) {
            setConsoleColorRed();
        }
        else if (tempos_medios[i] > 5000) { // Se não for > 8000us, checa se é > 5000us
            setConsoleColorYellow();
        }
        else { // Se não for nenhum dos anteriores, é <= 5000us
            setConsoleColorGreen();
        }

        // Desenha a barra
        for (int j = 0; j < barra; ++j) std::cout << "=";
        resetConsoleColor();

        std::cout << "\n";
    }
    // Calcula e desenha as barras de média para cada algoritmo
    std::cout << "\n ~~~~~~~~~~~~~~~~~~~~~~~~ MEDIAS POR ALGORITMO ~~~~~~~~~~~~~~~~~~~~~~~~ \n";

    for (size_t i = 0; i < todos_resultados.size(); ++i) {
        // Pega os 3 tempos de usuário para o algoritmo 'i' e calcula a média
        double media_algo = (tempos_medios[i * 3] + tempos_medios[i * 3 + 1] + tempos_medios[i * 3 + 2]) / 3.0;

        // Cria a legenda, ex: "FIFO-Media"
        std::string legenda_algo = " " + todos_resultados[i].nome_algoritmo + "-Media";

        // Usa a mesma lógica de desenho de barra de antes
        int barra = 0;
        if (maximoTempo > 0) {
            barra = static_cast<int>((media_algo / maximoTempo) * 50);
        }

        std::cout << std::left << std::setw(12) << legenda_algo
            << "[" << std::right << std::setw(5) << static_cast<int>(media_algo) << " us] | ";

        if (media_algo > 8000) setConsoleColorRed();
        else if (media_algo > 5000) setConsoleColorYellow();
        else setConsoleColorGreen();

        for (int j = 0; j < barra; ++j) std::cout << "=";
        resetConsoleColor();

        std::cout << "\n";
    }
    std::cout << "\n Legenda: Verde (< 5000us) | Amarelo (5000-8000us) | Vermelho (> 8000us)\n\n";

    std::cout << " ~~~~~~~~~~~~~~~ GRAFICO DE HITS & MISSES POR ALGORITMO ~~~~~~~~~~~~~~~ \n";

    std::vector<std::tuple<std::string, int, int>> dados_agregados; // {Nome, Total Hits, Total Misses}

    for (const auto& resultado_algo : todos_resultados) {
        int total_hits_algo = 0;
        int total_misses_algo = 0;

        for (const auto& resultado_user : resultado_algo.resultados_por_usuario) {
            total_hits_algo += resultado_user.total_hits;
            total_misses_algo += resultado_user.total_misses;
        }

        std::string nome = resultado_algo.nome_algoritmo;
        std::cout << "\n--------------------------------------------- [ " << nome << " ] ---------------------------------------------\n";

        // Escala: 1 "=" para cada 8 unidades (arredondado pra cima)
        int barra_hits = static_cast<int>(std::ceil(total_hits_algo / 8.0));
        int barra_misses = static_cast<int>(std::ceil(total_misses_algo / 8.0));

        std::cout << " Hits Totais   [" << std::setw(4) << total_hits_algo << "] | ";
        setConsoleColorGreen();
        for (int i = 0; i < barra_hits; ++i) std::cout << "=";
        resetConsoleColor();
        std::cout << "\n";

        std::cout << " Misses Totais [" << std::setw(4) << total_misses_algo << "] | ";
        setConsoleColorRed();
        for (int i = 0; i < barra_misses; ++i) std::cout << "=";
        resetConsoleColor();
        std::cout << "\n\n";

        // Usuário do mesmo algoritmo
        for (const auto& resultado_user : resultado_algo.resultados_por_usuario) {
            
            std::string nome_modelo;
            if (resultado_user.id_usuario == 1) nome_modelo = "Modelo Puro";
            else if (resultado_user.id_usuario == 2) nome_modelo = "Modelo Poisson";
            else nome_modelo = "Modelo Ponderado";

            int hits_usuario = resultado_user.total_hits;
            int misses_usuario = resultado_user.total_misses;

            // Escala: 1 "=" para cada 4 unidades (arredondado pra cima)
            int barra_hits_user = static_cast<int>(std::ceil(hits_usuario / 8.0));
            int barra_misses_user = static_cast<int>(std::ceil(misses_usuario / 8.0));
            
            std::cout << "  [User " << resultado_user.id_usuario << " - " << nome_modelo << "]\n";

            // Hits
            std::cout << "       Hits    [" << std::setw(4) << hits_usuario << "] | ";
            setConsoleColorGreen();
            for (int i = 0; i < barra_hits_user; ++i) std::cout << "=";
            resetConsoleColor();
            std::cout << "\n";

            // Misses
            std::cout << "       Misses  [" << std::setw(4) << misses_usuario << "] | ";
            setConsoleColorRed();
            for (int i = 0; i < barra_misses_user; ++i) std::cout << "=";
            resetConsoleColor();
            std::cout << "\n";
        }
        
    }
    std::cout << "\n---------------------------------------------------------------------------------------------------\n";
}

// Função para salvar a escolha do algoritmo em um arquivo
void salvarEscolhaCache(const std::string & nome_algoritmo) {
    // std::ofstream abre o arquivo para escrita, cria ele se não existir ou sobrescreve se já existir.
    std::ofstream arquivo_config("cache_config.txt");
    if (arquivo_config.is_open()) {
        arquivo_config << nome_algoritmo;
        arquivo_config.close();
        std::cout << "\n>>> Escolha salva em 'cache_config.txt' para futuras inicializacoes. <<<";
    }
    else {
        std::cout << "ERRO: Nao foi possivel salvar a escolha do cache no arquivo de configuracao.\n";
    }
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
    const int numSolicitacoes = 200; // 200 por Usuario por algoritmo

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
        //Captura a struct completa retornada pelo getTexto
        CacheGetResult resultado_get = cache->getTexto(idTexto);
        auto fim = std::chrono::high_resolution_clock::now();

        auto duracao_us = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();

        // A decisão de HIT/MISS vem diretamente do cache
        bool foi_hit = resultado_get.foi_hit;
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
                    << " // Tempo: " << s.tempo_us << " us"
                    << " // " << (s.foi_hit ? "HIT" : "MISS") << "]" << std::endl;
            }

            // Adicionando o sumário por usuário
            double tempo_total = 0;
            for (const auto& s : resultado_user.solicitacoes) {
                tempo_total += s.tempo_us;
            }
            std::cout << "\n[SUMARIO USER " << resultado_user.id_usuario << " (" << resultado_algo.nome_algoritmo << ")]" << std::endl;
            std::cout << " -> Total Hits: " << resultado_user.total_hits << " | Total Misses: " << resultado_user.total_misses << std::endl;
            std::cout << " -> Tempo medio por solicitacao: " << tempo_total / resultado_user.solicitacoes.size() << " us" << std::endl;
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
std::string executarSimulacao() {
    std::vector<ResultadoAlgoritmo> todos_os_resultados;
    const std::vector<std::string> nomesCache = { "FIFO", "LRU", "RR" };

    // Mapa para guardar o desempenho de cada algoritmo (nome -> total de misses)
    std::map<std::string, double> performance; // mapeia nome -> tempo médio em us

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
                    tempo_total_usuario += s.tempo_us;
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

    // Imprime o relatório
    exibirRelatorioFinal(todos_os_resultados);
    exibirGraficoResumoASCII(todos_os_resultados);

    // Vencedor baseado no tempo médio
    std::string melhor_algoritmo = "";
    double menor_tempo_medio = -1.0;

    for (const auto& par : performance) {
        std::cout << "[ANALISE] Tempo medio final para " << par.first << ": " << par.second << " us" << std::endl;
        if (menor_tempo_medio < 0 || par.second < menor_tempo_medio) {
            menor_tempo_medio = par.second;
            melhor_algoritmo = par.first;
        }
    }

    std::cout << "\n===================================================================\n";
    std::cout << ">>> VENCEDOR DA SIMULACAO: " << melhor_algoritmo << " (com tempo medio de " << menor_tempo_medio << " us) <<<" << std::endl;
    std::cout << "===================================================================\n";

    // Salva a escolha do algoritmo para a próxima execução
    if (!melhor_algoritmo.empty()) {
        salvarEscolhaCache(melhor_algoritmo);
    }

    // Retorna o nome do algoritmo vencedor para quem chamou a função no main
    return melhor_algoritmo;

}