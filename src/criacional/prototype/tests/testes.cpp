#include "demonstracao.hpp"
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace {
void exigir(bool condicao) {
    if (!condicao) throw std::runtime_error("Resultado diferente do esperado");
}
template<class Excecao> void rejeita(const std::function<void()>& acao) {
    try { acao(); } catch (const Excecao&) { return; }
    throw std::runtime_error("A operação deveria ter sido recusada");
}
InimigoElemental exemplo() {
    return {"Cogumelo", "cogumelo", "fogo", 24, {{"Investida", 5}, {"Esporos", 3}}};
}
}

int main() {
    int passou = 0, total = 0;
    auto teste = [&](const char* nome, const std::function<void()>& caso) {
        ++total;
        try { caso(); ++passou; std::cout << "OK  " << nome << '\n'; }
        catch (const std::exception& erro) { std::cerr << "ERRO " << nome << ": " << erro.what() << '\n'; }
    };

    teste("Identidade distinta, mesmo tipo concreto e atributos", [] {
        auto original = exemplo();
        auto copia = original.clonar();
        exigir(copia.get() != &original);
        exigir(dynamic_cast<InimigoElemental*>(copia.get()) != nullptr);
        exigir(copia->nome() == original.nome() && copia->sprite() == original.sprite());
        exigir(copia->fraqueza() == original.fraqueza() && copia->vidaMaxima() == 24);
        exigir(copia->vida() == 24 && copia->ataques().size() == 2);
    });
    teste("Dano afeta apenas a cópia escolhida", [] {
        auto original = exemplo(); auto a = original.clonar(); auto b = original.clonar();
        a->receberDano(7);
        exigir(a->vida() == 17 && b->vida() == 24 && original.vida() == 24);
    });
    teste("Lista e cada ataque têm armazenamento independente", [] {
        auto original = exemplo(); auto a = original.clonar(); auto b = original.clonar();
        exigir(a->ataques().data() != original.ataques().data());
        a->fortalecerAtaque(0, 2); a->fortalecerAtaque(1, 4);
        exigir(a->ataques()[0].dano == 7 && a->ataques()[1].dano == 7);
        exigir(b->ataques()[0].dano == 5 && b->ataques()[1].dano == 3);
        exigir(original.ataques()[0].dano == 5 && original.ataques()[1].dano == 3);
    });
    teste("Clonagem preserva estado alterado e mantém isolamento", [] {
        auto a = exemplo(); a.receberDano(7); a.fortalecerAtaque(0, 2);
        auto b = a.clonar(); exigir(b->vida() == 17 && b->ataques()[0].dano == 7);
        b->receberDano(7); b->fortalecerAtaque(0, 2);
        exigir(b->vida() == 10 && b->ataques()[0].dano == 9);
        exigir(a.vida() == 17 && a.ataques()[0].dano == 7);
    });
    teste("Valores fornecidos ao construtor são copiados", [] {
        std::vector<Ataque> ataques = {{"Investida", 5}};
        InimigoElemental a("A", "a", "fogo", 24, ataques);
        ataques[0].dano = 999;
        exigir(a.ataques()[0].dano == 5);
    });
    teste("Catálogo isola o objeto recebido e as cópias produzidas", [] {
        CatalogoDeInimigos catalogo; auto original = exemplo();
        catalogo.registrar("a", original); original.receberDano(20); original.fortalecerAtaque(0, 10);
        auto a = catalogo.criar("a"); a->receberDano(24); a->fortalecerAtaque(1, 20);
        auto b = catalogo.criar("a");
        exigir(b->vida() == 24 && b->ataques()[0].dano == 5 && b->ataques()[1].dano == 3);
        exigir(catalogo.modelo("a").vida() == 24);
    });
    teste("Catálogo aceita outro tipo pelo contrato de clonagem", [] {
        class OutroInimigo final : public Inimigo {
        public:
            OutroInimigo() : Inimigo("Outro", "outro", "água", 42, {{"Golpe", 2}}) {}
            std::unique_ptr<Inimigo> clonar() const override { return std::make_unique<OutroInimigo>(*this); }
        };
        CatalogoDeInimigos catalogo; OutroInimigo original; catalogo.registrar("outro", original);
        auto copia = catalogo.criar("outro");
        exigir(dynamic_cast<OutroInimigo*>(copia.get()) != nullptr && copia->vida() == 42);
    });
    teste("Vida limita em zero e clone de derrotado permanece derrotado", [] {
        auto a = exemplo(); a.receberDano(std::numeric_limits<int>::max());
        exigir(a.vida() == 0 && a.clonar()->vida() == 0);
    });
    teste("Operações inválidas preservam o estado", [] {
        auto a = exemplo();
        rejeita<std::invalid_argument>([&] { a.receberDano(-1); });
        rejeita<std::invalid_argument>([&] { a.fortalecerAtaque(0, -1); });
        rejeita<std::out_of_range>([&] { a.fortalecerAtaque(9, 2); });
        rejeita<std::overflow_error>([&] { a.fortalecerAtaque(0, std::numeric_limits<int>::max()); });
        exigir(a.vida() == 24 && a.ataques()[0].dano == 5);
    });
    teste("Construtor rejeita vida, nomes e ataques inválidos", [] {
        rejeita<std::invalid_argument>([] { InimigoElemental a("A", "a", "fogo", 0, {{"Golpe", 5}}); });
        rejeita<std::invalid_argument>([] { InimigoElemental a("", "a", "fogo", 24, {{"Golpe", 5}}); });
        rejeita<std::invalid_argument>([] { InimigoElemental a("A", "a", "fogo", 24, {}); });
        rejeita<std::invalid_argument>([] { InimigoElemental a("A", "a", "fogo", 24, {{"Golpe", -1}}); });
    });
    teste("Catálogo rejeita chave vazia, repetida e desconhecida", [] {
        auto catalogo = criarCatalogo(); auto a = exemplo();
        rejeita<std::invalid_argument>([&] { catalogo.registrar("", a); });
        rejeita<std::invalid_argument>([&] { catalogo.registrar("cogumelo", a); });
        rejeita<std::out_of_range>([&] { catalogo.criar("desconhecido"); });
    });
    teste("Modelos configurados preservam os valores iniciais", [] {
        auto catalogo = criarCatalogo();
        for (const auto& [chave, vida] : std::vector<std::pair<std::string, int>>{{"cogumelo", 24}, {"golem", 36}, {"morcego", 18}}) {
            auto a = catalogo.criar(chave); a->receberDano(7);
            exigir(catalogo.criar(chave)->vida() == vida);
        }
    });
    teste("Demonstração valida seleção, comando, limite e reinício", [] {
        Demonstracao demo;
        const auto antes = demo.json();
        rejeita<std::logic_error>([&] { demo.executar(Comando::Dano); });
        rejeita<std::out_of_range>([&] { demo.executar(Comando::Selecionar, -1); });
        rejeita<std::out_of_range>([&] { demo.executar(Comando::Modelo, 99); });
        rejeita<std::invalid_argument>([&] { demo.executar(static_cast<Comando>(99)); });
        exigir(demo.json() == antes);
        for (int i = 0; i < 4; ++i) demo.executar(Comando::Criar);
        const auto cheio = demo.json();
        rejeita<std::length_error>([&] { demo.executar(Comando::Criar); });
        rejeita<std::length_error>([&] { demo.executar(Comando::ClonarCopia); });
        exigir(demo.json() == cheio);
        demo.executar(Comando::Reiniciar);
        exigir(demo.json().find("\"copias\":[]") != std::string::npos);
    });
    static_assert(std::has_virtual_destructor_v<Inimigo>);
    static_assert(std::is_same_v<decltype(std::declval<const CatalogoDeInimigos&>().modelo("a")), const Inimigo&>);
    std::cout << passou << '/' << total << " testes aprovados.\n";
    return passou == total ? 0 : 1;
}
