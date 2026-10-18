# GoF Estrutural: Facade no sistema de alquimia

Projeto Godot 4.7 da SubEquipe 02, todo em C++ (GDExtension com godot-cpp). A mochila do jogador combina ingredientes por meio de uma única classe, a `AlquimiaFacade`, que esconde o subsistema de alquimia. O jogador anda por um mapa, coleta ingredientes e abre baús; tudo o que mexe no inventário passa pela fachada. A modelagem está em [Facade.md](../../docs/Base/Relatórios/SubEquipe_02/Facade.md) (e a visão geral da subequipe em [GoFEstrutural.md](../../docs/Base/Relatórios/SubEquipe_02/GoFEstrutural.md)).

## Onde está cada participante do padrão

| Participante | Arquivo |
|--------------|---------|
| Client | [`native/src/jogo/mochila_ui.cpp`](native/src/jogo/mochila_ui.cpp) (também `mundo`, `bau` e `ingrediente_coletavel`) |
| Facade | [`native/src/alquimia_facade.h`](native/src/alquimia_facade.h) e [`.cpp`](native/src/alquimia_facade.cpp) |
| Subsistema | [`native/src/subsistema.h`](native/src/subsistema.h) e [`.cpp`](native/src/subsistema.cpp) |
| Domínio (complemento) | [`native/src/dominio.h`](native/src/dominio.h), [`native/src/resultado_mistura.h`](native/src/resultado_mistura.h) |
| Dados (8 ingredientes, 36 receitas) | [`data/alquimia.json`](data/alquimia.json) |
| Mundo de teste (mapa, baús, jogador) | [`native/src/jogo/`](native/src/jogo/), [`data/mapa.json`](data/mapa.json), [`data/baus.json`](data/baus.json) |

## Como compilar e rodar

Pré-requisitos: Godot 4.7+, Python 3 com `pip install scons`, e compilador C++ (GCC/Clang no Linux, MSVC no Windows).

### Linux

```bash
cd src/estrutural/native
git clone --depth 1 https://github.com/godotengine/godot-cpp.git   # se ainda não clonado
python3 -m SCons platform=linux target=template_debug -j$(nproc)
```

A biblioteca compartilhada é gerada em `bin/libalquimia.linux.template_debug.x86_64.so`.

### Windows

```bash
cd src/estrutural/native
git clone --depth 1 https://github.com/godotengine/godot-cpp.git
py -m SCons platform=windows target=template_debug
```

A DLL é gerada em `bin/`.

### Assets visuais (Jogador, Galinha, Mapa e UI)

Os assets originais do TinyPixel não ficam no repositório por restrições de licença. Para rodar o jogo imediatamente sem erros:

**Opção 1 (Rápida - Placeholders inclusos):**
Gera os sprites em pixel art e o mapa proceduralmente com Pillow:
```bash
cd src/estrutural
python3 tools/gerar_placeholders.py
```

**Opção 2 (Pack TinyPixel oficial):**
Baixe o [TinyPixel Starter Edition](https://onelevelstudio.itch.io/tinypixel), extraia e importe:
```bash
cd src/estrutural
pip install pillow
py tools/importar_tinypixel.py "<pasta TinyPixel_StarterEdition_v1.0>"
```
Depois, abra `src/estrutural/project.godot` no Godot e rode a cena principal (F5).

## Build para web

Requer o [Emscripten](https://emscripten.org/) **4.0.11** (a mesma versão dos templates web do Godot 4.7.2) ativo no terminal:

```bash
py -m SCons platform=web target=template_release threads=no
```

No Linux, o SCons gera `bin/libalquimia.web...wasm` (com `lib`), mas o `alquimia.gdextension` aponta para `bin/alquimia.web...wasm`: copie um sobre o outro antes de exportar, senão o jogo exportado leva a extensão antiga.

No Godot, exporte com o preset **Web**, marcando **Extensions Support** e desmarcando **Thread Support** (ou `godot --headless --path src/estrutural --export-release "Web" export/web/index.html`). Para publicar, copie `export/web/` para `docs/jogo/` e refaça o anti-cache: `index.js?v=<versão>` no `index.html` e `fetch(url, { cache: 'no-store' })` com `?v=<versão>` no carregador do `index.js`, trocando a versão a cada publicação.

## Decorator: efeitos das poções

Cada efeito ativo de uma poção é um decorador sobre o personagem (padrão Decorator, de Marcos Vinícius). A modelagem está em [Decorator.md](../../docs/Base/Relatórios/SubEquipe_02/Decorator.md).

| Participante | Arquivo |
|--------------|---------|
| Component | [`decorator/personagem.hpp`](decorator/personagem.hpp) |
| ConcreteComponent | [`decorator/atributos_base.hpp`](decorator/atributos_base.hpp) |
| Decorator | [`decorator/efeito_decorator.hpp`](decorator/efeito_decorator.hpp) |
| ConcreteDecorators (9) | [`decorator/efeitos.hpp`](decorator/efeitos.hpp) |
| Client | [`decorator/combatente.hpp`](decorator/combatente.hpp) (e [`combate.hpp`](decorator/combate.hpp)) |
| Fábrica e dados | [`decorator/fabrica_de_efeitos.hpp`](decorator/fabrica_de_efeitos.hpp), `efeitos` de cada poção em [`data/alquimia.json`](data/alquimia.json) |
| No jogo | `usarPocao` na fachada, [`native/src/jogo/tela_de_combate.cpp`](native/src/jogo/tela_de_combate.cpp), caverna, inimigos e runas em [`mundo.cpp`](native/src/jogo/mundo.cpp), [`data/inimigos.json`](data/inimigos.json), [`data/exploracao.json`](data/exploracao.json) |

O núcleo em `decorator/` é C++17 puro, só com headers: compila sozinho com `g++` e entra na GDExtension pelo `CPPPATH` do `SConstruct`.

```bash
make -C src/estrutural/decorator test   # 172 verificações do núcleo, sem Godot
make -C src/estrutural/decorator run    # demonstração no terminal
```

No jogo: clicar em uma poção na mochila a usa; encostar em um inimigo do mapa começa o combate (1 a 4 escolhem a ação, o número escolhe o alvo); a caverna (E na entrada da Pedreira) é escura e a Chama Fátua ou o Elixir da Noite aumentam a luz; a Poção de Clarividência revela runas com recompensas; o Amuleto Líquido soma sorte aos próximos baús.

## Controles

WASD ou setas: andar · E: interagir (baú, caverna, runa) · I ou Tab: mochila · Esc: fechar a mochila · 1 a 4: ações no combate.

## Testes

```bash
godot --headless --path src/estrutural -s res://tests/teste_facade.gd
godot --headless --path src/estrutural -s res://tests/teste_decorator.gd
godot --path src/estrutural -s res://tests/captura_decorator.gd   # prints do combate e da caverna (precisa de janela)
```

## Ícones e créditos

- **Ícones de ingredientes, poções e baús:** gerados por [`tools/gerar_icones.py`](tools/gerar_icones.py).
- **Inimigos, fundos de combate, caverna e Sopro do Dragão:** gerados com o Gemini (Google) e processados por [`tools/processar_brutos.py`](tools/processar_brutos.py).
- **Ícones de status, efeitos visuais, interface de combate, luz e runas:** gerados por [`tools/gerar_assets_efeitos.py`](tools/gerar_assets_efeitos.py).
- **Mapa:** gerado por [`tools/gerar_mapa.py`](tools/gerar_mapa.py) a partir dos autotiles do TinyPixel.
- **Interface:** TinyPixel by One Level Studio (Starter Edition, uso não comercial). A licença está em [`assets/ui/TINYPIXEL_LICENSES.txt`](assets/ui/TINYPIXEL_LICENSES.txt).
