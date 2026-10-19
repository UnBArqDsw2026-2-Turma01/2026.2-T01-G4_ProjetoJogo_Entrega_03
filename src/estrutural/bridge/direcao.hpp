#pragma once
#include <cmath>

namespace bridge {

struct Direcao {
    double x = 0.0;
    double y = 0.0;
};

// Evita que a diagonal seja mais rápida que a reta.
inline Direcao normalizar(Direcao d) {
    const double m = std::hypot(d.x, d.y);
    if (m > 1.0) return {d.x / m, d.y / m};
    return d;
}

} 