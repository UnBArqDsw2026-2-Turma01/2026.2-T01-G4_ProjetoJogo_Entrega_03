#pragma once
#include "direcao.hpp"

namespace bridge {

// Contrato comum a todos os dispositivos e IAs.
// Devolve apenas a intenção de direção; não move nada.
class FonteDeEntrada {
public:
    virtual ~FonteDeEntrada() = default;
    virtual Direcao lerDirecao() const = 0;
};

} // namespace bridge