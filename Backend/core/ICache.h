#pragma once // Garante que este arquivo seja inclu�do apenas uma vez

#include <string>
#include <memory> // Para usar std::shared_ptr

// Usaremos um 'alias' para o tipo de dado do texto para facilitar futuras manuten��es.
using Texto = std::string;

/*
 * ICache (Interface de Cache)
 * Esta � uma classe base abstrata que define o "contrato" para todos os
 * algoritmos de cache que ser�o implementados.
 * O Aluno A define esta interface, e os Alunos B e C criar�o classes
 * concretas (FIFO, LRU, etc.) que herdam dela e implementam seus m�todos.
 */
class ICache {
public:
    // Destrutor virtual � obrigat�rio em classes base com fun��es virtuais.
    virtual ~ICache() = default;

    /*
     * A fun��o principal da interface.
     * Dado um ID de texto, este m�todo deve retornar o conte�do do texto.
     * A l�gica interna decidir� se busca do cache (hit) ou do "disco lento" (miss).
     *
     * @param textId O identificador num�rico do texto (de 1 a 100).
     * @return O conte�do do texto como uma string.
     */
    virtual Texto getTexto(int textId) = 0; // O "= 0" torna a fun��o "pura virtual", obrigando as classes filhas a implement�-la.

    // Futuramente, voc� pode adicionar outras fun��es para coletar m�tricas, se necess�rio.
    // Por exemplo:
    // virtual int getCacheHits() const = 0;
    // virtual int getCacheMisses() const = 0;
};

/*
 * Usaremos ponteiros inteligentes (smart pointers) para gerenciar a inst�ncia do cache.
 * Isso evita vazamentos de mem�ria e simplifica o gerenciamento do ciclo de vida do objeto.
 */
using CachePtr = std::shared_ptr<ICache>;