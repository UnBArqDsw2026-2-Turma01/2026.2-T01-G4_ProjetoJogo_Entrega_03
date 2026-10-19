#pragma once
#include "bridge/fonte_de_entrada.hpp"
#include <godot_cpp/classes/input.hpp>
#include <cmath>

namespace jogo {

// Usa as ações já definidas em Mundo::criarAcoes(), então WASD e setas
// continuam funcionando sem alteração.
class EntradaTeclado : public bridge::FonteDeEntrada {
public:
    bridge::Direcao lerDirecao() const override {
        const godot::Vector2 v = godot::Input::get_singleton()->get_vector(
                "mover_esquerda", "mover_direita", "mover_cima", "mover_baixo");
        return {v.x, v.y};
    }
};

// Analógico esquerdo do controle, com zona morta.
class EntradaControle : public bridge::FonteDeEntrada {
public:
    explicit EntradaControle(int dispositivo = 0) : dispositivo(dispositivo) {}

    bridge::Direcao lerDirecao() const override {
        godot::Input *in = godot::Input::get_singleton();
        return {zona(in->get_joy_axis(dispositivo, godot::JOY_AXIS_LEFT_X)),
                zona(in->get_joy_axis(dispositivo, godot::JOY_AXIS_LEFT_Y))};
    }

private:
    static double zona(double v) { return std::fabs(v) < 0.2 ? 0.0 : v; }
    int dispositivo;
};

} 