#pragma once

#include "inimigo.hpp"
#include <map>

class CatalogoDeInimigos {
public:
    void registrar(const std::string& chave, const Inimigo& prototipo);
    std::unique_ptr<Inimigo> criar(const std::string& chave) const;
    const Inimigo& modelo(const std::string& chave) const;

private:
    std::map<std::string, std::unique_ptr<Inimigo>> modelos_;
};

CatalogoDeInimigos criarCatalogo();
