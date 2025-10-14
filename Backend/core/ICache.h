#pragma once // Garante que este arquivo seja incluído apenas uma vez
#include <string>
#include <memory> // Para usar std::shared_ptr

using Texto = std::string;

// Uma struct para agrupar o resultado de uma busca no cache
struct CacheGetResult {
    Texto texto;
    bool foi_hit;
};

class ICache {
public:
    virtual ~ICache() = default;
    virtual CacheGetResult getTexto(int idTexto) = 0;
    virtual void printStatus() const {}
    virtual std::string getNome() const = 0;
};

using CachePtr = std::shared_ptr<ICache>;