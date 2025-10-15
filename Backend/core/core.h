#pragma once

#include "ICache.h" // Inclui a interface
#include <string>

// Declaração da função que simula a leitura do disco.
namespace Disco {
    Texto lerTextoDoDisco(int textId);
}