#include "core.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>   // Para std::this_thread::sleep_for
#include <chrono>   // Para std::chrono::milliseconds

// Implementação da função que simula a leitura lenta.
namespace Disco {
    Texto lerTextoDoDisco(int textId) {
        // Assume que os textos estão na pasta "texts" e nomeados como "texto_ID.txt"
        // Ex: "texts/1.txt", "texts/2.txt", etc.
        std::string path = "../../texts/" + std::to_string(textId) + ".txt";

        std::cout << "\n[DISCO LENTO] Acessando o disco para ler o arquivo: " << path << "..." << std::endl;

        // --- SIMULAÇÃO DE LENTIDÃO ---
        // Pausa a execução por 200 milissegundos para simular um disco lento.
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::ifstream arquivo(path);
        if (!arquivo.is_open()) {
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo " << path << std::endl;
            return "ERRO DE LEITURA";
        }

        std::string conteudo((std::istreambuf_iterator<char>(arquivo)), std::istreambuf_iterator<char>());
        arquivo.close();

        std::cout << "[DISCO LENTO] Leitura do arquivo " << path << " concluida." << std::endl;

        return conteudo;
    }
}