#ifndef G4_EFEITOS_EFEITO_DECORATOR_HPP
#define G4_EFEITOS_EFEITO_DECORATOR_HPP

#include "personagem.hpp"

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace G4Efeitos {

class EfeitoDecorator : public Personagem {
public:
    using Filtro = std::function<bool(const EfeitoDecorator &)>;

    EfeitoDecorator(std::unique_ptr<Personagem> p_envolvido, Duracao p_duracao, std::string p_icone) :
            envolvido(std::move(p_envolvido)), duracao(p_duracao), icone(std::move(p_icone)) {}

    std::string nome() const override { return envolvido->nome(); }
    int atributo(Atributo a) const override { return envolvido->atributo(a); }
    float resistencia(Elemento e) const override { return envolvido->resistencia(e); }
    Elemento elementoDoAtaque() const override { return envolvido->elementoDoAtaque(); }
    bool podeAgir() const override { return envolvido->podeAgir(); }
    bool podeSerAlvo() const override { return envolvido->podeSerAlvo(); }
    int escolherAlvo(int alvos, int pretendido) const override { return envolvido->escolherAlvo(alvos, pretendido); }
    float raioDeLuz() const override { return envolvido->raioDeLuz(); }
    Reacao reagirAoAtaque(const Ataque &ataque) override { return envolvido->reagirAoAtaque(ataque); }
    void aoReceberDano(int dano) override { envolvido->aoReceberDano(dano); }
    int hp() const override { return envolvido->hp(); }
    int mp() const override { return envolvido->mp(); }
    void alterarHp(int delta) override { envolvido->alterarHp(delta); }
    void alterarMp(int delta) override { envolvido->alterarMp(delta); }

    void aoIniciarTurno(Personagem &topo) override {
        duracao.consumir(Duracao::Unidade::TURNOS);
        envolvido->aoIniciarTurno(topo);
    }
    void aoSaquear() override {
        duracao.consumir(Duracao::Unidade::SAQUES);
        envolvido->aoSaquear();
    }
    std::vector<std::string> efeitosAtivos() const override {
        std::vector<std::string> icones = envolvido->efeitosAtivos();
        icones.push_back(icone);
        return icones;
    }

    bool expirou() const { return duracao.esgotada(); }
    const std::string &getIcone() const { return icone; }
    const Duracao &getDuracao() const { return duracao; }

    static std::unique_ptr<Personagem> removerEfeitos(std::unique_ptr<Personagem> cadeia, const Filtro &remover) {
        auto *efeito = dynamic_cast<EfeitoDecorator *>(cadeia.get());
        if (!efeito) {
            return cadeia;
        }
        efeito->envolvido = removerEfeitos(std::move(efeito->envolvido), remover);
        if (remover(*efeito)) {
            return std::move(efeito->envolvido);
        }
        return cadeia;
    }

protected:
    std::unique_ptr<Personagem> envolvido;
    Duracao duracao;
    std::string icone;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_EFEITO_DECORATOR_HPP
