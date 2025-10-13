#pragma once // Garante que este arquivo seja inclu�do apenas uma vez
#include <string>
#include <memory> // Para usar std::shared_ptr

using Texto = std::string;

class ICache {
public:
    virtual ~ICache() = default;
    virtual Texto getTexto(int textId) = 0; // O "= 0" torna a fun��o "pura virtual", obrigando as classes filhas a implement�-la.
    virtual void printStatus() const {}
    virtual std::string getNome() const = 0;
    virtual bool foiHit() const { return false; } // Default false
};

using CachePtr = std::shared_ptr<ICache>;