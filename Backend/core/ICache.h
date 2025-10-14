#pragma once // Garante que este arquivo seja inclu�do apenas uma vez
#include <string> // Para usar a classe std::string
#include <memory> // Para usar std::shared_ptr

// Cria um "alias de tipo" chamado 'Texto'.
// Com isso podemos usar 'Texto' em vez de 'std::string', deixando o c�digo mais claro
using Texto = std::string;

// Uma struct para agrupar o resultado de uma busca no cache
struct CacheGetResult {
    Texto texto;      // O conte�do do texto que foi solicitado.
    bool foi_hit;     // Um booleano que indica se o texto foi encontrado no cache (true) ou se teve que ser lido do disco (false).
};

// Classe base abstrata que define a interface para um cache.
// Uma classe que cont�m pelo menos uma fun��o virtual pura (como getTexto) n�o pode ser instanciada, logo serve apenas como um modelo para classes derivadas.
class ICache {
public:
    // Destrutor virtual para garantir a destrui��o correta de objetos de classes derivadas.
    virtual ~ICache() = default;

    // Fun��o virtual pura: busca um texto pelo ID. � implementada por todas as classes filhas.
    virtual CacheGetResult getTexto(int idTexto) = 0;

    // Exibe o estado atual do cache para fins de depura��o.
    virtual void printStatus() const {}

    // Fun��o virtual pura: retorna o nome do algoritmo.
    virtual std::string getNome() const = 0;
};

// Alias para o ponteiro inteligente que gerenciar� a mem�ria dos objetos de cache.
using CachePtr = std::shared_ptr<ICache>;