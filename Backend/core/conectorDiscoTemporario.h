#pragma once
#include "ICache.h"
#include <string>
#include <iostream>
#include "core.h" // Para ter acesso a Disco::lerTextoDoDisco

// Um cache "falso" que sempre falha (cache miss)
// Serve apenas para testar a estrutura principal do Aluno A.
class conectorDiscoTemporario : public ICache {
public:
    // A anotação 'override' é uma boa prática para garantir que estamos
    // substituindo corretamente uma função da classe mãe.
    Texto getTexto(int textId) override {
        // Imprime que houve um "cache miss"
        std::cout << "\n[conectorDiscoTemporario] Cache Miss! O texto " << textId << " nao foi encontrado no cache." << std::endl;

        // Como não encontrou no cache, chama a função de ler do disco lento.
        return Disco::lerTextoDoDisco(textId);
    }
};