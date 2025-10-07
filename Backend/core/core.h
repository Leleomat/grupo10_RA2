#pragma once

#include "ICache.h" // Inclui nossa interface
#include <string>

// Declara��o da fun��o que simula a leitura do disco lento.
// Ela ser� implementada no core.cpp
namespace Disco {
    Texto lerTextoDoDisco(int textId);
}