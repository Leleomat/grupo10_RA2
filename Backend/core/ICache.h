#pragma once // Garante que este arquivo seja incluído apenas uma vez
#include <string> // Para usar a classe std::string
#include <memory> // Para usar std::shared_ptr

// Cria um "alias de tipo" chamado 'Texto'.
// Com isso podemos usar 'Texto' em vez de 'std::string', deixando o código mais claro
using Texto = std::string;

// Uma struct para agrupar o resultado de uma busca no cache
struct CacheGetResult {
    Texto texto;      // O conteúdo do texto que foi solicitado.
    bool foi_hit;     // Um booleano que indica se o texto foi encontrado no cache (true) ou se teve que ser lido do disco (false).
};

// Classe base abstrata que define a interface para um cache.
// Uma classe que contém pelo menos uma função virtual pura (como getTexto) não pode ser instanciada, logo serve apenas como um modelo para classes derivadas.
class ICache {
public:
    // Destrutor virtual para garantir a destruição correta de objetos de classes derivadas.
    virtual ~ICache() = default;

    // Função virtual pura: busca um texto pelo ID. É implementada por todas as classes filhas.
    virtual CacheGetResult getTexto(int idTexto) = 0;

    // Exibe o estado atual do cache para fins de depuração.
    virtual void printStatus() const {}

    // Função virtual pura: retorna o nome do algoritmo.
    virtual std::string getNome() const = 0;
};

// Alias para o ponteiro inteligente que gerenciará a memória dos objetos de cache.
using CachePtr = std::shared_ptr<ICache>;