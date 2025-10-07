#pragma once

#include "ICache.h" // Inclui nossa interface
#include <string>

// Declaração da função que simula a leitura do disco lento.
// Ela será implementada no core.cpp
namespace Disco {
    Texto lerTextoDoDisco(int textId);
}