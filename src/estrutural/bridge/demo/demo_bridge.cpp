// Compilar (dentro de src/estrutural/bridge/demo):
//   g++ -std=c++17 -Wall -Wextra -I .. demo_bridge.cpp -o demo_bridge
// Executar:
//   ./demo_bridge          (Windows: .\demo_bridge.exe)

#include "controlador_jogador.hpp"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <deque>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace bridge;
using Relogio = std::chrono::steady_clock;

namespace {

volatile std::sig_atomic_t sair = 0;
void aoInterromper(int) { sair = 1; }

// ---------------------------------------------------------------- terminal

enum class Tecla { CIMA, BAIXO, ESQUERDA, DIREITA, FONTE_TECLADO, FONTE_IA, PENA_LEVE, COMBATE, SAIR, OUTRA };

struct Pressionada {
    Tecla tecla = Tecla::OUTRA;
    std::string nome;
};

// Coloca o terminal em modo "cru" (sem Enter, sem eco) e restaura ao sair.
class Terminal {
public:
    Terminal() {
#ifdef _WIN32
        HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD modo = 0;
        if (GetConsoleMode(saida, &modo)) SetConsoleMode(saida, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleOutputCP(CP_UTF8);
#else
        if (tcgetattr(STDIN_FILENO, &original) == 0) {
            termios cru = original;
            cru.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
            cru.c_cc[VMIN] = 0;  // read() não bloqueia
            cru.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &cru);
            alterado = true;
        }
#endif
        std::fputs("\033[2J\033[?25l", stdout); // limpa a tela e esconde o cursor
    }

    ~Terminal() {
#ifndef _WIN32
        if (alterado) tcsetattr(STDIN_FILENO, TCSANOW, &original);
#endif
        std::fputs("\033[?25h\n", stdout);
    }

    Terminal(const Terminal &) = delete;
    Terminal &operator=(const Terminal &) = delete;

    // Devolve uma tecla se houver alguma esperando; nunca bloqueia.
    bool lerTecla(Pressionada &p) {
        const int b = lerByte();
        if (b < 0) return false;
#ifdef _WIN32
        if (b == 0 || b == 224) { // teclas especiais: o código vem no byte seguinte
            switch (_getch()) {
                case 72: p = { Tecla::CIMA, "seta para cima" }; return true;
                case 80: p = { Tecla::BAIXO, "seta para baixo" }; return true;
                case 75: p = { Tecla::ESQUERDA, "seta para a esquerda" }; return true;
                case 77: p = { Tecla::DIREITA, "seta para a direita" }; return true;
                default: p = { Tecla::OUTRA, "tecla especial" }; return true;
            }
        }
#else
        if (b == 27) { // setas chegam como ESC [ A/B/C/D
            const int b2 = lerByte();
            const int b3 = lerByte();
            if (b2 == '[') {
                switch (b3) {
                    case 'A': p = { Tecla::CIMA, "seta para cima" }; return true;
                    case 'B': p = { Tecla::BAIXO, "seta para baixo" }; return true;
                    case 'C': p = { Tecla::DIREITA, "seta para a direita" }; return true;
                    case 'D': p = { Tecla::ESQUERDA, "seta para a esquerda" }; return true;
                    default: break;
                }
            }
            p = { Tecla::OUTRA, "ESC" };
            return true;
        }
#endif
        switch (b) {
            case 'w': case 'W': p = { Tecla::CIMA, "W" }; return true;
            case 's': case 'S': p = { Tecla::BAIXO, "S" }; return true;
            case 'a': case 'A': p = { Tecla::ESQUERDA, "A" }; return true;
            case 'd': case 'D': p = { Tecla::DIREITA, "D" }; return true;
            case '1': p = { Tecla::FONTE_TECLADO, "1" }; return true;
            case '2': p = { Tecla::FONTE_IA, "2" }; return true;
            case 'p': case 'P': p = { Tecla::PENA_LEVE, "P" }; return true;
            case 'c': case 'C': p = { Tecla::COMBATE, "C" }; return true;
            case 'q': case 'Q': case 3: p = { Tecla::SAIR, "Q" }; return true; // 3 = Ctrl+C
            default:
                p.tecla = Tecla::OUTRA;
                p.nome = (b >= 32 && b < 127) ? std::string(1, static_cast<char>(b)) : "código " + std::to_string(b);
                return true;
        }
    }

private:
    static int lerByte() {
#ifdef _WIN32
        return _kbhit() ? _getch() : -1;
#else
        unsigned char c = 0;
        return read(STDIN_FILENO, &c, 1) == 1 ? c : -1;
#endif
    }

#ifndef _WIN32
    termios original{};
    bool alterado = false;
#endif
};

// ----------------------------------------------- ConcreteImplementors (Bridge)

// O terminal não avisa quando a tecla é solta. Por isso cada tecla vale por um
// pulso curto de direção; segurar a tecla (repetição automática) mantém o pulso.
class EntradaTecladoTerminal : public FonteDeEntrada {
public:
    static constexpr double PULSO = 0.12; // segundos

    void pressionar(Tecla t) {
        switch (t) {
            case Tecla::CIMA: direcao = { 0.0, -1.0 }; break;
            case Tecla::BAIXO: direcao = { 0.0, 1.0 }; break;
            case Tecla::ESQUERDA: direcao = { -1.0, 0.0 }; break;
            case Tecla::DIREITA: direcao = { 1.0, 0.0 }; break;
            default: return;
        }
        restante = PULSO;
    }

    void atualizar(double dt) {
        if (restante > 0.0) {
            restante -= dt;
            if (restante <= 0.0) direcao = { 0.0, 0.0 };
        }
    }

    Direcao lerDirecao() const override { return direcao; }

private:
    Direcao direcao;
    double restante = 0.0;
};

// "IA" simples: percorre uma rota de direções, trocando a cada `intervalo` segundos.
class EntradaPatrulha : public FonteDeEntrada {
public:
    EntradaPatrulha(std::vector<Direcao> p_rota, double p_intervalo) : rota(std::move(p_rota)), intervalo(p_intervalo) {}

    void atualizar(double dt) {
        tempo += dt;
        if (tempo >= intervalo) {
            tempo -= intervalo;
            indice = (indice + 1) % rota.size();
        }
    }

    Direcao lerDirecao() const override { return rota[indice]; }

private:
    std::vector<Direcao> rota;
    double intervalo;
    double tempo = 0.0;
    size_t indice = 0;
};

// ------------------------------------------------------------------- mundo

constexpr int COLUNAS = 48;
constexpr int LINHAS = 12;
constexpr double PX_POR_COLUNA = 8.0;  // célula de terminal: ~2x mais alta que larga
constexpr double PX_POR_LINHA = 16.0;
constexpr double LARGURA = COLUNAS * PX_POR_COLUNA;
constexpr double ALTURA = LINHAS * PX_POR_LINHA;

struct Ator {
    double x = 0.0;
    double y = 0.0;
    void mover(Direcao d) {
        x = std::clamp(x + d.x, 0.0, LARGURA - 0.01);
        y = std::clamp(y + d.y, 0.0, ALTURA - 0.01);
    }
    int coluna() const { return static_cast<int>(x / PX_POR_COLUNA); }
    int linha() const { return static_cast<int>(y / PX_POR_LINHA); }
};

void acrescentar(std::string &quadro, const std::string &texto) {
    quadro += texto;
    quadro += "\033[K\n"; // apaga o resto da linha (evita resíduos do quadro anterior)
}

std::string formatar(const char *formato, double a, double b) {
    char buffer[96];
    std::snprintf(buffer, sizeof(buffer), formato, a, b);
    return buffer;
}

} // namespace

int main() {
    std::signal(SIGINT, aoInterromper);
    Terminal terminal;

    // Implementações concretas (o "lado de baixo" da ponte).
    auto teclado = std::make_shared<EntradaTecladoTerminal>();
    auto patrulhaJogador = std::make_shared<EntradaPatrulha>(
            std::vector<Direcao>{ { 1.0, 0.0 }, { 0.0, 1.0 }, { -1.0, 0.0 }, { 0.0, -1.0 } }, 0.8);
    auto patrulhaGalinha = std::make_shared<EntradaPatrulha>(std::vector<Direcao>{ { 1.0, 0.0 }, { -1.0, 0.0 } }, 1.5);

    // Abstração e abstração refinada (o "lado de cima" da ponte).
    auto controladorJogador = std::make_shared<ControladorJogador>(teclado, ControladorJogador::BASE);
    ControladorMovimento controladorGalinha(patrulhaGalinha, 50.0); // mesma abstração, outro personagem

    std::shared_ptr<FonteDeEntrada> fonteAtual = teclado;
    std::string nomeFonte = "EntradaTecladoTerminal";
    bool usandoTeclado = true;
    bool penaLeve = false;
    bool combate = false;

    Ator jogador{ LARGURA / 2.0, ALTURA / 2.0 };
    Ator galinha{ 40.0, 24.0 };
    std::string ultimaTecla = "(nenhuma)";
    std::deque<std::string> historico;
    Direcao ultimoDeslocamento;
    // Só para a tela: mantém o último valor diferente de zero por 0,5 s, senão um toque
    // de tecla (0,12 s) apareceria e sumiria rápido demais para ser lido no vídeo.
    Direcao direcaoExibida, deslocamentoExibido;
    double tempoExibido = 0.0;

    auto registrar = [&historico](const std::string &texto) {
        historico.push_back(texto);
        if (historico.size() > 5) historico.pop_front();
    };
    registrar("Demonstração iniciada com a fonte EntradaTecladoTerminal.");

    auto anterior = Relogio::now();
    while (!sair) {
        const auto agora = Relogio::now();
        double dt = std::chrono::duration<double>(agora - anterior).count();
        anterior = agora;
        if (dt > 0.1) dt = 0.1;

        // 1. Ler o terminal e reagir às teclas.
        Pressionada p;
        while (terminal.lerTecla(p)) {
            ultimaTecla = p.nome;
            switch (p.tecla) {
                case Tecla::CIMA: case Tecla::BAIXO: case Tecla::ESQUERDA: case Tecla::DIREITA:
                    if (usandoTeclado) {
                        teclado->pressionar(p.tecla);
                        registrar("Tecla " + p.nome + " -> EntradaTecladoTerminal informa a direção.");
                    } else {
                        registrar("Tecla " + p.nome + " lida, mas a fonte atual é a IA: ignorada.");
                    }
                    break;
                case Tecla::FONTE_TECLADO:
                    if (!usandoTeclado) {
                        controladorJogador->trocarFonte(teclado);
                        fonteAtual = teclado;
                        nomeFonte = "EntradaTecladoTerminal";
                        usandoTeclado = true;
                        registrar("trocarFonte(EntradaTecladoTerminal): o Jogador não foi alterado.");
                    }
                    break;
                case Tecla::FONTE_IA:
                    if (usandoTeclado) {
                        controladorJogador->trocarFonte(patrulhaJogador);
                        fonteAtual = patrulhaJogador;
                        nomeFonte = "EntradaPatrulha (IA)";
                        usandoTeclado = false;
                        registrar("trocarFonte(EntradaPatrulha): o Jogador não foi alterado.");
                    }
                    break;
                case Tecla::PENA_LEVE:
                    penaLeve = !penaLeve;
                    controladorJogador->aplicarAtributoVelocidade(penaLeve ? 2 : 0);
                    registrar(penaLeve ? "Pena Leve (+2 velocidade): a velocidade do controlador mudou."
                                       : "Pena Leve removida: velocidade volta ao valor base.");
                    break;
                case Tecla::COMBATE:
                    combate = !combate;
                    controladorJogador->ativar(!combate);
                    registrar(combate ? "Combate iniciado: ativar(false), o movimento é suspenso."
                                      : "Combate terminou: ativar(true), o movimento volta.");
                    break;
                case Tecla::SAIR:
                    sair = 1;
                    break;
                case Tecla::OUTRA:
                    registrar("Tecla " + p.nome + " sem função nesta demonstração.");
                    break;
            }
        }

        // 2. Atualizar as fontes e mover os personagens pelos controladores.
        teclado->atualizar(dt);
        patrulhaJogador->atualizar(dt);
        patrulhaGalinha->atualizar(dt);
        ultimoDeslocamento = controladorJogador->deslocamento(dt);
        jogador.mover(ultimoDeslocamento);
        galinha.mover(controladorGalinha.deslocamento(dt));

        // 3. Desenhar o quadro.
        const Direcao lida = fonteAtual->lerDirecao();
        if (lida.x != 0.0 || lida.y != 0.0) {
            direcaoExibida = lida;
            deslocamentoExibido = ultimoDeslocamento;
            tempoExibido = 0.5;
        } else if (tempoExibido > 0.0) {
            tempoExibido -= dt;
            if (tempoExibido <= 0.0) direcaoExibida = deslocamentoExibido = Direcao{};
        }
        const double velocidade = ControladorJogador::BASE + (penaLeve ? 2 : 0) * ControladorJogador::GANHO;

        std::string quadro = "\033[H";
        acrescentar(quadro, " BRIDGE no G4_ProjetoJogo: demonstração em terminal");
        acrescentar(quadro, "");
        acrescentar(quadro, " +" + std::string(COLUNAS, '-') + "+");
        for (int l = 0; l < LINHAS; l++) {
            std::string linha(COLUNAS, ' ');
            if (galinha.linha() == l) linha[static_cast<size_t>(galinha.coluna())] = 'G';
            if (jogador.linha() == l) linha[static_cast<size_t>(jogador.coluna())] = '@';
            acrescentar(quadro, " |" + linha + "|");
        }
        acrescentar(quadro, " +" + std::string(COLUNAS, '-') + "+");
        acrescentar(quadro, "  @ = jogador (ControladorJogador)    G = galinha (ControladorMovimento + IA)");
        acrescentar(quadro, "");
        acrescentar(quadro, "  Ponte:  ControladorJogador  -->  " + nomeFonte);
        acrescentar(quadro, "  Última tecla pressionada: " + ultimaTecla);
        acrescentar(quadro, "  lerDirecao()      = " + formatar("(%+.2f, %+.2f)", direcaoExibida.x, direcaoExibida.y));
        acrescentar(quadro, "  deslocamento(dt)  = " + formatar("(%+.2f, %+.2f) px por quadro", deslocamentoExibido.x, deslocamentoExibido.y));
        acrescentar(quadro, "  Velocidade: " + std::to_string(static_cast<int>(velocidade)) + " px/s | Pena Leve: " +
                (penaLeve ? "ON " : "OFF") + " | Movimento: " + (combate ? "SUSPENSO (combate)" : "ativo"));
        acrescentar(quadro, "");
        acrescentar(quadro, "  Eventos:");
        for (const std::string &h : historico) acrescentar(quadro, "   - " + h);
        for (size_t i = historico.size(); i < 5; i++) acrescentar(quadro, "");
        acrescentar(quadro, "");
        acrescentar(quadro, "  [W A S D / setas] mover   [1] fonte teclado   [2] fonte IA");
        acrescentar(quadro, "  [P] Pena Leve   [C] combate (liga/desliga)   [Q] sair");

        std::fputs(quadro.c_str(), stdout);
        std::fflush(stdout);

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    return 0;
}