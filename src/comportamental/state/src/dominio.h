#pragma once

// Classes do domínio do combate, tiradas do diagrama de classes da SubEquipe 03
// (versão 1.3). Não fazem parte do padrão State: são os dados que os estados
// manipulam. Ficaram só os atributos e as operações que o combate usa.

#include <string>
#include <vector>

namespace G4Combate {

enum class TipoEfeito {
    FOGO,
    GELO,
    VENENO,
};

std::string nomeDoTipo(TipoEfeito tipo);

// Elemento da tabela periódica, por exemplo H (Hidrogênio).
class Elemento {
public:
    Elemento(std::string simbolo, std::string nome, int numeroAtomico);

    const std::string &getSimbolo() const;
    const std::string &getNome() const;
    int getNumeroAtomico() const;

private:
    std::string simbolo;
    std::string nome;
    int numeroAtomico;
};

class Efeito {
public:
    Efeito(TipoEfeito tipo, int danoBase);

    TipoEfeito getTipo() const;
    int getDanoBase() const;

private:
    TipoEfeito tipo;
    int danoBase;
};

// Mistura de dois ou mais elementos. A fórmula segue o sistema de Hill (com
// carbono, C e H vêm primeiro; o resto fica em ordem alfabética), então
// H + H + O e O + H + H formam a mesma combinação: H2O.
class Combinacao {
public:
    explicit Combinacao(std::vector<Elemento> elementos);

    const std::string &getFormula() const;
    const std::vector<Elemento> &getElementos() const;
    bool ehValida() const;
    // Só deve ser chamada para combinações válidas.
    Efeito gerarEfeito() const;

private:
    std::vector<Elemento> elementos;
    std::string formula;
};

class InventarioElementos {
public:
    bool adicionar(const Elemento &elemento);
    bool possui(const Elemento &elemento) const;
    const std::vector<Elemento> &listarElementos() const;

private:
    std::vector<Elemento> elementos;
};

// Histórico de magias (RF08): guarda só combinações válidas, sem repetir.
class DiarioAventureiro {
public:
    bool registrarCombinacao(const Combinacao &combinacao);
    bool conhece(const Combinacao &combinacao) const;
    const std::vector<Combinacao> &listarCombinacoes() const;

private:
    std::vector<Combinacao> combinacoes;
};

// Abstrata: o construtor protegido impede criar um personagem genérico.
class Personagem {
public:
    virtual ~Personagem() = default;

    const std::string &getNome() const;

protected:
    explicit Personagem(std::string nome);

private:
    std::string nome;
};

// Abstrata: reúne o que só faz sentido para quem participa de combates.
class Combatente : public Personagem {
public:
    int getVidaAtual() const;
    int getVidaMaxima() const;
    int getAtaque() const;
    int getDefesa() const;
    void receberDano(int valor);
    bool estaVivo() const;

protected:
    Combatente(std::string nome, int vidaMaxima, int ataque, int defesa);

private:
    int vidaAtual;
    int vidaMaxima;
    int ataque;
    int defesa;
};

// Vida no formato "atual/máxima", usada nas mensagens do combate.
std::string textoDeVida(const Combatente &combatente);

class Jogador : public Combatente {
public:
    Jogador(std::string nome, int vidaMaxima, int ataque, int defesa);

    // RF05 e RF09: mistura só elementos que estão no inventário.
    Combinacao misturarElementos(const std::vector<Elemento> &elementos) const;

    InventarioElementos &getInventario();
    const InventarioElementos &getInventario() const;
    DiarioAventureiro &getDiario();
    const DiarioAventureiro &getDiario() const;

private:
    InventarioElementos inventario;
    DiarioAventureiro diario;
};

class Inimigo : public Combatente {
public:
    Inimigo(std::string nome, int vidaMaxima, int ataque, int defesa, TipoEfeito fraqueza);

    TipoEfeito getFraqueza() const;
    // RF06: o dano dobra quando o efeito é a fraqueza do inimigo.
    float calcularMultiplicador(const Efeito &efeito) const;
    void atacar(Jogador &alvo) const;

private:
    TipoEfeito fraqueza;
};

} // namespace G4Combate
