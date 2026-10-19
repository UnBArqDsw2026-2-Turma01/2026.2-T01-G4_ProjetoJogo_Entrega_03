#pragma once
#include "direcao.hpp"
#include "fonte_de_entrada.hpp"
#include <memory>
#include <utility>

namespace bridge {

// Sabe COMO mover; delega DE ONDE vem a intenção à fonte (a "ponte").
class ControladorMovimento {
public:
    ControladorMovimento(std::shared_ptr<FonteDeEntrada> fonte, double velocidade)
        : fonte(std::move(fonte)), velocidade(velocidade) {}
    virtual ~ControladorMovimento() = default;

    // Troca de dispositivo em tempo de execução: basta trocar a referência.
    void trocarFonte(std::shared_ptr<FonteDeEntrada> nova) { fonte = std::move(nova); }
    void ativar(bool a) { ativo = a; }

    // Deslocamento deste quadro, em pixels.
    virtual Direcao deslocamento(double dt) const {
        if (!ativo || !fonte) return {0.0, 0.0};
        const Direcao d = normalizar(fonte->lerDirecao());
        return {d.x * velocidade * dt, d.y * velocidade * dt};
    }

protected:
    std::shared_ptr<FonteDeEntrada> fonte;
    double velocidade;
    bool ativo = true;
};

}