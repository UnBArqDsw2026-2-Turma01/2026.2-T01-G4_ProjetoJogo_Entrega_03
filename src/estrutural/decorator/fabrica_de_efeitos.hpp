#ifndef G4_EFEITOS_FABRICA_DE_EFEITOS_HPP
#define G4_EFEITOS_FABRICA_DE_EFEITOS_HPP

#include "combatente.hpp"
#include "efeitos.hpp"

#include <cstdlib>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace G4Efeitos {

struct EspecificacaoEfeito {
    std::string tipo;
    std::string alvo = "si";
    Atributo atributo = Atributo::FORCA;
    Elemento elemento = Elemento::FISICO;
    int valor = 0;
    float fator = 1.0f;
    Reacao reacao = Reacao::NENHUMA;
    std::string remover;
    Duracao duracao = Duracao::permanente();
    std::string icone;

    bool ehDecorador() const {
        return tipo == "modificador" || tipo == "resistencia" || tipo == "por_turno" || tipo == "sono" ||
                tipo == "confusao" || tipo == "invisibilidade" || tipo == "reacao" || tipo == "arma_elemental" ||
                tipo == "luz";
    }

    static EspecificacaoEfeito deCampos(const std::map<std::string, std::string> &campos, std::string &erro) {
        EspecificacaoEfeito e;
        erro.clear();
        auto campo = [&campos](const char *chave) {
            auto it = campos.find(chave);
            return it == campos.end() ? std::string() : it->second;
        };
        e.tipo = campo("tipo");
        if (!campo("alvo").empty()) e.alvo = campo("alvo");
        e.icone = campo("icone");
        e.remover = campo("remover");
        e.valor = std::atoi(campo("valor").c_str());
        if (!campo("fator").empty()) e.fator = static_cast<float>(std::atof(campo("fator").c_str()));

        bool ok = true;
        if (!campo("atributo").empty()) {
            e.atributo = atributoDeTexto(campo("atributo"), ok);
            if (!ok) erro = "atributo desconhecido: " + campo("atributo");
        }
        e.elemento = elementoDeTexto(campo("elemento"), ok);
        if (!ok) erro = "elemento desconhecido: " + campo("elemento");

        if (campo("reacao") == "esquiva") e.reacao = Reacao::ESQUIVOU;
        else if (campo("reacao") == "reflexo") e.reacao = Reacao::REFLETIU;

        if (!campo("turnos").empty()) e.duracao = Duracao::turnos(std::atoi(campo("turnos").c_str()));
        else if (!campo("saques").empty()) e.duracao = Duracao::saques(std::atoi(campo("saques").c_str()));
        else if (campo("duracao") == "uso_unico") e.duracao = Duracao::usoUnico();

        static const char *tipos[] = { "modificador", "resistencia", "por_turno", "sono", "confusao",
            "invisibilidade", "reacao", "arma_elemental", "luz", "cura_hp", "cura_mp", "aumentar_base", "dano",
            "revelar_mapa", "remover_efeitos" };
        bool conhecido = false;
        for (const char *t : tipos) conhecido = conhecido || e.tipo == t;
        if (!conhecido) erro = "tipo de efeito desconhecido: " + e.tipo;
        if (e.alvo != "si" && e.alvo != "inimigo" && e.alvo != "todos_inimigos") erro = "alvo desconhecido: " + e.alvo;
        if (e.ehDecorador() && e.icone.empty()) erro = "efeito " + e.tipo + " sem ícone";
        if (e.tipo == "reacao" && e.reacao == Reacao::NENHUMA) erro = "reação sem tipo (esquiva ou reflexo)";
        if (e.tipo == "remover_efeitos" && e.remover.empty()) erro = "remover_efeitos sem o campo remover";
        return e;
    }
};

struct ResultadoEfeito {
    bool aplicado = false;
    std::string mensagem;
    int valor = 0;
    bool revelarMapa = false;
    Reacao reacao = Reacao::NENHUMA;
};

class FabricaDeEfeitos {
public:
    explicit FabricaDeEfeitos(unsigned semente = std::random_device{}()) : sorteio(semente) {}

    ResultadoEfeito aplicar(const EspecificacaoEfeito &espec, Combatente &alvo, const Combatente *usuario = nullptr) {
        if (!espec.ehDecorador()) {
            return aplicarInstantaneo(espec, alvo, usuario);
        }
        ResultadoEfeito r;
        const std::string nome = alvo.personagem().nome();
        if (espec.tipo == "por_turno" && espec.valor < 0 && alvo.personagem().resistencia(espec.elemento) == 0.0f) {
            r.mensagem = nome + " é imune a " + nomeDe(espec.elemento) + ".";
            return r;
        }
        alvo.decorar(criarDecorador(espec));
        r.aplicado = true;
        r.mensagem = nome + descrever(espec) + sufixoDuracao(espec.duracao) + ".";
        return r;
    }

    using Aplicacao = std::pair<const Combatente *, ResultadoEfeito>;
    std::vector<Aplicacao> aplicarPocao(const std::vector<EspecificacaoEfeito> &efeitos, Combatente &usuario,
            const std::vector<Combatente *> &inimigos, Combatente *alvoEscolhido) {
        std::vector<Aplicacao> resultados;
        for (const EspecificacaoEfeito &e : efeitos) {
            std::vector<Combatente *> alvos;
            if (e.alvo == "si") {
                alvos.push_back(&usuario);
            } else if (e.alvo == "inimigo") {
                if (alvoEscolhido) alvos.push_back(alvoEscolhido);
            } else {
                for (Combatente *c : inimigos) {
                    if (!c->derrotado()) alvos.push_back(c);
                }
            }
            for (Combatente *c : alvos) {
                resultados.emplace_back(c, aplicar(e, *c, &usuario));
            }
        }
        return resultados;
    }

    static bool precisaDeInimigo(const std::vector<EspecificacaoEfeito> &efeitos) {
        for (const EspecificacaoEfeito &e : efeitos) {
            if (e.alvo != "si") return true;
        }
        return false;
    }

    static bool precisaEscolherAlvo(const std::vector<EspecificacaoEfeito> &efeitos) {
        for (const EspecificacaoEfeito &e : efeitos) {
            if (e.alvo == "inimigo") return true;
        }
        return false;
    }

private:
    Combatente::ComEfeito criarDecorador(const EspecificacaoEfeito &espec) {
        const EspecificacaoEfeito e = espec;
        const unsigned semente = sorteio();
        return [e, semente](std::unique_ptr<Personagem> p) -> std::unique_ptr<Personagem> {
            if (e.tipo == "modificador") return std::make_unique<ModificadorDeAtributo>(std::move(p), e.duracao, e.icone, e.atributo, e.valor);
            if (e.tipo == "resistencia") return std::make_unique<Resistencia>(std::move(p), e.duracao, e.icone, e.elemento, e.fator);
            if (e.tipo == "por_turno") return std::make_unique<EfeitoPorTurno>(std::move(p), e.duracao, e.icone, e.valor, e.elemento);
            if (e.tipo == "sono") return std::make_unique<Sono>(std::move(p), e.duracao, e.icone);
            if (e.tipo == "confusao") return std::make_unique<Confusao>(std::move(p), e.duracao, e.icone, semente);
            if (e.tipo == "invisibilidade") return std::make_unique<Invisibilidade>(std::move(p), e.duracao, e.icone);
            if (e.tipo == "reacao") return std::make_unique<ReacaoUnica>(std::move(p), e.duracao, e.icone, e.reacao);
            if (e.tipo == "arma_elemental") return std::make_unique<ArmaElemental>(std::move(p), e.duracao, e.icone, e.elemento);
            return std::make_unique<RaioDeLuz>(std::move(p), e.duracao, e.icone, static_cast<float>(e.valor));
        };
    }

    ResultadoEfeito aplicarInstantaneo(const EspecificacaoEfeito &e, Combatente &alvo, const Combatente *usuario) {
        ResultadoEfeito r;
        r.aplicado = true;
        Personagem &p = alvo.personagem();
        const std::string nome = p.nome();
        if (e.tipo == "cura_hp" || e.tipo == "cura_mp") {
            const bool hp = e.tipo == "cura_hp";
            const int antes = hp ? p.hp() : p.mp();
            hp ? p.alterarHp(e.valor) : p.alterarMp(e.valor);
            r.valor = (hp ? p.hp() : p.mp()) - antes;
            r.mensagem = nome + " recupera " + std::to_string(r.valor) + (hp ? " HP." : " MP.");
        } else if (e.tipo == "aumentar_base") {
            alvo.aumentarBase(e.atributo, e.valor);
            r.valor = e.valor;
            r.mensagem = nome + ": " + nomeDe(e.atributo) + " +" + std::to_string(e.valor) + " para sempre.";
        } else if (e.tipo == "dano") {
            const int bonus = usuario ? usuario->personagem().atributo(Atributo::DANO_MAGICO) : 0;
            const ResultadoAtaque ataque = alvo.receberAtaque(Ataque{ e.valor + bonus, e.elemento, true });
            r.reacao = ataque.reacao;
            r.valor = ataque.dano;
            if (ataque.reacao == Reacao::ESQUIVOU) {
                r.mensagem = nome + " esquivou.";
            } else if (ataque.reacao == Reacao::REFLETIU) {
                r.mensagem = nome + " refletiu a poção.";
            } else {
                r.mensagem = nome + " sofre " + textoDeDano(ataque.dano, e.elemento) + ".";
            }
        } else if (e.tipo == "revelar_mapa") {
            r.revelarMapa = true;
            r.mensagem = "Os segredos do mapa foram revelados.";
        } else if (e.tipo == "remover_efeitos") {
            const int removidos = e.remover == "todos"
                    ? alvo.removerEfeitos([](const EfeitoDecorator &) { return true; })
                    : alvo.removerEfeitos(e.remover);
            r.valor = removidos;
            r.aplicado = removidos > 0;
            r.mensagem = removidos > 0 ? nome + " perde " + std::to_string(removidos) + (removidos == 1 ? " efeito." : " efeitos.")
                                       : nome + " não tinha efeitos para remover.";
        }
        return r;
    }

    static std::string sinal(int v) { return v >= 0 ? "+" + std::to_string(v) : std::to_string(v); }

    static std::string descrever(const EspecificacaoEfeito &e) {
        if (e.tipo == "modificador") return std::string(": ") + nomeDe(e.atributo) + " " + sinal(e.valor);
        if (e.tipo == "resistencia") {
            return e.fator == 0.0f ? std::string(" fica imune a ") + nomeDe(e.elemento)
                                   : std::string(" resiste a ") + nomeDe(e.elemento);
        }
        if (e.tipo == "por_turno") {
            return e.valor >= 0 ? " regenera " + std::to_string(e.valor) + " HP por turno"
                                : " sofre " + std::to_string(-e.valor) + " de " + nomeDe(e.elemento) + " por turno";
        }
        if (e.tipo == "sono") return " adormece";
        if (e.tipo == "confusao") return " fica confuso";
        if (e.tipo == "invisibilidade") return " fica invisível";
        if (e.tipo == "reacao") return e.reacao == Reacao::ESQUIVOU ? " vai esquivar do próximo ataque" : " vai refletir a próxima magia";
        if (e.tipo == "arma_elemental") return std::string(": ataques de ") + nomeDe(e.elemento);
        return " enxerga melhor no escuro";
    }

    static std::string sufixoDuracao(const Duracao &d) {
        const int n = d.getRestante();
        switch (d.getUnidade()) {
            case Duracao::Unidade::TURNOS: return " por " + std::to_string(n) + (n == 1 ? " turno" : " turnos");
            case Duracao::Unidade::SAQUES: return " nos próximos " + std::to_string(n) + " saques";
            default: return "";
        }
    }

    std::mt19937 sorteio; // semente da Confusão
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_FABRICA_DE_EFEITOS_HPP
