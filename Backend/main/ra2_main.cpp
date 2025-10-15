#include <iostream> // Para usar std::cin, std::cout, e std::endl para interagir com o console
#include <string> // Para usar a classe std::string
#include <memory> // Para std::shared_ptr (CachePtr).
#include <limits> // Para std::numeric_limits, usado na limpeza do buffer de entrada.
#include "../core/ICache.h"
#include "../main/FifoCache.h"
#include "../main/RRCache.h"
#include "../main/LruCache.h"
#include "../main/simulation.h"
#include <chrono> // Para medir o tempo de execução.
#define NOMINMAX// Evita conflitos de macros de min/max com a biblioteca do Windows.
#include <windows.h> // Mudar as cores do texto no console.
#include <fstream> // Para manipulação de arquivos

// Função para carregar o cache salvo do arquivo de configuração
// Garante a persistência da escolha entre as execuções do programa.
CachePtr carregarCacheSalvo() {
    // Tenta abrir o arquivo de configuração.
    std::ifstream arquivo_config("cache_config.txt");
    if (arquivo_config.is_open()) {
        std::string nome_algoritmo;
        // Lê a primeira linha, que deve conter o nome do algoritmo.
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

// Exibe o conteúdo do texto com formatação padrão.
void mostrarTexto(const Texto& texto) {
    std::cout << "\n--------------------- INICIO DO TEXTO ---------------------\n";
    std::cout << texto << std::endl;
    std::cout << "---------------------- FIM DO TEXTO -----------------------\n\n";
}

// Função principal do programa.
int main() {
    // Para fins de testes, substituir a linha abaixo por: 
    // FIFO: CachePtr cacheAtual = std::make_shared<FifoCache>();
    // LRU: CachePtr cacheAtual = std::make_shared<LruCache>();
    // RR: CachePtr cacheAtual = std::make_shared<RRCache>();
    
    // Na inicialização, tenta carregar o cache salvo da última sessão.
    // Se não houver, 'cacheAtual' será um ponteiro nulo.
    CachePtr cacheAtual = carregarCacheSalvo();
    
    // Variável para guardar o vencedor da simulação apenas na sessão atual.
    std::string vencedor_desta_sessao = "";
    std::cout << " ~~~~~ Bem-vindo ao Leitor de Textos da 'Texto eh Vida'! ~~~~~ " << std::endl;

    int idTexto = -1; // Variável para armazenar a escolha do usuário.

    // Loop principal do programa, continua executando até o usuário escolher sair.
    while (true) {
        if (!cacheAtual) { // Se nenhum cache foi escolhido ainda
            if (vencedor_desta_sessao.empty()) {
                // Estado 1: Primeira execução, sem cache e sem simulação rodada ainda.
                std::cout << "\n Nenhum algoritmo de cache esta ativo. Execute o modo de simulacao (-1) para escolher um." << std::endl;
            }
            else {
                // Estado 2: Sem cache ativo, mas uma simulação já foi rodada nesta sessão.
                std::cout << "\n Nenhum algoritmo de cache rodando. Vencedor da simulacao: [" << vencedor_desta_sessao << "]." << std::endl;
                std::cout << " Encerre e inicie o programa novamente para ativar o algoritmo." << std::endl;
            }
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << " Sua escolha: ";
        }
        else { // Se algum cache já está carregado e ativo.
            std::cout << "\n>>> Algoritmo de cache vencedor da inicializacao anterior '" << cacheAtual->getNome() << "' carregado. <<<\n\n";
            std::cout << " Digite o numero do texto que deseja ler (1-100), ou:" << std::endl;
            std::cout << " -1 para entrar no modo de simulacao" << std::endl;
            std::cout << "  0 para sair" << std::endl;
            std::cout << " Sua escolha: ";
        }
        // --- Leitura e Validação da Entrada ---
        std::cin >> idTexto;

        // Tratamento de erro para entrada não-numérica
        if (std::cin.fail()) {
            std::cout << "\nEntrada invalida. Por favor, digite um numero.\n" << std::endl;
            std::cin.clear(); // Limpa o estado de erro do std::cin.
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue; // Pula para a próxima iteração do loop.
        }

        // --- Processamento dos Comandos ---
        if (idTexto == 0) { // Opção 0: Sair do programa.
            std::cout << "Encerrando o programa. Ate mais!" << std::endl;
            break; // Quebra o loop while(true)
        }
        else if (idTexto == -1) { // Opção -1: Entrar no modo de simulação.
            // Executa a simulação e captura o nome do algoritmo vencedor.
            std::string vencedor_simulacao = executarSimulacao();
            
            // Armazena o vencedor na variável da sessão.
            vencedor_desta_sessao = vencedor_simulacao;

            std::cout << "\n>>> Simulacao concluida!" << std::endl;

            // Verifica se a string não está vazia (caso a simulação falhe por algum motivo)
            // Informa ao usuário qual algoritmo será usado na próxima inicialização.
            if (!vencedor_simulacao.empty()) {
                std::cout << ">>> O algoritmo [" << vencedor_simulacao << "] foi o vencedor e sera ativado na proxima vez que o programa for iniciado." << std::endl;
            }
            else {
                std::cout << "Nao foi possivel determinar um vencedor na simulacao.\n" << std::endl;
            }
        }
        else if (idTexto >= 1 && idTexto <= 100) {// Opção 1-100: Ler um arquivo de texto.
            if (cacheAtual) { // Apenas tenta ler se um cache estiver ativo.
                // Marca o tempo de início.
                auto inicio = std::chrono::high_resolution_clock::now();
                // A chamada a getTexto retorna uma struct com o texto e o status de hit/miss.
                CacheGetResult resultado = cacheAtual->getTexto(idTexto);
                // Usa o membro '.texto' da struct para exibir o conteúdo.
                mostrarTexto(resultado.texto);

                auto fim = std::chrono::high_resolution_clock::now(); // Marca o tempo de fim.
                auto duracao = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio); // Calcula a duração.

                std::cout << "----------------------------------------------" << std::endl;
                std::cout << "+++ Tempo total de carregamento: " << duracao.count() << " us +++" << std::endl;
                std::cout << "----------------------------------------------" << std::endl;
                cacheAtual->printStatus(); // Mostra o estado atual do cache.
            }
            else {
                // Mensagem de erro se o usuário tentar ler sem um cache ativo.
                std::cout << "\nERRO: Nao ha um cache ativo. Nao eh possivel carregar o texto." << std::endl;
                std::cout << "\nVoce deve primeiro rodar a simulacao (-1).\n";
            }
        }
        else if (!cacheAtual) {
            // Trata entradas numéricas inválidas (fora do intervalo esperado).
            std::cout << "\nNumero invalido. Por favor, escolha 0 para Sair e -1 para entrar no Modo de Simulação.\n" << std::endl;
        }else {
            std::cout << "\nNumero de texto invalido. Por favor, escolha um numero entre 1 e 100.\n" << std::endl;
        }
    }
    return 0; // Fim do programa.
}