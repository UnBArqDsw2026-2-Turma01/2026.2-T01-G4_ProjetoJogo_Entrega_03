#pragma once

#include <string>
#include <vector>

enum class EstadoDaPartida { NaoIniciada, EmAndamento, Encerrada };

std::string nomeDoEstado(EstadoDaPartida estado);

// Fotografia do progresso no momento do salvamento (RF03). É guardada por
// valor: mudanças posteriores na partida não alteram um salvamento antigo.
struct Salvamento {
    std::string jogador;
    std::string area;
    int pocoesCriadas;
    int inimigosDerrotados;
    std::string origem;
};

// Singleton: a sessão de jogo em execução. Mundo, bancada de alquimia,
// combate e menu de pausa precisam enxergar e salvar o mesmo progresso.
class Partida {
public:
    static Partida& instancia();

    // Sem cópia nem atribuição: não há como surgir uma segunda partida a partir
    // da primeira. Como o construtor de cópia foi declarado, o compilador também
    // não gera construtor nem atribuição de movimento.
    Partida(const Partida&) = delete;
    Partida& operator=(const Partida&) = delete;

    // Começa um novo jogo. A instância continua a mesma; só o progresso recomeça.
    void iniciar(const std::string& jogador, const std::string& areaInicial);
    void mudarArea(const std::string& area);
    void registrarPocao(const std::string& pocao);
    void registrarVitoria(const std::string& inimigo);
    Salvamento salvar(const std::string& origem);
    void sair();

    EstadoDaPartida estado() const;
    const std::string& jogador() const;
    const std::string& area() const;
    int pocoesCriadas() const;
    int inimigosDerrotados() const;
    const std::vector<Salvamento>& salvamentos() const;

private:
    // Privados: só instancia() cria o objeto, e ninguém consegue destruí-lo
    // por fora (por exemplo, com delete &Partida::instancia()).
    Partida() = default;
    ~Partida() = default;

    void exigirEmAndamento() const;

    EstadoDaPartida estado_ = EstadoDaPartida::NaoIniciada;
    std::string jogador_;
    std::string area_;
    int pocoesCriadas_ = 0;
    int inimigosDerrotados_ = 0;
    std::vector<Salvamento> salvamentos_;
};
