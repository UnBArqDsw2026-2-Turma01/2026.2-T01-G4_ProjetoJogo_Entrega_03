#include "clientes.hpp"

#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

// Garantias verificadas na compilação: fora da classe, não há como criar,
// copiar, mover nem atribuir uma Partida. Se alguma regra for removida do
// cabeçalho, este arquivo deixa de compilar.
static_assert(!std::is_default_constructible_v<Partida>, "construtor deve ser privado");
static_assert(!std::is_copy_constructible_v<Partida>, "cópia deve estar proibida");
static_assert(!std::is_move_constructible_v<Partida>, "movimento deve estar proibido");
static_assert(!std::is_copy_assignable_v<Partida>, "atribuição deve estar proibida");
static_assert(!std::is_move_assignable_v<Partida>, "atribuição por movimento deve estar proibida");
static_assert(!std::is_destructible_v<Partida>, "destrutor deve ser privado");

namespace {
void exigir(bool condicao, const char* mensagem) {
    if (!condicao) throw std::runtime_error(mensagem);
}
template<class Excecao> void rejeita(const std::function<void()>& acao) {
    try { acao(); } catch (const Excecao&) { return; }
    throw std::runtime_error("a operação deveria ter sido recusada");
}
Partida& novoJogo() {
    Partida& partida = Partida::instancia();
    partida.iniciar("Alquimista", "Floresta");
    return partida;
}
}

int main() {
    int passou = 0, total = 0;
    auto teste = [&](const char* nome, const std::function<void()>& caso) {
        ++total;
        try { caso(); ++passou; std::cout << "OK  " << nome << '\n'; }
        catch (const std::exception& erro) { std::cout << "ERRO " << nome << ": " << erro.what() << '\n'; }
    };

    teste("Todas as chamadas de instancia() devolvem o mesmo objeto", [] {
        Partida* primeira = &Partida::instancia();
        for (int i = 0; i < 1000; ++i) exigir(&Partida::instancia() == primeira, "endereço mudou");
    });
    teste("Clientes independentes alteram a mesma partida", [] {
        Partida& partida = novoJogo();
        Exploracao().entrarEm("Caverna");
        BancadaDeAlquimia().criarPocao("Poção Flamejante");
        Combate().derrotar("Cogumelo Andante");
        exigir(partida.area() == "Caverna", "área não chegou à partida");
        exigir(partida.pocoesCriadas() == 1, "poção não chegou à partida");
        exigir(partida.inimigosDerrotados() == 1, "vitória não chegou à partida");
    });
    teste("Salvamento reúne o progresso feito por todos os clientes", [] {
        novoJogo();
        Exploracao().entrarEm("Caverna");
        BancadaDeAlquimia().criarPocao("Poção Flamejante");
        BancadaDeAlquimia().criarPocao("Poção de Cura Menor");
        Combate().derrotar("Gólem de Enxofre");
        Salvamento s = MenuDePausa().salvar();
        exigir(s.jogador == "Alquimista" && s.area == "Caverna", "dados do jogador incorretos");
        exigir(s.pocoesCriadas == 2 && s.inimigosDerrotados == 1, "progresso incompleto");
        exigir(s.origem == "Menu de pausa", "origem incorreta");
    });
    teste("Salvamento antigo não muda com o progresso posterior", [] {
        Partida& partida = novoJogo();
        Salvamento antes = MenuDePausa().salvar();
        Combate().derrotar("Morcego");
        Exploracao().entrarEm("Caverna");
        exigir(antes.inimigosDerrotados == 0 && antes.area == "Floresta", "cópia alterada");
        exigir(partida.salvamentos().front().inimigosDerrotados == 0, "histórico alterado");
    });
    teste("Novo jogo recomeça o progresso na mesma instância", [] {
        Partida* antes = &novoJogo();
        Combate().derrotar("Cogumelo Andante");
        MenuDePausa().salvar();
        Partida& depois = Partida::instancia();
        depois.iniciar("Outra alquimista", "Caverna");
        exigir(&depois == antes, "novo jogo criou outro objeto");
        exigir(depois.inimigosDerrotados() == 0 && depois.salvamentos().empty(), "progresso mantido");
        exigir(depois.jogador() == "Outra alquimista" && depois.area() == "Caverna", "dados iniciais");
    });
    teste("Salvar e sair encerra a partida e recusa novas ações", [] {
        Partida& partida = novoJogo();
        Salvamento s = MenuDePausa().salvarESair();
        exigir(partida.estado() == EstadoDaPartida::Encerrada, "partida continua em andamento");
        exigir(s.origem == "Menu de pausa (salvar e sair)", "origem incorreta");
        rejeita<std::logic_error>([] { Exploracao().entrarEm("Floresta"); });
        rejeita<std::logic_error>([] { MenuDePausa().salvar(); });
        exigir(partida.area() == "Floresta" && partida.salvamentos().size() == 1, "estado alterado");
    });
    teste("Entradas vazias são recusadas sem alterar o progresso", [] {
        Partida& partida = novoJogo();
        rejeita<std::invalid_argument>([] { Exploracao().entrarEm(""); });
        rejeita<std::invalid_argument>([] { BancadaDeAlquimia().criarPocao(""); });
        rejeita<std::invalid_argument>([] { Combate().derrotar(""); });
        rejeita<std::invalid_argument>([&] { partida.iniciar("", "Floresta"); });
        exigir(partida.area() == "Floresta" && partida.pocoesCriadas() == 0, "estado alterado");
        exigir(partida.inimigosDerrotados() == 0, "estado alterado");
    });
    teste("Oito threads diferentes recebem a mesma instância", [] {
        std::vector<const Partida*> enderecos(8, nullptr);
        std::vector<std::thread> threads;
        for (std::size_t i = 0; i < enderecos.size(); ++i)
            threads.emplace_back([&enderecos, i] { enderecos[i] = &Partida::instancia(); });
        for (auto& t : threads) t.join();
        exigir(std::set<const Partida*>(enderecos.begin(), enderecos.end()).size() == 1,
               "threads receberam objetos diferentes");
    });

    std::cout << passou << "/" << total << " testes aprovados.\n";
    return passou == total ? 0 : 1;
}
