#include "inimigo.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

Inimigo::Inimigo(std::string nome, std::string sprite, std::string fraqueza,
                 int vida_maxima, std::vector<Ataque> ataques)
    : nome_(std::move(nome)), sprite_(std::move(sprite)), fraqueza_(std::move(fraqueza)),
      vida_maxima_(vida_maxima), vida_(vida_maxima), ataques_(std::move(ataques)) {
    if (nome_.empty() || sprite_.empty() || fraqueza_.empty() || vida_maxima_ <= 0 || ataques_.empty()) {
        throw std::invalid_argument("Dados do inimigo inválidos.");
    }
    for (const auto& ataque : ataques_) {
        if (ataque.nome.empty() || ataque.dano < 0) throw std::invalid_argument("Ataque inválido.");
    }
}

void Inimigo::receberDano(int quantidade) {
    if (quantidade < 0) throw std::invalid_argument("O dano deve ser não negativo.");
    vida_ = std::max(0, vida_ - quantidade);
}

void Inimigo::fortalecerAtaque(std::size_t indice, int aumento) {
    if (aumento < 0) throw std::invalid_argument("O aumento deve ser não negativo.");
    auto& ataque = ataques_.at(indice);
    if (aumento > std::numeric_limits<int>::max() - ataque.dano) {
        throw std::overflow_error("O dano excede o limite inteiro.");
    }
    ataque.dano += aumento;
}

InimigoElemental::InimigoElemental(std::string nome, std::string sprite, std::string fraqueza,
                                   int vida_maxima, std::vector<Ataque> ataques)
    : Inimigo(std::move(nome), std::move(sprite), std::move(fraqueza), vida_maxima, std::move(ataques)) {}

std::unique_ptr<Inimigo> InimigoElemental::clonar() const {
    // O construtor de cópia copia strings, o vector e cada Ataque por valor.
    return std::make_unique<InimigoElemental>(*this);
}
