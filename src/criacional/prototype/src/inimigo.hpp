#pragma once

#include <memory>
#include <string>
#include <vector>

struct Ataque {
    std::string nome;
    int dano;
};

// Prototype: a criação polimórfica passa por clonar().
class Inimigo {
public:
    virtual ~Inimigo() = default;
    virtual std::unique_ptr<Inimigo> clonar() const = 0;

    const std::string& nome() const { return nome_; }
    const std::string& sprite() const { return sprite_; }
    const std::string& fraqueza() const { return fraqueza_; }
    int vida() const { return vida_; }
    int vidaMaxima() const { return vida_maxima_; }
    const std::vector<Ataque>& ataques() const { return ataques_; }

    void receberDano(int quantidade);
    void fortalecerAtaque(std::size_t indice, int aumento);

protected:
    Inimigo(std::string nome, std::string sprite, std::string fraqueza,
            int vida_maxima, std::vector<Ataque> ataques);
    Inimigo(const Inimigo&) = default;
    Inimigo& operator=(const Inimigo&) = default;

private:
    std::string nome_;
    std::string sprite_;
    std::string fraqueza_;
    int vida_maxima_;
    int vida_;
    std::vector<Ataque> ataques_;
};

class InimigoElemental final : public Inimigo {
public:
    InimigoElemental(std::string nome, std::string sprite, std::string fraqueza,
                     int vida_maxima, std::vector<Ataque> ataques);
    std::unique_ptr<Inimigo> clonar() const override;
};
