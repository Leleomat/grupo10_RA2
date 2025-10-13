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
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#define NOMINMAX
#include <windows.h>


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

void mostrarTexto(const Texto& texto) {
    std::cout << "\n--------------------- INICIO DO TEXTO ---------------------\n";
    std::cout << texto << std::endl;
    std::cout << "---------------------- FIM DO TEXTO -----------------------\n\n";
}

int main() {
    CachePtr cacheAtual = nullptr;
    //CachePtr cacheAtual = std::make_shared<FifoCache>();
    
    std::cout << "Bem-vindo ao Leitor de Textos da 'Texto eh Vida'!" << std::endl;

    int idTexto = -1;
    // Vetores para coletar dados para o gráfico
    std::vector<int> idsLidos;
    std::vector<double> temposMs;


    while (true) {
        if (!cacheAtual) { // Se nenhum cache foi escolhido ainda
            std::cout << " Nenhum algoritmo de cache esta ativo. Execute o modo de simulacao (-1) para escolher um." << std::endl;
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << "Sua escolha: ";
        }
        else {
            std::cout << "Algoritmo de cache ativo: " << cacheAtual->getNome() << std::endl;
            std::cout << " Digite o numero do texto que deseja ler (1-100), ou:" << std::endl;
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << "Sua escolha: ";
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
            cacheAtual = executarSimulacao();

            if (cacheAtual) {
                std::cout << "\n>>> O algoritmo " << cacheAtual->getNome() << " foi selecionado e esta ativo! <<<\n";
            }
            else {
                std::cout << "\nERRO: A simulacao nao conseguiu selecionar um algoritmo.\n";
            }
        }
        else if (idTexto >= 1 && idTexto <= 100) {
            if (cacheAtual) {
                auto inicio = std::chrono::high_resolution_clock::now();
                Texto texto = cacheAtual->getTexto(idTexto);
                mostrarTexto(texto);
                auto fim = std::chrono::high_resolution_clock::now();
                auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
                std::cout << "-------------------------------------------" << std::endl;
                std::cout << "+++ Tempo total de carregamento: " << duracao.count() << " ms +++" << std::endl;
                std::cout << "-------------------------------------------" << std::endl;
                cacheAtual->printStatus();
                // salva os resultados pro gráfico
                idsLidos.push_back(idTexto);
                temposMs.push_back(static_cast<double>(duracao.count()));
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

    if (!idsLidos.empty()) {
        mostrarGraficoASCII(idsLidos, temposMs);
    }
    else {
        std::cout << "Nenhum texto foi carregado, sem grafico a exibir.\n";
    }

    return 0;
}