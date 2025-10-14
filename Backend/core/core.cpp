#include "core.h"      // Inclui o cabeçalho correspondente que declara a função e tipos como 'Texto'.
#include <iostream>    // Para entrada e saída de dados no console (std::cout, std::cerr).
#include <fstream>     // Para manipulação de arquivos (std::ifstream).
#include <string>      // Para usar a classe std::string.
#include <chrono>      // Para especificar durações de tempo

// Implementação da função que simula a leitura dos arquivos de texto do disco.
namespace Disco {
    Texto lerTextoDoDisco(int textId) {
        // Concatena o caminho do diretório de textos com o ID convertido para string e a extensão ".txt".
        std::string path = "../../texts/" + std::to_string(textId) + ".txt";

        // Imprime uma mensagem no console para fins de log, informando o acesso ao disco
        std::cout << "[DISCO LENTO] Acessando o disco para ler o arquivo: " << path << "..." << std::endl;

        // Cria um objeto de fluxo de entrada de arquivo (ifstream) e tenta abrir o arquivo no caminho especificado.
        std::ifstream arquivo(path);
        if (!arquivo.is_open()) {
            // Imprime uma mensagem de erro no fluxo de erro padrão caso o arquivo não seja encontrado.
            std::cerr << "ERRO: Nao foi possivel abrir o arquivo " << path << std::endl;
            return "ERRO DE LEITURA";
        }

        // O construtor da std::string está recebendo dois iteradores:
        // - std::istreambuf_iterator<char>(arquivo): Aponta para o início do buffer do arquivo.
        // - std::istreambuf_iterator<char>(): Representa o "fim do fluxo".
        // A string 'conteudo' é construída com todos os caracteres entre esses dois pontos.
        std::string conteudo((std::istreambuf_iterator<char>(arquivo)), std::istreambuf_iterator<char>());
        arquivo.close(); // Fecha o arquivo.

        // Log de sucesso, informando que a operação de leitura foi concluída.
        std::cout << "[DISCO LENTO] Leitura do arquivo " << path << " concluida." << std::endl;

        // Retorna a string com o conteúdo do arquivo para a função que a chamou (uma das classes de cache).
        return conteudo;
    }
}