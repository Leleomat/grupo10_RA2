#pragma once

#include "ICache.h" // Inclui a interface
#include <string>

// Declara��o da fun��o que simula a leitura do disco.
namespace Disco {
    Texto lerTextoDoDisco(int textId);
}