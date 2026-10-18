#ifndef G4_EFEITOS_COMBATENTE_HPP
#define G4_EFEITOS_COMBATENTE_HPP

#include "atributos_base.hpp"
#include "efeito_decorator.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <utility>

namespace G4Efeitos {

struct ResultadoAtaque {
    Reacao reacao = Reacao::NENHUMA;
    int dano = 0;
};

class Combatente {
public:
    using ComEfeito = std::function<std::unique_ptr<Personagem>(std::unique_ptr<Personagem>)>;

    explicit Combatente(std::unique_ptr<AtributosBase> p_base) :
            base(p_base.get()), cadeia(std::move(p_base)) {}

    Personagem &personagem() { return *cadeia; }
    const Personagem &personagem() const { return *cadeia; }

    void decorar(const ComEfeito &comEfeito) { cadeia = comEfeito(std::move(cadeia)); }

    template <class Efeito, class... Args>
    void decorar(Args &&...args) {
        cadeia = std::make_unique<Efeito>(std::move(cadeia), std::forward<Args>(args)...);
    }

    int removerEfeitos(const EfeitoDecorator::Filtro &filtro) {
        const size_t antes = cadeia->efeitosAtivos().size();
        cadeia = EfeitoDecorator::removerEfeitos(std::move(cadeia), filtro);
        return static_cast<int>(antes - cadeia->efeitosAtivos().size());
    }

    int removerEfeitos(const std::string &icone) {
        return removerEfeitos([&icone](const EfeitoDecorator &e) { return e.getIcone() == icone; });
    }

    void passarTurno() {
        cadeia->aoIniciarTurno(*cadeia);
        removerExpirados();
    }

    void saquear() {
        cadeia->aoSaquear();
        removerExpirados();
    }

    Ataque atacar() const {
        return Ataque{ cadeia->atributo(Atributo::FORCA), cadeia->elementoDoAtaque(), false };
    }

    ResultadoAtaque receberAtaque(const Ataque &ataque) {
        ResultadoAtaque resultado;
        resultado.reacao = cadeia->reagirAoAtaque(ataque);
        if (resultado.reacao == Reacao::NENHUMA) {
            const int defesa = cadeia->atributo(ataque.magico ? Atributo::DEFESA_MAGICA : Atributo::DEFESA);
            const float fator = cadeia->resistencia(ataque.elemento);
            const int bruto = std::max(1, ataque.dano - defesa);
            resultado.dano = static_cast<int>(std::lround(bruto * fator));
            cadeia->alterarHp(-resultado.dano);
            cadeia->aoReceberDano(resultado.dano);
        }
        removerExpirados();
        return resultado;
    }

    void aumentarBase(Atributo a, int delta) { base->aumentarBase(a, delta); }
    void restaurar() {
        removerEfeitos([](const EfeitoDecorator &) { return true; });
        base->restaurar();
    }

    bool derrotado() const { return cadeia->hp() <= 0; }
    int hpMaximo() const { return cadeia->atributo(Atributo::HP_MAXIMO); }
    int mpMaximo() const { return cadeia->atributo(Atributo::MP_MAXIMO); }

private:
    void removerExpirados() {
        cadeia = EfeitoDecorator::removerEfeitos(std::move(cadeia), [](const EfeitoDecorator &e) { return e.expirou(); });
    }

    AtributosBase *base;
    std::unique_ptr<Personagem> cadeia;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_COMBATENTE_HPP
