#pragma once

#include "catalogo.hpp"
#include <iosfwd>

enum class Comando { Modelo = 1, Criar, Selecionar, Dano, Fortalecer, ClonarCopia, Reiniciar };

// Estado da demonstração, compartilhado pelos clientes terminal e navegador.
class Demonstracao {
public:
    void executar(Comando comando, int argumento = 0);
    std::string json() const;
    void imprimir(std::ostream& saida) const;

private:
    CatalogoDeInimigos catalogo_ = criarCatalogo();
    std::string chave_ = "cogumelo";
    std::vector<std::unique_ptr<Inimigo>> copias_;
    int selecionada_ = -1;
    std::string mensagem_ = "Escolha um modelo e crie duas cópias para comparar.";
};
