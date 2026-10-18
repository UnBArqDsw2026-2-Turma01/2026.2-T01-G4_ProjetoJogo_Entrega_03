#ifndef G4_EFEITOS_PERSONAGEM_HPP
#define G4_EFEITOS_PERSONAGEM_HPP

#include "tipos.hpp"

#include <string>
#include <vector>

namespace G4Efeitos {

class Personagem {
public:
    virtual ~Personagem() = default;

    virtual std::string nome() const = 0;
    virtual int atributo(Atributo a) const = 0;
    virtual float resistencia(Elemento e) const = 0;
    virtual Elemento elementoDoAtaque() const = 0;
    virtual bool podeAgir() const = 0;
    virtual bool podeSerAlvo() const = 0;
    virtual int escolherAlvo(int alvos, int pretendido) const = 0;
    virtual float raioDeLuz() const = 0;

    virtual Reacao reagirAoAtaque(const Ataque &ataque) = 0;
    virtual void aoIniciarTurno(Personagem &topo) = 0;
    virtual void aoReceberDano(int dano) = 0;
    virtual void aoSaquear() = 0;

    virtual int hp() const = 0;
    virtual int mp() const = 0;
    virtual void alterarHp(int delta) = 0;
    virtual void alterarMp(int delta) = 0;

    virtual std::vector<std::string> efeitosAtivos() const = 0;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_PERSONAGEM_HPP
