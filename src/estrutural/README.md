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

Os assets do TinyPixel não estão no repositório, porque a licença proíbe redistribuir os arquivos do pack. Baixe o [TinyPixel Starter Edition](https://onelevelstudio.itch.io/tinypixel), extraia e importe (isso também gera o mapa):

```bash
cd src/estrutural
pip install pillow
py tools/importar_tinypixel.py "<pasta TinyPixel_StarterEdition_v1.0>"
``` Depois, abra `src/estrutural/project.godot` no Godot e rode a cena principal (F5).

## Build para web

Requer o [Emscripten](https://emscripten.org/) ativo no terminal:

```bash
py -m SCons platform=web target=template_release threads=no
```

No Godot, exporte com o preset **Web**, marcando **Extensions Support** e desmarcando **Thread Support**.

## Controles

WASD ou setas: andar · E: abrir baú · I ou Tab: mochila · Esc: fechar a mochila.

## Testes

```bash
godot --headless --path src/estrutural -s res://tests/teste_facade.gd
```

## Ícones e créditos

- **Ícones de ingredientes, poções e baús:** gerados por [`tools/gerar_icones.py`](tools/gerar_icones.py).
- **Mapa:** gerado por [`tools/gerar_mapa.py`](tools/gerar_mapa.py) a partir dos autotiles do TinyPixel.
- **Interface:** TinyPixel by One Level Studio (Starter Edition, uso não comercial). A licença está em [`assets/ui/TINYPIXEL_LICENSES.txt`](assets/ui/TINYPIXEL_LICENSES.txt).
