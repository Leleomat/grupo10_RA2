// simulation.h
#pragma once
#include <string> // Para usar a classe std::string
#include <vector> // Acesso ao std::vector, um array din�mico. 
#include "../core/ICache.h" // Precisa saber o que � CachePtr

// Estrutura para guardar os dados de uma �nica solicita��o de texto
struct Solicitacao {
    int id_texto;
    long long tempo_us; // tempo em microssegundos
    bool foi_hit;
    std::string modo_sorteio;
};

// Estrutura para guardar o resultado completo de um �nico usu�rio com sua solicita��es
struct ResultadoUsuario {
    int id_usuario;
    std::vector<Solicitacao> solicitacoes;
    int total_hits = 0;
    int total_misses = 0;
};

// Estrutura para guardar todos os resultados de um �nico algoritmo com seus usu�rios
struct ResultadoAlgoritmo {
    std::string nome_algoritmo;
    std::vector<ResultadoUsuario> resultados_por_usuario;
};

// Fun��o principal que o main.cpp ir� chamar
std::string executarSimulacao();