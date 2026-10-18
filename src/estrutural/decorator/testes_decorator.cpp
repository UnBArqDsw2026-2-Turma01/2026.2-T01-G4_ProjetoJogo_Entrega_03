#include "combate.hpp"
#include "json_minimo.hpp"

#include <iostream>
#include <set>

using namespace G4Efeitos;

static int falhas = 0;
static int verificacoes = 0;

#define CHECAR(cond, msg)                                              \
    do {                                                               \
        verificacoes++;                                                \
        if (!(cond)) {                                                 \
            falhas++;                                                  \
            std::cout << "  FALHOU: " << msg << "  (" << #cond << ")\n"; \
        }                                                              \
    } while (0)

static std::unique_ptr<Combatente> novoCombatente(const std::string &nome, int hp = 50, int forca = 7, int defesa = 2,
        int velocidade = 4, std::map<Elemento, float> resistencias = {}) {
    std::map<Atributo, int> base = {
        { Atributo::HP_MAXIMO, hp }, { Atributo::MP_MAXIMO, 30 }, { Atributo::FORCA, forca },
        { Atributo::DEFESA, defesa }, { Atributo::DEFESA_MAGICA, 2 }, { Atributo::VELOCIDADE, velocidade },
        { Atributo::PRECISAO, 100 },
    };
    return std::make_unique<Combatente>(std::make_unique<AtributosBase>(nome, base, resistencias, 40.0f));
}

static void testarModificadores() {
    std::cout << "Modificadores de atributo e duração\n";
    auto c = novoCombatente("Herói");
    c->decorar<ModificadorDeAtributo>(Duracao::turnos(3), "forca", Atributo::FORCA, 2);
    c->decorar<ModificadorDeAtributo>(Duracao::turnos(3), "velocidade", Atributo::VELOCIDADE, 2);
    CHECAR(c->personagem().atributo(Atributo::FORCA) == 9, "Quimera soma força");
    CHECAR(c->personagem().atributo(Atributo::VELOCIDADE) == 6, "Quimera soma velocidade");
    CHECAR(c->personagem().atributo(Atributo::DEFESA) == 2, "atributo não decorado fica igual");
    CHECAR(c->personagem().efeitosAtivos().size() == 2, "dois efeitos ativos");
    c->passarTurno();
    c->passarTurno();
    CHECAR(c->personagem().efeitosAtivos().size() == 2, "ainda ativos após 2 turnos");
    c->passarTurno();
    CHECAR(c->personagem().efeitosAtivos().empty(), "expiram após 3 turnos");
    CHECAR(c->personagem().atributo(Atributo::FORCA) == 7, "força volta ao valor base");
}

static void testarVenenoEAntidoto() {
    std::cout << "Veneno, Antídoto (por cima) e Bálsamo (no meio da cadeia)\n";
    auto c = novoCombatente("Herói");
    c->decorar<EfeitoPorTurno>(Duracao::turnos(3), "veneno", -4, Elemento::VENENO);
    c->passarTurno();
    CHECAR(c->personagem().hp() == 46, "veneno tira 4 HP no início do turno");
    c->decorar<Resistencia>(Duracao::turnos(3), "imune_veneno", Elemento::VENENO, 0.0f);
    c->passarTurno();
    CHECAR(c->personagem().hp() == 46, "Antídoto bebido depois do veneno zera o dano (consulta o topo)");
    c->decorar<ModificadorDeAtributo>(Duracao::turnos(3), "forca", Atributo::FORCA, 3);
    const int removidos = c->removerEfeitos("veneno");
    CHECAR(removidos == 1, "Bálsamo remove um efeito");
    const auto efeitos = c->personagem().efeitosAtivos();
    CHECAR(efeitos.size() == 2 && efeitos[0] == "imune_veneno" && efeitos[1] == "forca", "efeitos acima e abaixo continuam, na mesma ordem");
    CHECAR(c->personagem().atributo(Atributo::FORCA) == 10, "o efeito de cima continua funcionando");

    auto r = novoCombatente("Regenerado");
    r->personagem().alterarHp(-20);
    r->decorar<EfeitoPorTurno>(Duracao::turnos(3), "regeneracao", 5, Elemento::FISICO);
    r->passarTurno();
    CHECAR(r->personagem().hp() == 35, "regeneração cura 5 HP por turno");
}

static void testarControle() {
    std::cout << "Sono, Confusão, Invisibilidade, reações únicas, arma elemental e luz\n";
    auto c = novoCombatente("Gólem", 40, 8, 0);
    c->decorar<Sono>(Duracao::turnos(3), "sono");
    CHECAR(!c->personagem().podeAgir(), "dormindo não age");
    c->receberAtaque(Ataque{ 5, Elemento::FISICO, false });
    CHECAR(c->personagem().podeAgir() && c->personagem().efeitosAtivos().empty(), "acorda ao receber dano");

    c->decorar<Confusao>(Duracao::turnos(2), "confusao", 42u);
    std::set<int> escolhidos;
    for (int i = 0; i < 50; i++) escolhidos.insert(c->personagem().escolherAlvo(3, 0));
    CHECAR(escolhidos.size() == 3, "confuso sorteia entre todos os alvos");
    c->removerEfeitos("confusao");
    CHECAR(c->personagem().escolherAlvo(3, 1) == 1, "sem confusão escolhe o pretendido");

    c->decorar<Invisibilidade>(Duracao::turnos(2), "invisivel");
    CHECAR(!c->personagem().podeSerAlvo(), "invisível não pode ser alvo");

    auto h = novoCombatente("Herói", 50, 7, 0);
    h->decorar<ReacaoUnica>(Duracao::usoUnico(), "esquiva", Reacao::ESQUIVOU);
    CHECAR(h->receberAtaque(Ataque{ 10, Elemento::FISICO, false }).reacao == Reacao::ESQUIVOU, "Voo esquiva o primeiro ataque");
    CHECAR(h->receberAtaque(Ataque{ 10, Elemento::FISICO, false }).dano == 10, "e só o primeiro");
    CHECAR(h->personagem().efeitosAtivos().empty(), "reação usada sai da cadeia");

    h->decorar<ReacaoUnica>(Duracao::usoUnico(), "reflexo", Reacao::REFLETIU);
    CHECAR(h->receberAtaque(Ataque{ 4, Elemento::FISICO, false }).reacao == Reacao::NENHUMA, "Prisma não reflete ataque físico");
    CHECAR(h->receberAtaque(Ataque{ 4, Elemento::FOGO, true }).reacao == Reacao::REFLETIU, "Prisma reflete magia");

    h->decorar<ArmaElemental>(Duracao::turnos(3), "arma_fogo", Elemento::FOGO);
    CHECAR(h->atacar().elemento == Elemento::FOGO, "Óleo de Salamandra muda o elemento do ataque");

    CHECAR(h->personagem().raioDeLuz() == 40.0f, "raio de luz base");
    h->decorar<RaioDeLuz>(Duracao::permanente(), "luz", 96.0f);
    h->decorar<RaioDeLuz>(Duracao::permanente(), "visao_noturna", 160.0f);
    CHECAR(h->personagem().raioDeLuz() == 160.0f, "vale o maior raio de luz");
    h->passarTurno();
    CHECAR(h->personagem().raioDeLuz() == 160.0f, "luz é permanente (não conta turnos)");
}

static void testarSorteEBase() {
    std::cout << "Sorte contada em saques e mudanças permanentes\n";
    auto c = novoCombatente("Herói");
    c->decorar<ModificadorDeAtributo>(Duracao::saques(3), "sorte", Atributo::SORTE, 25);
    c->passarTurno();
    CHECAR(c->personagem().atributo(Atributo::SORTE) == 25, "turnos não gastam a sorte");
    c->saquear();
    c->saquear();
    CHECAR(c->personagem().atributo(Atributo::SORTE) == 25, "ainda vale após 2 saques");
    c->saquear();
    CHECAR(c->personagem().atributo(Atributo::SORTE) == 0, "acaba no 3o saque");

    c->aumentarBase(Atributo::HP_MAXIMO, 10);
    CHECAR(c->hpMaximo() == 60 && c->personagem().hp() == 60, "Poção de Ferro sobe o máximo e o atual");
}

static std::map<std::string, std::vector<EspecificacaoEfeito>> lerPocoes(const std::string &caminho, std::map<std::string, std::string> &nomes) {
    std::map<std::string, std::vector<EspecificacaoEfeito>> pocoes;
    Json::Valor dados;
    CHECAR(Json::lerArquivo(caminho, dados), "lê " + caminho);
    for (const Json::Valor &receita : dados["receitas"].lista) {
        const Json::Valor &pocao = receita["pocao"];
        nomes[pocao["id"].texto] = pocao["nome"].texto;
        for (const Json::Valor &efeito : pocao["efeitos"].lista) {
            std::string erro;
            pocoes[pocao["id"].texto].push_back(Json::especificacao(efeito, erro));
            CHECAR(erro.empty(), pocao["id"].texto + ": " + erro);
        }
    }
    return pocoes;
}

static void testarAs36Pocoes(const std::string &caminho) {
    std::cout << "As 36 poções de data/alquimia.json\n";
    std::map<std::string, std::string> nomes;
    const auto pocoes = lerPocoes(caminho, nomes);
    CHECAR(pocoes.size() == 36, "36 poções com efeitos");
    int comDecorador = 0;
    for (const auto &[id, efeitos] : pocoes) {
        bool decora = false;
        for (const auto &e : efeitos) decora = decora || e.ehDecorador();
        comDecorador += decora;
    }
    CHECAR(comDecorador == 25, "25 poções são decoradores e 11 são instantâneas (" + std::to_string(comDecorador) + ")");

    FabricaDeEfeitos fabrica(7);
    for (const auto &[id, efeitos] : pocoes) {
        auto jogador = novoCombatente("Herói");
        jogador->personagem().alterarHp(-25);
        jogador->personagem().alterarMp(-25);
        jogador->decorar<EfeitoPorTurno>(Duracao::turnos(3), "veneno", -4, Elemento::VENENO);
        auto a = novoCombatente("Gólem", 40, 8, 0);
        auto b = novoCombatente("Cogumelo", 24, 5, 0);
        a->decorar<ModificadorDeAtributo>(Duracao::turnos(3), "forca", Atributo::FORCA, 1);
        const auto resultados = fabrica.aplicarPocao(efeitos, *jogador, { a.get(), b.get() }, a.get());
        bool algum = false;
        for (const auto &[alvo, r] : resultados) algum = algum || r.aplicado;
        CHECAR(algum, id + " tem efeito");
        CHECAR(!resultados.empty() && !resultados.front().second.mensagem.empty(), id + " descreve o resultado");
    }

    // Alguns resultados conferidos por valor.
    auto jogador = novoCombatente("Herói");
    auto golem = novoCombatente("Gólem", 40, 8, 0, 2, { { Elemento::FOGO, 0.25f } });
    fabrica.aplicarPocao(pocoes.at("pocao_flamejante"), *jogador, { golem.get() }, golem.get());
    CHECAR(golem->personagem().hp() == 40 - 3, "Gólem resiste a fogo: (15 - defesa mágica 2) x 0,25 = 3");
    fabrica.aplicarPocao(pocoes.at("solucao_condutora"), *jogador, {}, nullptr);
    fabrica.aplicarPocao(pocoes.at("faisca_de_cobre"), *jogador, { golem.get() }, golem.get());
    CHECAR(golem->personagem().hp() == 37 - 18, "Solução Condutora soma 5 ao dano elétrico (15 + 5 - defesa mágica 2)");
    auto cogumelo = novoCombatente("Cogumelo", 24, 5, 0, 3, { { Elemento::VENENO, 0.0f } });
    const auto r = fabrica.aplicarPocao(pocoes.at("veneno_roxo"), *jogador, { cogumelo.get() }, cogumelo.get());
    CHECAR(!r.front().second.aplicado && cogumelo->personagem().efeitosAtivos().empty(), "Cogumelo é imune a veneno");
    fabrica.aplicarPocao(pocoes.at("elixir_da_quimera"), *jogador, {}, nullptr);
    CHECAR(jogador->personagem().efeitosAtivos().size() == 3, "Quimera coloca dois decoradores (mais a Solução)");
    fabrica.aplicarPocao(pocoes.at("pocao_do_esquecimento"), *jogador, { golem.get() }, golem.get());
    CHECAR(jogador->personagem().efeitosAtivos().size() == 3, "Esquecimento só afeta o inimigo");
}

static Inimigo inimigoDeTeste(const std::string &nome, int hp, int forca, int velocidade) {
    Inimigo i;
    i.id = nome;
    i.combatente = novoCombatente(nome, hp, forca, 0, velocidade);
    return i;
}

static void testarCombate() {
    std::cout << "Combate por turnos\n";
    for (unsigned semente = 1; semente <= 20; semente++) {
        auto jogador = novoCombatente("Herói", 50, 9, 2, 4);
        std::vector<Inimigo> inimigos;
        inimigos.push_back(inimigoDeTeste("Cogumelo", 24, 5, 3));
        inimigos.push_back(inimigoDeTeste("Morcego", 18, 5, 7));
        Combate combate(*jogador, std::move(inimigos), semente);
        int passos = 0;
        while (!combate.terminou() && passos++ < 200) {
            combate.avancar();
            if (combate.aguardandoJogador()) {
                combate.jogadorAtaca(combate.alvosDoJogador().front());
            }
        }
        CHECAR(combate.terminou(), "o combate termina (semente " + std::to_string(semente) + ")");
    }

    auto jogador = novoCombatente("Herói", 50, 30, 2, 4);
    jogador->decorar<ModificadorDeAtributo>(Duracao::turnos(9), "velocidade", Atributo::VELOCIDADE, 10);
    jogador->decorar<Invisibilidade>(Duracao::turnos(9), "invisivel");
    jogador->decorar<ModificadorDeAtributo>(Duracao::saques(3), "sorte", Atributo::SORTE, 25);
    std::vector<Inimigo> inimigos;
    inimigos.push_back(inimigoDeTeste("Morcego", 18, 5, 7));
    Combate combate(*jogador, std::move(inimigos), 3);
    combate.avancar();
    CHECAR(combate.aguardandoJogador(), "com velocidade 14, o jogador começa antes do Morcego (7)");
    combate.jogadorAtaca(1);
    CHECAR(combate.terminou() && combate.resultado() == Combate::Resultado::VITORIA, "força 30 derrota o Morcego");
    const auto restantes = jogador->personagem().efeitosAtivos();
    CHECAR(restantes.size() == 1 && restantes[0] == "sorte", "efeitos de turno saem no fim; a sorte (saques) fica");

    auto escondido = novoCombatente("Herói", 50, 1, 2, 1);
    escondido->decorar<Invisibilidade>(Duracao::turnos(9), "invisivel");
    std::vector<Inimigo> grupo;
    grupo.push_back(inimigoDeTeste("Gólem", 40, 8, 5));
    Combate c2(*escondido, std::move(grupo), 4);
    bool semAlvo = false;
    for (const Evento &e : c2.avancar()) semAlvo = semAlvo || e.tipo == Evento::Tipo::SEM_ALVO;
    CHECAR(semAlvo && escondido->personagem().hp() == 50, "inimigo não encontra o jogador invisível");
}

int main(int argc, char **argv) {
    const std::string caminho = argc > 1 ? argv[1] : "../data/alquimia.json";
    testarModificadores();
    testarVenenoEAntidoto();
    testarControle();
    testarSorteEBase();
    testarAs36Pocoes(caminho);
    testarCombate();
    std::cout << "\n" << verificacoes << " verificações, " << falhas << " falha(s)\n";
    std::cout << (falhas ? "RESULTADO: FALHOU" : "RESULTADO: OK") << "\n";
    return falhas ? 1 : 0;
}
