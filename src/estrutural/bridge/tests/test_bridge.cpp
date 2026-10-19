#include "controlador_jogador.hpp"
#include <cmath>
#include <cstdio>
#include <memory>

using namespace bridge;

static int falhas = 0;
#define VERIFICAR(cond)                                              \
    do {                                                             \
        if (!(cond)) {                                               \
            std::printf("FALHOU: %s (linha %d)\n", #cond, __LINE__); \
            ++falhas;                                                \
        }                                                            \
    } while (0)

static bool aprox(double a, double b) { return std::fabs(a - b) < 1e-9; }

// Fonte falsa: permite testar o movimento sem teclado nem Godot.
class FonteFalsa : public FonteDeEntrada {
public:
    explicit FonteFalsa(Direcao d) : d(d) {}
    Direcao lerDirecao() const override { return d; }
private:
    Direcao d;
};

int main() {
    // 1. Deslocamento = direção × velocidade × dt
    {
        auto fonte = std::make_shared<FonteFalsa>(Direcao{1, 0});
        ControladorMovimento c(fonte, 100);
        Direcao d = c.deslocamento(0.5);
        VERIFICAR(aprox(d.x, 50) && aprox(d.y, 0));
    }

    // 2. A diagonal não é mais rápida que a reta
    {
        auto fonte = std::make_shared<FonteFalsa>(Direcao{1, 1});
        ControladorMovimento c(fonte, 100);
        Direcao d = c.deslocamento(1.0);
        VERIFICAR(aprox(std::hypot(d.x, d.y), 100));
    }

    // 3. Trocar a fonte em tempo de execução muda a direção
    {
        auto teclado = std::make_shared<FonteFalsa>(Direcao{1, 0});
        auto controle = std::make_shared<FonteFalsa>(Direcao{0, -1});
        ControladorMovimento c(teclado, 100);
        c.trocarFonte(controle);
        Direcao d = c.deslocamento(1.0);
        VERIFICAR(aprox(d.x, 0) && aprox(d.y, -100));
    }

    // 4. Desativado não se move
    {
        auto fonte = std::make_shared<FonteFalsa>(Direcao{1, 0});
        ControladorMovimento c(fonte, 100);
        c.ativar(false);
        Direcao d = c.deslocamento(1.0);
        VERIFICAR(aprox(d.x, 0) && aprox(d.y, 0));
    }

    // 5. Atributo de velocidade (Pena Leve, +2) altera a velocidade
    {
        auto fonte = std::make_shared<FonteFalsa>(Direcao{1, 0});
        ControladorJogador j(fonte, ControladorJogador::BASE);
        j.aplicarAtributoVelocidade(2);
        Direcao d = j.deslocamento(1.0);
        VERIFICAR(aprox(d.x, ControladorJogador::BASE + 2 * ControladorJogador::GANHO));
    }

    if (falhas == 0) {
        std::printf("OK: 5 grupos de verificações do Bridge passaram.\n");
        return 0;
    }
    std::printf("%d falha(s).\n", falhas);
    return 1;
}