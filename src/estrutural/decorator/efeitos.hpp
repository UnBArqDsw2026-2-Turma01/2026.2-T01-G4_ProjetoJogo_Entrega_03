#ifndef G4_EFEITOS_EFEITOS_HPP
#define G4_EFEITOS_EFEITOS_HPP

#include "efeito_decorator.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace G4Efeitos {

// soma um valor (positivo ou negativo) a um atributo
class ModificadorDeAtributo : public EfeitoDecorator {
public:
    ModificadorDeAtributo(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone,
            Atributo p_alvo, int p_delta) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), alvo(p_alvo), delta(p_delta) {}

    int atributo(Atributo a) const override {
        const int valor = envolvido->atributo(a);
        return a == alvo ? valor + delta : valor;
    }

private:
    Atributo alvo;
    int delta;
};

// multiplica o dano recebido de um elemento (fator 0 = imunidade)
class Resistencia : public EfeitoDecorator {
public:
    Resistencia(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone,
            Elemento p_elemento, float p_fator) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), elemento(p_elemento), fator(p_fator) {}

    float resistencia(Elemento e) const override {
        const float valor = envolvido->resistencia(e);
        return e == elemento ? valor * fator : valor;
    }

private:
    Elemento elemento;
    float fator;
};

// altera o HP no início de cada turno de quem tem o efeito.
class EfeitoPorTurno : public EfeitoDecorator {
public:
    EfeitoPorTurno(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone,
            int p_deltaHp, Elemento p_elemento) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), deltaHp(p_deltaHp), elemento(p_elemento) {}

    void aoIniciarTurno(Personagem &topo) override {
        int delta = deltaHp;
        if (delta < 0) {
            delta = -static_cast<int>(std::lround(-delta * topo.resistencia(elemento)));
        }
        topo.alterarHp(delta);
        EfeitoDecorator::aoIniciarTurno(topo);
    }

private:
    int deltaHp;
    Elemento elemento;
};

// não deixa agir e acorda ao receber dano.
class Sono : public EfeitoDecorator {
public:
    using EfeitoDecorator::EfeitoDecorator;

    bool podeAgir() const override { return false; }

    void aoReceberDano(int dano) override {
        if (dano > 0) {
            duracao.esgotar();
        }
        envolvido->aoReceberDano(dano);
    }
};

// sorteia o alvo das ações
class Confusao : public EfeitoDecorator {
public:
    Confusao(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone,
            unsigned p_semente = std::random_device{}()) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), sorteio(p_semente) {}

    int escolherAlvo(int alvos, int pretendido) const override {
        if (alvos <= 1) {
            return envolvido->escolherAlvo(alvos, pretendido);
        }
        return std::uniform_int_distribution<int>(0, alvos - 1)(sorteio);
    }

private:
    mutable std::mt19937 sorteio;
};

// não pode ser escolhido como alvo
class Invisibilidade : public EfeitoDecorator {
public:
    using EfeitoDecorator::EfeitoDecorator;

    bool podeSerAlvo() const override { return false; }
};

// esquiva (qualquer ataque) ou reflete (só magia) o próximo ataque e acaba
class ReacaoUnica : public EfeitoDecorator {
public:
    ReacaoUnica(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone, Reacao p_reacao) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), reacao(p_reacao) {}

    Reacao reagirAoAtaque(const Ataque &ataque) override {
        const bool serve = reacao == Reacao::ESQUIVOU || (reacao == Reacao::REFLETIU && ataque.magico);
        if (duracao.esgotada() || !serve) {
            return envolvido->reagirAoAtaque(ataque);
        }
        duracao.esgotar();
        return reacao;
    }

private:
    Reacao reacao;
};

// troca o elemento dos ataques
class ArmaElemental : public EfeitoDecorator {
public:
    ArmaElemental(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone, Elemento p_elemento) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), elemento(p_elemento) {}

    Elemento elementoDoAtaque() const override { return elemento; }

private:
    Elemento elemento;
};

// aumenta o raio de visão na escuridão
class RaioDeLuz : public EfeitoDecorator {
public:
    RaioDeLuz(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone, float p_raio) :
            EfeitoDecorator(std::move(p_envolvido), p_duracao, std::move(p_icone)), raio(p_raio) {}

    float raioDeLuz() const override { return std::max(envolvido->raioDeLuz(), raio); }

private:
    float raio;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_EFEITOS_HPP
