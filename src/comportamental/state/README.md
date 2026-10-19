# GoF Comportamental: State no combate por turnos

Programa de terminal em C++17 da SubEquipe 03. O `Combate` guarda o estado atual da luta (vez do jogador, vez do inimigo, vitória ou derrota) e repassa a ele cada pedido do turno. Cada estado decide o que o pedido faz e qual é o próximo estado, então o combate não precisa testar de quem é a vez. A modelagem está em [GoFComportamental.md](../../../docs/Base/Relatórios/SubEquipe_03/GoFComportamental.md).

## Onde está cada participante do padrão

| Participante | Arquivo |
|--------------|---------|
| Context | [`src/combate.h`](src/combate.h) e [`.cpp`](src/combate.cpp) |
| State | [`src/estado_combate.h`](src/estado_combate.h) |
| ConcreteState | [`src/turno_jogador.h`](src/turno_jogador.h), [`src/turno_inimigo.h`](src/turno_inimigo.h), [`src/vitoria.h`](src/vitoria.h) e [`src/derrota.h`](src/derrota.h), com os `.cpp` correspondentes |
| Client (menu interativo) | [`src/menu_terminal.h`](src/menu_terminal.h) e [`.cpp`](src/menu_terminal.cpp) |
| Domínio (complemento) | [`src/dominio.h`](src/dominio.h) e [`.cpp`](src/dominio.cpp) |
| Ponto de entrada e demonstração | [`src/main.cpp`](src/main.cpp) |
| Testes | [`tests/testes.cpp`](tests/testes.cpp) |

Os estados não guardam dados (tudo fica no `Combate`), por isso cada estado concreto tem uma única instância, obtida por `instancia()`.

## Como compilar e rodar

Pré-requisitos: um compilador com suporte a C++17 (`g++` ou `clang++`) e o `make`. No Windows, use o WSL ou o MSYS2/MinGW.

```bash
cd src/comportamental/state
make run    # partida interativa no terminal
make demo   # demonstração com roteiro fixo, que passa por todos os estados
make test   # testes automáticos
make clean  # apaga a pasta bin/
```

Sem o `make`:

```bash
cd src/comportamental/state
g++ -std=c++17 src/*.cpp -o combate
./combate --demo
```

## Regras do combate

| Item | Regra |
|------|-------|
| Aventureiro | 40 de vida e defesa 2, com H, C, O e S no inventário |
| Gólem de Enxofre | 50 de vida e ataque 9, fraco contra GELO |
| Combinações válidas | H2O (GELO, dano 12), CO2 (GELO, dano 9), CH4 (FOGO, dano 15) e H2S (VENENO, dano 10) |
| Fraqueza | o dano do efeito dobra |
| Combinação inválida | explode e causa 8 de dano no próprio jogador |
| Diário | guarda as combinações válidas descobertas, que podem ser usadas de novo sem misturar |
| Ataque do inimigo | ataque do inimigo menos a defesa do jogador, com mínimo de 1 |

A fórmula sai dos elementos escolhidos, em qualquer ordem: no menu, `1 1 3` (H, H, O) forma H2O.

## Exemplo de saída

Trecho de `make demo`. As linhas `[estado]` mostram cada troca de estado, e os avisos mostram o mesmo pedido sendo tratado de forma diferente em cada estado:

```text
> 1. O cliente pede o turno do inimigo antes da hora  [turno 1 | estado atual: TurnoJogador]
Aviso (TurnoJogador): ainda é a vez do jogador; o inimigo espera.

> 2. O jogador mistura H + H + O  [turno 1 | estado atual: TurnoJogador]
Aventureiro usa a combinação H2O.
Nova combinação registrada no Diário do Aventureiro: H2O.
Efeito GELO: Gólem de Enxofre sofre 24 de dano (fraqueza!). Vida: 26/50.
[estado] TurnoJogador -> TurnoInimigo

> 3. O jogador tenta agir de novo na vez do inimigo  [turno 1 | estado atual: TurnoInimigo]
Aviso (TurnoInimigo): é a vez do inimigo; aguarde.

> 4. Turno do inimigo  [turno 1 | estado atual: TurnoInimigo]
Gólem de Enxofre ataca! Aventureiro sofre 7 de dano. Vida: 33/40.
[estado] TurnoInimigo -> TurnoJogador
```

## Simplificações em relação ao modelo

- O combate tem um único inimigo, como no diagrama de atividades da Entrega 02. O diagrama de classes admite vários (`enfrenta 1..*`).
- Mana, livros, itens e fuga ficaram de fora: o padrão trata a alternância de turnos e o fim do combate.
- As combinações e os valores de dano são exemplos para a demonstração.
