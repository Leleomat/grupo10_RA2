#pragma once
#include "ICache.h"
#include <string>
#include <iostream>
#include "core.h" // Para ter acesso a Disco::lerTextoDoDisco

// Um cache "falso" que sempre falha (cache miss)
// Serve apenas para testar a estrutura principal do Aluno A.
class conectorDiscoTemporario : public ICache {
public:
    // A anota��o 'override' � uma boa pr�tica para garantir que estamos
    // substituindo corretamente uma fun��o da classe m�e.
    Texto getTexto(int textId) override {
        // Imprime que houve um "cache miss"
        std::cout << "\n[conectorDiscoTemporario] Cache Miss! O texto " << textId << " nao foi encontrado no cache." << std::endl;

        // Como n�o encontrou no cache, chama a fun��o de ler do disco lento.
        return Disco::lerTextoDoDisco(textId);
    }
};