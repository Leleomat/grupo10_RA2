#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include <limits>
#include "../core/ICache.h"
#include "../main/FifoCache.h"
#include "../main/RRCache.h"
#include "../main/LruCache.h"
#include "../main/simulation.h"
#include <chrono>
#define NOMINMAX
#include <windows.h>
#include <fstream> // Para manipulação de arquivos

// Função para carregar o cache salvo do arquivo de configuração
CachePtr carregarCacheSalvo() {
    std::ifstream arquivo_config("cache_config.txt");
    if (arquivo_config.is_open()) {
        std::string nome_algoritmo;
        if (std::getline(arquivo_config, nome_algoritmo)) {
            if (nome_algoritmo == "FIFO") {
                return std::make_shared<FifoCache>();
            }
            if (nome_algoritmo == "LRU") {
                return std::make_shared<LruCache>();
            }
            if (nome_algoritmo == "RR") {
                return std::make_shared<RRCache>();
            }
        }
    }
    return nullptr; // Retorna nulo se o arquivo não existir ou for inválido
}

void mostrarTexto(const Texto& texto) {
    std::cout << "\n--------------------- INICIO DO TEXTO ---------------------\n";
    std::cout << texto << std::endl;
    std::cout << "---------------------- FIM DO TEXTO -----------------------\n\n";
}

int main() {
    // Para fins de testes, substituir a linha abaixo por: 
    // FIFO: CachePtr cacheAtual = std::make_shared<FifoCache>();
    // LRU: CachePtr cacheAtual = std::make_shared<LruCache>();
    // RR: CachePtr cacheAtual = std::make_shared<RRCache>();
    // Tenta carregar o cache da última sessão >> >
    CachePtr cacheAtual = carregarCacheSalvo();
    
    std::cout << " ~~~~~ Bem-vindo ao Leitor de Textos da 'Texto eh Vida'! ~~~~~ " << std::endl;

    int idTexto = -1;


    while (true) {
        if (!cacheAtual) { // Se nenhum cache foi escolhido ainda
            std::cout << " Nenhum algoritmo de cache esta ativo. Execute o modo de simulacao (-1) para escolher um." << std::endl;
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << " Sua escolha: ";
        }
        else {
            std::cout << "\n>>> Algoritmo de cache ('" << cacheAtual->getNome() << "') carregado. <<<\n\n";
            std::cout << " Digite o numero do texto que deseja ler (1-100), ou:" << std::endl;
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << " Sua escolha: ";
        }
        std::cin >> idTexto;

        // Tratamento de erro para entrada não-numérica
        if (std::cin.fail()) {
            std::cout << "\nEntrada invalida. Por favor, digite um numero.\n" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (idTexto == 0) {
            std::cout << "Encerrando o programa. Ate mais!" << std::endl;
            break; // Encerra o laço
        }
        else if (idTexto == -1) {
            //Captura o nome do vencedor retornado pela função >> >
            std::string vencedor_simulacao = executarSimulacao();

            std::cout << "\n>>> Simulacao concluida! <<<" << std::endl;

            // Verifica se a string não está vazia (caso a simulação falhe por algum motivo)
            if (!vencedor_simulacao.empty()) {
                std::cout << ">>> O algoritmo [" << vencedor_simulacao << "] foi o vencedor e sera ativado na proxima vez que o programa for iniciado." << std::endl;
            }
            else {
                std::cout << "Nao foi possivel determinar um vencedor na simulacao.\n" << std::endl;
            }
        }
        else if (idTexto >= 1 && idTexto <= 100) {
            if (cacheAtual) {
                auto inicio = std::chrono::high_resolution_clock::now();

                // A chamada a getTexto retorna uma struct.
                CacheGetResult resultado = cacheAtual->getTexto(idTexto);
                // O membro '.texto' da struct é passado para a função.
                mostrarTexto(resultado.texto);

                auto fim = std::chrono::high_resolution_clock::now();
                auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio);

                std::cout << "----------------------------------------------" << std::endl;
                std::cout << "+++ Tempo total de carregamento: " << duracao.count() << " us +++" << std::endl;
                std::cout << "----------------------------------------------" << std::endl;
                cacheAtual->printStatus();
            }
            else {
                std::cout << "\nERRO: Nao ha um cache ativo. Nao eh possivel carregar o texto." << std::endl;
                std::cout << "\nVoce deve primeiro rodar a simulacao (-1).\n";
            }
        }
        else if (!cacheAtual) {
            std::cout << "\nNumero invalido. Por favor, escolha 0 para Sair e -1 para entrar no Modo de Simulação.\n" << std::endl;
        }else {
            std::cout << "\nNumero de texto invalido. Por favor, escolha um numero entre 1 e 100.\n" << std::endl;
        }
    }
    return 0;
}