#include "catalogo.hpp"
#include <stdexcept>

void CatalogoDeInimigos::registrar(const std::string& chave, const Inimigo& prototipo) {
    if (chave.empty()) throw std::invalid_argument("Informe a chave do modelo.");
    if (modelos_.count(chave)) throw std::invalid_argument("Modelo já registrado.");
    modelos_.emplace(chave, prototipo.clonar());
}

const Inimigo& CatalogoDeInimigos::modelo(const std::string& chave) const {
    return *modelos_.at(chave);
}

std::unique_ptr<Inimigo> CatalogoDeInimigos::criar(const std::string& chave) const {
    return modelo(chave).clonar();
}

CatalogoDeInimigos criarCatalogo() {
    CatalogoDeInimigos catalogo;
    catalogo.registrar("cogumelo", InimigoElemental("Cogumelo Andante", "cogumelo", "fogo", 24, {{"Investida", 5}}));
    catalogo.registrar("golem", InimigoElemental("Gólem de Enxofre", "golem", "eletricidade", 36, {{"Bafo Sulfuroso", 9}}));
    catalogo.registrar("morcego", InimigoElemental("Morcego da Caverna", "morcego", "luz", 18, {{"Mordida", 5}}));
    return catalogo;
}
