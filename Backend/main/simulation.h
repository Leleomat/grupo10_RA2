// simulation.h
#pragma once
#include <string> // Para usar a classe std::string
#include <vector> // Acesso ao std::vector, um array dinâmico. 
#include "../core/ICache.h" // Precisa saber o que é CachePtr

// Estrutura para guardar os dados de uma única solicitação de texto
struct Solicitacao {
    int id_texto;
    long long tempo_us; // tempo em microssegundos
    bool foi_hit;
    std::string modo_sorteio;
};

// Estrutura para guardar o resultado completo de um único usuário com sua solicitações
struct ResultadoUsuario {
    int id_usuario;
    std::vector<Solicitacao> solicitacoes;
    int total_hits = 0;
    int total_misses = 0;
};

// Estrutura para guardar todos os resultados de um único algoritmo com seus usuários
struct ResultadoAlgoritmo {
    std::string nome_algoritmo;
    std::vector<ResultadoUsuario> resultados_por_usuario;
};

// Função principal que o main.cpp irá chamar
std::string executarSimulacao();