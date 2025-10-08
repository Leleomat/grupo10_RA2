#pragma once // Garante que este arquivo seja incluído apenas uma vez

#include <string>
#include <memory> // Para usar std::shared_ptr

// Usaremos um 'alias' para o tipo de dado do texto para facilitar futuras manutenções.
using Texto = std::string;

/*
 * ICache (Interface de Cache)
 * Esta é uma classe base abstrata que define o "contrato" para todos os
 * algoritmos de cache que serão implementados.
 * O Aluno A define esta interface, e os Alunos B e C criarão classes
 * concretas (FIFO, LRU, etc.) que herdam dela e implementam seus métodos.
 */
class ICache {
public:
    // Destrutor virtual é obrigatório em classes base com funções virtuais.
    virtual ~ICache() = default;

    /*
     * A função principal da interface.
     * Dado um ID de texto, este método deve retornar o conteúdo do texto.
     * A lógica interna decidirá se busca do cache (hit) ou do "disco lento" (miss).
     *
     * @param textId O identificador numérico do texto (de 1 a 100).
     * @return O conteúdo do texto como uma string.
     */
    virtual Texto getTexto(int textId) = 0; // O "= 0" torna a função "pura virtual", obrigando as classes filhas a implementá-la.

    // Futuramente, você pode adicionar outras funções para coletar métricas, se necessário.
    // Por exemplo:
    // virtual int getCacheHits() const = 0;
    // virtual int getCacheMisses() const = 0;
};

/*
 * Usaremos ponteiros inteligentes (smart pointers) para gerenciar a instância do cache.
 * Isso evita vazamentos de memória e simplifica o gerenciamento do ciclo de vida do objeto.
 */
using CachePtr = std::shared_ptr<ICache>;