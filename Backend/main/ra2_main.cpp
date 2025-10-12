#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include <limits>
#include "../main/CacheGuardaChuva.h"
#include "../core/ICache.h"
#include "../core/conectorDiscoTemporario.h"
#include "../main/FifoCache.h"

// --- ATENÇÃO ---
// No futuro, aqui incluiremos os headers dos algoritmos dos seus colegas.
// #include "../algorithms/FifoCache.h"
// #include "../algorithms/LruCache.hh"
// E também o módulo de simulação.
// #include "../simulation/simulation.h"

class CacheGenerico {
private:
    std::shared_ptr<CacheGuardaChuva> algoritmoCache;

public:
    CacheGenerico(std::shared_ptr<CacheGuardaChuva> algoritmo) : algoritmoCache(algoritmo) {}

    void setAlgoritmo(std::shared_ptr<CacheGuardaChuva> algoritmo) {
        algoritmoCache = algoritmo;
    }

    std::string getTexto(int id) {
        if (!algoritmoCache) return "";
        return algoritmoCache->getTexto(id);
    }

    void printStatus() const {
        if (algoritmoCache)
            algoritmoCache->printStatus();
    }
};

void mostrarTexto(const Texto& texto) {
    std::cout << "\n--------------------- INICIO DO TEXTO ---------------------\n";
    std::cout << texto << std::endl;
    std::cout << "---------------------- FIM DO TEXTO -----------------------\n\n";
}

int main() {
    //// Por enquanto, nosso ponteiro de cache estará vazio (nullptr).
    //// Quando o Aluno B terminar o FIFO, poderemos fazer:
    //// cacheAtual = std::make_shared<FifoCache>();
    //// Versão NOVA que ativa o cache de teste
    //CachePtr cacheAtual = std::make_shared<conectorDiscoTemporario>();

    //std::cout << "Bem-vindo ao Leitor de Textos da 'Texto eh Vida'!" << std::endl;
    //// Aqui, você carregaria a configuração do último cache escolhido na simulação.
    //// Por agora, vamos apenas simular que nenhum foi escolhido ainda.

    //std::shared_ptr<CacheGuardaChuva> fifo = std::make_shared<FifoCache>();
    //CacheGenerico cache(fifo);


    //int idTexto = -1;

    //while (true) {
    //    if (!cacheAtual) { // Se nenhum cache foi escolhido ainda
    //        std::cout << "Nenhum algoritmo de cache esta ativo. Execute o modo de simulacao (-1) para escolher um." << std::endl;
    //    }

    //    std::cout << "Digite o numero do texto que deseja ler (1-100), ou:" << std::endl;
    //    std::cout << " -1 para entrar no modo de simulacao" << std::endl;
    //    std::cout << "  0 para sair" << std::endl;
    //    std::cout << "Sua escolha: ";

    //    std::cin >> idTexto;

    //    // Tratamento de erro para entrada não-numérica
    //    if (std::cin.fail()) {
    //        std::cout << "\nEntrada invalida. Por favor, digite um numero.\n" << std::endl;
    //        std::cin.clear();
    //        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //        continue;
    //    }

    //    if (idTexto == 0) {
    //        std::cout << "Encerrando o programa. Ate mais!" << std::endl;
    //        break; // Encerra o laço
    //    }
    //    else if (idTexto == -1) {
    //        std::cout << "\n--- MODO DE SIMULACAO (A SER IMPLEMENTADO PELO ALUNO D) ---\n" << std::endl;
    //        // Aqui será a chamada para a função do Aluno D.
    //        // ex: cacheAtual = Simulacao::executar();
    //    }
    //    else if (idTexto >= 1 && idTexto <= 100) {
    //        if (cacheAtual) {
    //            Texto texto = cacheAtual->getTexto(idTexto);
    //            mostrarTexto(texto);
    //        }
    //        else {
    //            std::cout << "\nERRO: Nao ha um cache ativo. Nao eh possivel carregar o texto." << std::endl;
    //        }
    //    }
    //    else {
    //        std::cout << "\nNumero de texto invalido. Por favor, escolha um numero entre 1 e 100.\n" << std::endl;
    //    }
    //}

    std::shared_ptr<CacheGuardaChuva> fifo = std::make_shared<FifoCache>();
    CacheGenerico cache(fifo);

    
    int i = 1;
    std::string texto = cache.getTexto(i);

    std::cout << texto << "\n\n";

    texto = cache.getTexto(i);
    
    std::cout << texto << "\n\n";

    cache.printStatus();

    return 0;
}