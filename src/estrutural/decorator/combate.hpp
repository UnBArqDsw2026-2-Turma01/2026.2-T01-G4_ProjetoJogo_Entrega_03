#ifndef G4_EFEITOS_COMBATE_HPP
#define G4_EFEITOS_COMBATE_HPP

#include "fabrica_de_efeitos.hpp"

#include <algorithm>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace G4Efeitos {

struct Inimigo {
    std::string id;
    std::unique_ptr<Combatente> combatente;
    int chanceEspecial = 0;
    std::string nomeEspecial;
    bool especialEhAtaque = false;
    Ataque ataqueEspecial;
    EspecificacaoEfeito efeitoEspecial;
};

struct Evento {
    enum class Tipo {
        VEZ,         
        POR_TURNO,   
        NAO_AGE,    
        ATAQUE,
        MAGIA,
        ESPECIAL,
        DANO,
        ERROU,
        ESQUIVOU,
        REFLETIU,
        SEM_ALVO,
        POCAO,
        EFEITO, 
        DERROTADO,
        VITORIA,
        DERROTA,
        FUGIU,
        FUGA_FALHOU,
        AVISO,
    };
    Tipo tipo;
    int origem = -1;
    int alvo = -1;
    int valor = 0;
    Elemento elemento = Elemento::FISICO;
    std::string texto;
};

class Combate {
public:
    enum class Resultado { EM_ANDAMENTO, VITORIA, DERROTA, FUGA };
    static constexpr int CUSTO_MAGIA = 6;
    static constexpr int DANO_MAGIA = 8;

    Combate(Combatente &p_jogador, std::vector<Inimigo> p_inimigos, unsigned semente = std::random_device{}()) :
            jogador(p_jogador), inimigos(std::move(p_inimigos)), sorteio(semente), fabrica(semente + 1) {}

    int quantidade() const { return 1 + static_cast<int>(inimigos.size()); }
    Combatente &participante(int i) { return i == 0 ? jogador : *inimigos[i - 1].combatente; }
    const Combatente &participante(int i) const { return i == 0 ? jogador : *inimigos[i - 1].combatente; }
    const Inimigo &inimigo(int i) const { return inimigos[i - 1]; }
    int indiceDe(const Combatente *c) const {
        for (int i = 0; i < quantidade(); i++) {
            if (&participante(i) == c) return i;
        }
        return -1;
    }

    Resultado resultado() const { return estado; }
    bool terminou() const { return estado != Resultado::EM_ANDAMENTO; }
    bool aguardandoJogador() const { return !terminou() && jogadorPodeAgir; }
    int vezAtual() const { return atual; }

    std::vector<Combatente *> inimigosVivos() {
        std::vector<Combatente *> vivos;
        for (Inimigo &i : inimigos) {
            if (!i.combatente->derrotado()) vivos.push_back(i.combatente.get());
        }
        return vivos;
    }

    std::vector<int> alvosDoJogador() const {
        std::vector<int> alvos;
        for (int i = 1; i < quantidade(); i++) {
            if (!participante(i).derrotado() && participante(i).personagem().podeSerAlvo()) alvos.push_back(i);
        }
        return alvos;
    }

    std::vector<Evento> avancar() {
        std::vector<Evento> eventos;
        if (terminou() || jogadorPodeAgir) {
            return eventos;
        }
        do {
            if (++posicao >= static_cast<int>(ordem.size())) {
                novaRodada();
                posicao = 0;
            }
            atual = ordem[posicao];
        } while (participante(atual).derrotado());

        Combatente &c = participante(atual);
        eventos.push_back({ Evento::Tipo::VEZ, atual, -1, 0, Elemento::FISICO, c.personagem().nome() });
        const int hpAntes = c.personagem().hp();
        c.passarTurno();
        const int delta = c.personagem().hp() - hpAntes;
        if (delta != 0) {
            eventos.push_back({ Evento::Tipo::POR_TURNO, -1, atual, delta, Elemento::FISICO,
                    c.personagem().nome() + (delta > 0 ? " recupera " + std::to_string(delta) + " HP." : " sofre " + std::to_string(-delta) + " de dano.") });
            if (c.derrotado()) {
                eventos.push_back({ Evento::Tipo::DERROTADO, -1, atual, 0, Elemento::FISICO, c.personagem().nome() + " foi derrotado." });
                derrotadosAnunciados.push_back(atual);
                verificarFim(eventos);
                return eventos;
            }
        }
        if (!c.personagem().podeAgir()) {
            eventos.push_back({ Evento::Tipo::NAO_AGE, atual, -1, 0, Elemento::FISICO, c.personagem().nome() + " está dormindo." });
            return eventos;
        }
        if (atual == 0) {
            jogadorPodeAgir = true;
        } else {
            agirInimigo(atual, eventos);
            verificarFim(eventos);
        }
        return eventos;
    }

    std::vector<Evento> jogadorAtaca(int alvo) {
        std::vector<Evento> eventos;
        if (!aguardandoJogador()) return eventos;
        eventos.push_back({ Evento::Tipo::ATAQUE, 0, alvo, 0, Elemento::FISICO, jogador.personagem().nome() + " ataca." });
        resolverAtaque(0, jogador.atacar(), alvo, eventos);
        encerrarAcaoDoJogador(eventos);
        return eventos;
    }

    std::vector<Evento> jogadorUsaMagia(int alvo) {
        std::vector<Evento> eventos;
        if (!aguardandoJogador()) return eventos;
        Personagem &p = jogador.personagem();
        if (p.mp() < CUSTO_MAGIA) {
            eventos.push_back({ Evento::Tipo::AVISO, 0, -1, 0, Elemento::FISICO, "MP insuficiente para a magia." });
            return eventos;
        }
        p.alterarMp(-CUSTO_MAGIA);
        const Ataque raio{ DANO_MAGIA + p.atributo(Atributo::DANO_MAGICO), Elemento::ELETRICO, true };
        eventos.push_back({ Evento::Tipo::MAGIA, 0, alvo, 0, Elemento::ELETRICO, p.nome() + " lança um raio arcano." });
        resolverAtaque(0, raio, alvo, eventos);
        encerrarAcaoDoJogador(eventos);
        return eventos;
    }

    std::vector<Evento> jogadorUsouPocao(const std::string &nomePocao, const std::vector<FabricaDeEfeitos::Aplicacao> &aplicacoes) {
        std::vector<Evento> eventos;
        if (!aguardandoJogador()) return eventos;
        eventos.push_back({ Evento::Tipo::POCAO, 0, -1, 0, Elemento::FISICO, jogador.personagem().nome() + " usa " + nomePocao + "." });
        for (const auto &[combatente, r] : aplicacoes) {
            const int alvo = indiceDe(combatente);
            eventos.push_back({ Evento::Tipo::EFEITO, 0, alvo, r.valor, Elemento::FISICO, r.mensagem });
        }
        for (int i = 1; i < quantidade(); i++) {
            if (participante(i).derrotado() && !anunciado(i)) {
                eventos.push_back({ Evento::Tipo::DERROTADO, -1, i, 0, Elemento::FISICO, participante(i).personagem().nome() + " foi derrotado." });
                derrotadosAnunciados.push_back(i);
            }
        }
        encerrarAcaoDoJogador(eventos);
        return eventos;
    }

    std::vector<Evento> jogadorFoge() {
        std::vector<Evento> eventos;
        if (!aguardandoJogador()) return eventos;
        int maisRapido = 0;
        for (Combatente *c : inimigosVivos()) {
            maisRapido = std::max(maisRapido, c->personagem().atributo(Atributo::VELOCIDADE));
        }
        const int chance = std::clamp(50 + (jogador.personagem().atributo(Atributo::VELOCIDADE) - maisRapido) * 10, 10, 90);
        if (sortear(chance)) {
            eventos.push_back({ Evento::Tipo::FUGIU, 0, -1, 0, Elemento::FISICO, "Fuga bem-sucedida!" });
            encerrar(Resultado::FUGA);
        } else {
            eventos.push_back({ Evento::Tipo::FUGA_FALHOU, 0, -1, 0, Elemento::FISICO, "Não conseguiu fugir!" });
        }
        jogadorPodeAgir = false;
        return eventos;
    }

private:
    void novaRodada() {
        ordem.clear();
        for (int i = 0; i < quantidade(); i++) {
            if (!participante(i).derrotado()) ordem.push_back(i);
        }

        std::stable_sort(ordem.begin(), ordem.end(), [this](int a, int b) {
            return participante(a).personagem().atributo(Atributo::VELOCIDADE) >
                    participante(b).personagem().atributo(Atributo::VELOCIDADE);
        });
    }

    bool sortear(int chance) { return std::uniform_int_distribution<int>(0, 99)(sorteio) < chance; }

    void agirInimigo(int i, std::vector<Evento> &eventos) {
        Inimigo &ini = inimigos[i - 1];
        const std::string nome = ini.combatente->personagem().nome();
        if (ini.chanceEspecial > 0 && sortear(ini.chanceEspecial)) {
            eventos.push_back({ Evento::Tipo::ESPECIAL, i, 0, 0, ini.ataqueEspecial.elemento, nome + " usa " + ini.nomeEspecial + "!" });
            if (ini.especialEhAtaque) {
                resolverAtaque(i, ini.ataqueEspecial, 0, eventos);
            } else if (jogador.personagem().podeSerAlvo()) {
                const ResultadoEfeito r = fabrica.aplicar(ini.efeitoEspecial, jogador, ini.combatente.get());
                eventos.push_back({ Evento::Tipo::EFEITO, i, 0, r.valor, Elemento::FISICO, r.mensagem });
            } else {
                eventos.push_back({ Evento::Tipo::SEM_ALVO, i, -1, 0, Elemento::FISICO, nome + " não encontra o alvo." });
            }
            return;
        }
        eventos.push_back({ Evento::Tipo::ATAQUE, i, 0, 0, Elemento::FISICO, nome + " ataca." });
        resolverAtaque(i, ini.combatente->atacar(), 0, eventos);
    }

    int escolherAlvo(int origem, int pretendido) {
        std::vector<int> candidatos;
        for (int i = 0; i < quantidade(); i++) {
            const bool aliado = (origem == 0) == (i == 0);
            if (i == origem || participante(i).derrotado() || !participante(i).personagem().podeSerAlvo()) continue;
            if (origem == 0 && aliado) continue;
            candidatos.push_back(i);
        }
        int indicePretendido = -1;
        for (size_t k = 0; k < candidatos.size(); k++) {
            if (candidatos[k] == pretendido) indicePretendido = static_cast<int>(k);
        }
        if (candidatos.empty()) return -1;
        const int escolhido = participante(origem).personagem().escolherAlvo(static_cast<int>(candidatos.size()), indicePretendido);
        return escolhido < 0 ? -1 : candidatos[escolhido];
    }

    void resolverAtaque(int origem, Ataque ataque, int pretendido, std::vector<Evento> &eventos) {
        Combatente &atacante = participante(origem);
        const int alvo = escolherAlvo(origem, pretendido);
        if (alvo < 0) {
            eventos.push_back({ Evento::Tipo::SEM_ALVO, origem, -1, 0, Elemento::FISICO, atacante.personagem().nome() + " não encontra o alvo." });
            return;
        }
        Combatente &defensor = participante(alvo);
        if (alvo != pretendido) {
            eventos.push_back({ Evento::Tipo::AVISO, origem, alvo, 0, Elemento::FISICO, atacante.personagem().nome() + ", confuso, ataca " + defensor.personagem().nome() + "!" });
        }
        if (!ataque.magico && !sortear(std::clamp(atacante.personagem().atributo(Atributo::PRECISAO), 5, 100))) {
            eventos.push_back({ Evento::Tipo::ERROU, origem, alvo, 0, ataque.elemento, atacante.personagem().nome() + " errou." });
            return;
        }
        const ResultadoAtaque r = defensor.receberAtaque(ataque);
        if (r.reacao == Reacao::ESQUIVOU) {
            eventos.push_back({ Evento::Tipo::ESQUIVOU, origem, alvo, 0, ataque.elemento, defensor.personagem().nome() + " esquivou!" });
            return;
        }
        if (r.reacao == Reacao::REFLETIU) {
            eventos.push_back({ Evento::Tipo::REFLETIU, origem, alvo, 0, ataque.elemento, defensor.personagem().nome() + " refletiu a magia!" });
            const ResultadoAtaque volta = atacante.receberAtaque(ataque);
            registrarDano(origem, volta.dano, ataque.elemento, eventos);
            return;
        }
        registrarDano(alvo, r.dano, ataque.elemento, eventos);
    }

    void registrarDano(int alvo, int dano, Elemento elemento, std::vector<Evento> &eventos) {
        Combatente &c = participante(alvo);
        eventos.push_back({ Evento::Tipo::DANO, -1, alvo, dano, elemento,
                c.personagem().nome() + " sofre " + textoDeDano(dano, elemento) + "." });
        if (c.derrotado() && !anunciado(alvo)) {
            eventos.push_back({ Evento::Tipo::DERROTADO, -1, alvo, 0, Elemento::FISICO, c.personagem().nome() + " foi derrotado." });
            derrotadosAnunciados.push_back(alvo);
        }
    }

    bool anunciado(int i) const {
        return std::find(derrotadosAnunciados.begin(), derrotadosAnunciados.end(), i) != derrotadosAnunciados.end();
    }

    void encerrarAcaoDoJogador(std::vector<Evento> &eventos) {
        jogadorPodeAgir = false;
        verificarFim(eventos);
    }

    void verificarFim(std::vector<Evento> &eventos) {
        if (terminou()) return;
        if (jogador.derrotado()) {
            eventos.push_back({ Evento::Tipo::DERROTA, -1, 0, 0, Elemento::FISICO, jogador.personagem().nome() + " foi derrotado..." });
            encerrar(Resultado::DERROTA);
        } else if (inimigosVivos().empty()) {
            eventos.push_back({ Evento::Tipo::VITORIA, -1, -1, 0, Elemento::FISICO, "Vitória!" });
            encerrar(Resultado::VITORIA);
        }
    }

    void encerrar(Resultado r) {
        estado = r;
        jogadorPodeAgir = false;
        jogador.removerEfeitos([](const EfeitoDecorator &e) {
            const auto u = e.getDuracao().getUnidade();
            return u == Duracao::Unidade::TURNOS || u == Duracao::Unidade::USO_UNICO;
        });
    }

    Combatente &jogador;
    std::vector<Inimigo> inimigos;
    std::mt19937 sorteio;
    FabricaDeEfeitos fabrica;
    std::vector<int> ordem;
    std::vector<int> derrotadosAnunciados;
    int posicao = -1;
    int atual = -1;
    bool jogadorPodeAgir = false;
    Resultado estado = Resultado::EM_ANDAMENTO;
};

} // namespace G4Efeitos

#endif // G4_EFEITOS_COMBATE_HPP
