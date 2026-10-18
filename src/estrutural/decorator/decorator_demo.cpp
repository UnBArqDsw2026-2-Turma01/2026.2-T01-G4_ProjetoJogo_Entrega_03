#include "combate.hpp"
#include "json_minimo.hpp"

#include <iostream>

using namespace G4Efeitos;

static std::map<std::string, std::vector<EspecificacaoEfeito>> pocoes;
static std::map<std::string, std::string> nomes;

static void carregarPocoes(const std::string &caminho) {
    Json::Valor dados;
    if (!Json::lerArquivo(caminho, dados)) {
        std::cout << "Não consegui ler " << caminho << "\n";
        std::exit(1);
    }
    for (const Json::Valor &receita : dados["receitas"].lista) {
        const Json::Valor &pocao = receita["pocao"];
        nomes[pocao["id"].texto] = pocao["nome"].texto;
        for (const Json::Valor &efeito : pocao["efeitos"].lista) {
            std::string erro;
            pocoes[pocao["id"].texto].push_back(Json::especificacao(efeito, erro));
        }
    }
}

static void titulo(const std::string &texto) {
    std::cout << "\n" << std::string(75, '-') << "\n" << texto << "\n" << std::string(75, '-') << "\n";
}

static void mostrarCadeia(const Combatente &c) {
    const auto efeitos = c.personagem().efeitosAtivos();
    std::cout << "   cadeia: Combatente -> ";
    for (auto it = efeitos.rbegin(); it != efeitos.rend(); ++it) std::cout << "[" << *it << "] -> ";
    std::cout << "AtributosBase\n";
}

static void mostrarAtributos(const Combatente &c) {
    const Personagem &p = c.personagem();
    std::cout << "   " << p.nome() << ": HP " << p.hp() << "/" << c.hpMaximo()
              << " | força " << p.atributo(Atributo::FORCA)
              << " | defesa " << p.atributo(Atributo::DEFESA)
              << " | velocidade " << p.atributo(Atributo::VELOCIDADE)
              << " | precisão " << p.atributo(Atributo::PRECISAO) << "\n";
}

static void beber(FabricaDeEfeitos &fabrica, const std::string &id, Combatente &usuario,
        std::vector<Combatente *> inimigos = {}, Combatente *alvo = nullptr) {
    std::cout << ">> " << usuario.personagem().nome() << " usa " << nomes[id] << "\n";
    for (const auto &[c, r] : fabrica.aplicarPocao(pocoes[id], usuario, inimigos, alvo)) {
        std::cout << "   " << r.mensagem << "\n";
    }
}

static std::unique_ptr<Combatente> criar(const std::string &nome, std::map<Atributo, int> base,
        std::map<Elemento, float> resistencias = {}) {
    return std::make_unique<Combatente>(std::make_unique<AtributosBase>(nome, std::move(base), std::move(resistencias), 40.0f));
}

int main(int argc, char **argv) {
    carregarPocoes(argc > 1 ? argv[1] : "../data/alquimia.json");
    FabricaDeEfeitos fabrica(2026);

    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << " G4_ProjetoJogo: DEMONSTRAÇÃO DO PADRÃO GOF DECORATOR (C++)\n";
    std::cout << " SubEquipe 02: Marcos Vinícius Gündel da Silva (@MarcosViniciusG)\n";
    std::cout << " Efeitos das poções decorando o personagem em tempo de execução\n";
    std::cout << std::string(75, '=') << "\n";

    titulo("[PASSO 1] O personagem sem efeitos (ConcreteComponent)");
    auto jogador = criar("Alquimista", { { Atributo::HP_MAXIMO, 50 }, { Atributo::MP_MAXIMO, 30 }, { Atributo::FORCA, 7 },
            { Atributo::DEFESA, 2 }, { Atributo::DEFESA_MAGICA, 2 }, { Atributo::VELOCIDADE, 4 }, { Atributo::PRECISAO, 90 } });
    mostrarCadeia(*jogador);
    mostrarAtributos(*jogador);

    titulo("[PASSO 2] Um efeito por cima do outro: Elixir da Quimera e Poção de Pele de Pedra");
    beber(fabrica, "elixir_da_quimera", *jogador);
    beber(fabrica, "pocao_de_pele_de_pedra", *jogador);
    mostrarCadeia(*jogador);
    mostrarAtributos(*jogador);
    std::cout << "   atributo(FORCA) atravessa a cadeia: 7 da base + 2 da Quimera = "
              << jogador->personagem().atributo(Atributo::FORCA) << "\n";

    titulo("[PASSO 3] Veneno, e o Antídoto bebido DEPOIS dele");
    jogador->decorar<EfeitoPorTurno>(Duracao::turnos(3), "veneno", -4, Elemento::VENENO);
    std::cout << ">> Os esporos do Cogumelo Andante envenenam o Alquimista\n";
    jogador->passarTurno();
    std::cout << "   início do turno: o veneno tira 4 HP -> HP " << jogador->personagem().hp() << "\n";
    beber(fabrica, "antidoto_escamado", *jogador);
    jogador->passarTurno();
    std::cout << "   início do turno: o veneno pergunta a resistência ao TOPO da cadeia -> HP " << jogador->personagem().hp() << "\n";
    mostrarCadeia(*jogador);

    titulo("[PASSO 4] Bálsamo de Cobre: remove o veneno do MEIO da cadeia");
    beber(fabrica, "balsamo_de_cobre", *jogador);
    mostrarCadeia(*jogador);
    std::cout << "   (os efeitos acima e abaixo do veneno continuam no lugar)\n";

    titulo("[PASSO 5] Os mesmos efeitos mudando um combate");
    std::vector<Inimigo> inimigos;
    Inimigo golem;
    golem.id = "golem_de_enxofre";
    golem.combatente = criar("Gólem de Enxofre", { { Atributo::HP_MAXIMO, 40 }, { Atributo::FORCA, 8 }, { Atributo::DEFESA, 4 },
            { Atributo::DEFESA_MAGICA, 1 }, { Atributo::VELOCIDADE, 2 }, { Atributo::PRECISAO, 80 } },
            { { Elemento::FOGO, 0.25f }, { Elemento::ELETRICO, 1.5f } });
    inimigos.push_back(std::move(golem));
    Combate combate(*jogador, std::move(inimigos), 11);
    Combatente &alvo = combate.participante(1);

    auto mostrarEventos = [](const std::vector<Evento> &eventos) {
        for (const Evento &e : eventos) {
            if (e.tipo != Evento::Tipo::VEZ) std::cout << "   " << e.texto << "\n";
        }
    };
    auto usar = [&](const std::string &id) {
        auto aplicacoes = fabrica.aplicarPocao(pocoes[id], *jogador, combate.inimigosVivos(), &alvo);
        mostrarEventos(combate.jogadorUsouPocao(nomes[id], aplicacoes));
    };

    int rodada = 0;
    while (!combate.terminou() && rodada < 20) {
        mostrarEventos(combate.avancar());
        if (!combate.aguardandoJogador()) continue;
        std::cout << ">> Vez do Alquimista\n";
        rodada++;
        if (rodada == 1) usar("elixir_do_sono");
        else if (rodada == 2) usar("pocao_flamejante");
        else if (rodada == 3) usar("faisca_de_cobre");
        else mostrarEventos(combate.jogadorAtaca(1));
        mostrarCadeia(alvo);
    }
    std::cout << "   Resultado: " << (combate.resultado() == Combate::Resultado::VITORIA ? "vitória" : "derrota") << "\n";
    std::cout << "   Efeitos de turno saem no fim do combate:\n";
    mostrarCadeia(*jogador);

    std::cout << "\n" << std::string(75, '=') << "\n";
    std::cout << " DEMONSTRAÇÃO CONCLUÍDA\n";
    std::cout << std::string(75, '=') << "\n";
    return 0;
}
