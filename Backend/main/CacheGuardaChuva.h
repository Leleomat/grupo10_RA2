#pragma once
#include <string>

//Classe guarda chuva para os algoritmos a serem implementados, usar ela como base!
class CacheGuardaChuva {
public:
    virtual ~CacheGuardaChuva() = default;
    virtual std::string getTexto(int id) = 0;
    virtual void printStatus() const = 0;
};