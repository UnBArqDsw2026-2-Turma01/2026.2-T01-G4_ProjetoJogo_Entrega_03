# GoF Criacional: Builder na criação de personagens

Programa de terminal em C++17 da SubEquipe 01. O jogador escolhe, atributo por atributo, o que entra no personagem (1 inclui, 0 deixa de fora), e o `CharacterBuilder` monta o `Character` por partes até o `Build()`. A modelagem está em [GoFCriacional.md](../../../docs/Base/Relatórios/SubEquipe_01/GoFCriacional.md).

## Onde está cada participante do padrão

| Participante | Arquivo |
|--------------|---------|
| Builder (interface) | [`src/builder.h`](src/builder.h) |
| ConcreteBuilder | [`src/character_builder.h`](src/character_builder.h) e [`.cpp`](src/character_builder.cpp) |
| Product | [`src/character.h`](src/character.h) e [`.cpp`](src/character.cpp) |
| Parte do produto | [`src/attack_action.h`](src/attack_action.h) e [`.cpp`](src/attack_action.cpp) |
| Client (menu interativo) | [`src/terminal_menu.h`](src/terminal_menu.h) e [`.cpp`](src/terminal_menu.cpp) |
| Ponto de entrada | [`src/main.cpp`](src/main.cpp) |
| Testes | [`tests/tests.cpp`](tests/tests.cpp) |
| Cabeçalho `bits/stdc++.h` portável | [`include/bits/stdc++.h`](include/bits/stdc++.h) |

## Como compilar e rodar

Pré-requisitos: um compilador com suporte a C++17 (`g++` ou `clang++`) e o `make`. No Windows, use o MSYS2/MinGW ou o WSL.

```bash
cd src/criacional/builder
make run
```

Todos os arquivos usam `#include <bits/stdc++.h>` e `using namespace std;`. Como esse cabeçalho só existe no GCC (o `g++` do macOS é o Apple clang, que não o tem), o projeto traz uma versão própria em [`include/bits/stdc++.h`](include/bits/stdc++.h), que reúne os cabeçalhos padrão. Por isso a compilação usa `-Iinclude`, já configurado no `Makefile`.

Sem o `make`:

```bash
cd src/criacional/builder
g++ -std=c++17 -Iinclude src/*.cpp -o builder
./builder
```

## Exemplo de uso

```text
=== Criacao de personagens com o padrao Builder ===

Atributos disponiveis: nome, tipo, pontos de vida, acoes de ataque
Incluir nome? (1 = sim, 0 = nao): 1
  Nome: Arthas
Incluir tipo? (1 = sim, 0 = nao): 0
Incluir pontos de vida? (1 = sim, 0 = nao): 1
  Pontos de vida: 120
Incluir acoes de ataque? (1 = sim, 0 = nao): 1
  Quantidade de acoes: 1
  Nome da acao 1: Golpe de espada
  Dano da acao 1: 25

--- Personagem criado ---
Nome: Arthas
Tipo: (nao incluido)
Pontos de vida: 120
Acoes de ataque:
  - Golpe de espada (25 de dano)

Arthas ataca:
  Golpe de espada causa 25 de dano
Arthas se defende com 120 pontos de vida

Criar outro personagem? (1 = sim, 0 = nao): 0
Fim.
```

## Testes

```bash
cd src/criacional/builder
make test
```

Os testes cobrem o `CharacterBuilder` (montagem completa, parcial, reinício após o `Build()` e validação dos pontos de vida), os comportamentos do `Character` e o menu, alimentado por uma entrada simulada. A saída termina com `27/27 verificacoes passaram`, e o programa retorna código 0 quando tudo passa.

Para apagar os binários: `make clean`.
