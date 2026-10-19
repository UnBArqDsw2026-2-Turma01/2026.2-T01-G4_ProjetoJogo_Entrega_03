#include "dominio.h"

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>

namespace G4Combate {

namespace {

// Combinações que existem no jogo: fórmula -> efeito.
const std::map<std::string, Efeito> &receitas() {
    static const std::map<std::string, Efeito> tabela = {
        { "H2O", Efeito(TipoEfeito::GELO, 12) },
        { "CO2", Efeito(TipoEfeito::GELO, 9) },
        { "CH4", Efeito(TipoEfeito::FOGO, 15) },
        { "H2S", Efeito(TipoEfeito::VENENO, 10) },
    };
    return tabela;
}

std::string formulaDe(const std::vector<Elemento> &elementos) {
    std::map<std::string, int> contagem; // já fica em ordem alfabética
    for (const Elemento &elemento : elementos) {
        ++contagem[elemento.getSimbolo()];
    }

    std::string formula;
    auto escrever = [&](const std::string &simbolo) {
        formula += simbolo;
        if (contagem[simbolo] > 1) {
            formula += std::to_string(contagem[simbolo]);
        }
    };

    const bool temCarbono = contagem.count("C") > 0;
    if (temCarbono) {
        escrever("C");
        if (contagem.count("H") > 0) {
            escrever("H");
        }
    }
    for (const auto &par : contagem) {
        const std::string &simbolo = par.first;
        if (temCarbono && (simbolo == "C" || simbolo == "H")) {
            continue;
        }
        escrever(simbolo);
    }
    return formula;
}

} // namespace

std::string nomeDoTipo(TipoEfeito tipo) {
    switch (tipo) {
        case TipoEfeito::FOGO:
            return "FOGO";
        case TipoEfeito::GELO:
            return "GELO";
        case TipoEfeito::VENENO:
            return "VENENO";
    }
    return "?";
}

// --- Elemento e Efeito ---

Elemento::Elemento(std::string p_simbolo, std::string p_nome, int p_numeroAtomico) :
        simbolo(std::move(p_simbolo)), nome(std::move(p_nome)), numeroAtomico(p_numeroAtomico) {}

const std::string &Elemento::getSimbolo() const { return simbolo; }
const std::string &Elemento::getNome() const { return nome; }
int Elemento::getNumeroAtomico() const { return numeroAtomico; }

Efeito::Efeito(TipoEfeito p_tipo, int p_danoBase) :
        tipo(p_tipo), danoBase(p_danoBase) {}

TipoEfeito Efeito::getTipo() const { return tipo; }
int Efeito::getDanoBase() const { return danoBase; }

// --- Combinacao ---

Combinacao::Combinacao(std::vector<Elemento> p_elementos) :
        elementos(std::move(p_elementos)), formula(formulaDe(elementos)) {}

const std::string &Combinacao::getFormula() const { return formula; }
const std::vector<Elemento> &Combinacao::getElementos() const { return elementos; }
bool Combinacao::ehValida() const { return receitas().count(formula) > 0; }
Efeito Combinacao::gerarEfeito() const { return receitas().at(formula); }

// --- Inventário e diário ---

bool InventarioElementos::adicionar(const Elemento &elemento) {
    if (possui(elemento)) {
        return false;
    }
    elementos.push_back(elemento);
    return true;
}

bool InventarioElementos::possui(const Elemento &elemento) const {
    return std::any_of(elementos.begin(), elementos.end(), [&](const Elemento &guardado) {
        return guardado.getSimbolo() == elemento.getSimbolo();
    });
}

const std::vector<Elemento> &InventarioElementos::listarElementos() const { return elementos; }

bool DiarioAventureiro::registrarCombinacao(const Combinacao &combinacao) {
    if (conhece(combinacao)) {
        return false;
    }
    combinacoes.push_back(combinacao);
    return true;
}

bool DiarioAventureiro::conhece(const Combinacao &combinacao) const {
    return std::any_of(combinacoes.begin(), combinacoes.end(), [&](const Combinacao &registrada) {
        return registrada.getFormula() == combinacao.getFormula();
    });
}

const std::vector<Combinacao> &DiarioAventureiro::listarCombinacoes() const { return combinacoes; }

// --- Personagens ---

Personagem::Personagem(std::string p_nome) :
        nome(std::move(p_nome)) {}

const std::string &Personagem::getNome() const { return nome; }

Combatente::Combatente(std::string p_nome, int p_vidaMaxima, int p_ataque, int p_defesa) :
        Personagem(std::move(p_nome)), vidaAtual(p_vidaMaxima), vidaMaxima(p_vidaMaxima), ataque(p_ataque), defesa(p_defesa) {}

int Combatente::getVidaAtual() const { return vidaAtual; }
int Combatente::getVidaMaxima() const { return vidaMaxima; }
int Combatente::getAtaque() const { return ataque; }
int Combatente::getDefesa() const { return defesa; }

void Combatente::receberDano(int valor) {
    vidaAtual = std::max(0, vidaAtual - std::max(0, valor));
}

bool Combatente::estaVivo() const { return vidaAtual > 0; }

std::string textoDeVida(const Combatente &combatente) {
    return std::to_string(combatente.getVidaAtual()) + "/" + std::to_string(combatente.getVidaMaxima());
}

Jogador::Jogador(std::string p_nome, int p_vidaMaxima, int p_ataque, int p_defesa) :
        Combatente(std::move(p_nome), p_vidaMaxima, p_ataque, p_defesa) {}

Combinacao Jogador::misturarElementos(const std::vector<Elemento> &elementos) const {
    if (elementos.size() < 2) {
        throw std::invalid_argument("Uma combinação precisa de pelo menos dois elementos.");
    }
    for (const Elemento &elemento : elementos) {
        if (!inventario.possui(elemento)) {
            throw std::invalid_argument("O elemento " + elemento.getSimbolo() + " não está no inventário.");
        }
    }
    return Combinacao(elementos);
}

InventarioElementos &Jogador::getInventario() { return inventario; }
const InventarioElementos &Jogador::getInventario() const { return inventario; }
DiarioAventureiro &Jogador::getDiario() { return diario; }
const DiarioAventureiro &Jogador::getDiario() const { return diario; }

Inimigo::Inimigo(std::string p_nome, int p_vidaMaxima, int p_ataque, int p_defesa, TipoEfeito p_fraqueza) :
        Combatente(std::move(p_nome), p_vidaMaxima, p_ataque, p_defesa), fraqueza(p_fraqueza) {}

TipoEfeito Inimigo::getFraqueza() const { return fraqueza; }

float Inimigo::calcularMultiplicador(const Efeito &efeito) const {
    return efeito.getTipo() == fraqueza ? 2.0f : 1.0f;
}

void Inimigo::atacar(Jogador &alvo) const {
    alvo.receberDano(std::max(1, getAtaque() - alvo.getDefesa()));
}

} // namespace G4Combate
