#ifndef G4_EFEITOS_ATRIBUTOS_BASE_HPP
#define G4_EFEITOS_ATRIBUTOS_BASE_HPP

#include "personagem.hpp"

#include <algorithm>
#include <map>
#include <string>
#include <utility>

namespace G4Efeitos {

class AtributosBase : public Personagem {
public:
    AtributosBase(std::string p_nome, std::map<Atributo, int> p_base,
            std::map<Elemento, float> p_resistencias = {}, float p_raioBase = 0.0f) :
            nomeBase(std::move(p_nome)), base(std::move(p_base)),
            resistenciasBase(std::move(p_resistencias)), raioBase(p_raioBase) {
        hpAtual = valorBase(Atributo::HP_MAXIMO);
        mpAtual = valorBase(Atributo::MP_MAXIMO);
    }

    std::string nome() const override { return nomeBase; }
    int atributo(Atributo a) const override { return valorBase(a); }
    float resistencia(Elemento e) const override {
        auto it = resistenciasBase.find(e);
        return it == resistenciasBase.end() ? 1.0f : it->second;
    }
    Elemento elementoDoAtaque() const override { return Elemento::FISICO; }
    bool podeAgir() const override { return true; }
    bool podeSerAlvo() const override { return true; }
    int escolherAlvo(int, int pretendido) const override { return pretendido; }
    float raioDeLuz() const override { return raioBase; }

    Reacao reagirAoAtaque(const Ataque &) override { return Reacao::NENHUMA; }
    void aoIniciarTurno(Personagem &) override {}
    void aoReceberDano(int) override {}
    void aoSaquear() override {}

    int hp() const override { return hpAtual; }
    int mp() const override { return mpAtual; }
    void alterarHp(int delta) override { hpAtual = std::clamp(hpAtual + delta, 0, valorBase(Atributo::HP_MAXIMO)); }
    void alterarMp(int delta) override { mpAtual = std::clamp(mpAtual + delta, 0, valorBase(Atributo::MP_MAXIMO)); }

    std::vector<std::string> efeitosAtivos() const override { return {}; }

    // mudança permanente
    void aumentarBase(Atributo a, int delta) {
        base[a] = valorBase(a) + delta;
        if (a == Atributo::HP_MAXIMO) {
            alterarHp(delta);
        } else if (a == Atributo::MP_MAXIMO) {
            alterarMp(delta);
        }
    }

    // volta ao estado inicial de HP e MP
    void restaurar() {
        hpAtual = valorBase(Atributo::HP_MAXIMO);
        mpAtual = valorBase(Atributo::MP_MAXIMO);
    }

private:
    int valorBase(Atributo a) const {
        auto it = base.find(a);
        return it == base.end() ? 0 : it->second;
    }

    std::string nomeBase;
    std::map<Atributo, int> base;
    std::map<Elemento, float> resistenciasBase;
    float raioBase;
    int hpAtual = 0;
    int mpAtual = 0;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_ATRIBUTOS_BASE_HPP
