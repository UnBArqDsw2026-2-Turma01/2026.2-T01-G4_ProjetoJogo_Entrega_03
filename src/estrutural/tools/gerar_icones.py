"""Gera os ícones 16x16 em pixel art dos ingredientes e das poções.

Uso (de dentro de src/estrutural/):  py tools/gerar_icones.py
Lê data/alquimia.json e escreve em assets/icones/.
"""
import json
from pathlib import Path

from PIL import Image

RAIZ = Path(__file__).resolve().parent.parent
SAIDA = RAIZ / "assets" / "icones"

# '.' = transparente; cada outra letra é uma cor da paleta do ícone.
INGREDIENTES = {
    "erva_lunar": ({"k": "#1d3b2a", "g": "#3f8f4f", "l": "#7fd18a", "m": "#e8f4ff"}, [
        "................",
        "..........mm....",
        ".........mmmm...",
        "....kk....mm....",
        "...kllk.........",
        "..klggk..kk.....",
        "..kgggk.kllk....",
        "...kgggkkggk....",
        "....kggkgggk....",
        ".....kgggkk.....",
        "......kggk......",
        "......kgk.......",
        "......kgk.......",
        ".....kgk........",
        ".....kk.........",
        "................",
    ]),
    "salvia_de_fogo": ({"k": "#3a1a10", "r": "#c4341b", "o": "#f08c3c", "y": "#ffd23f", "g": "#4f7a3a"}, [
        "................",
        ".......kk.......",
        "......kyyk......",
        ".....kyooyk.....",
        "....kyorroyk....",
        "....korrrrok....",
        "...korrkkrrok...",
        "...krrk..krrk...",
        "....kk.gg.kk....",
        ".......gg.......",
        "......kgg.......",
        "......kgk.......",
        ".....kgk........",
        ".....kgk........",
        ".....kk.........",
        "................",
    ]),
    "cogumelo_roxo": ({"k": "#24122e", "p": "#7a2fa8", "l": "#b46fe0", "w": "#f2e8f7", "s": "#d9cbb5"}, [
        "................",
        "................",
        ".....kkkkkk.....",
        "...kkpplpppkk...",
        "..kppwplpwpppk..",
        ".kpppppppplppk..",
        ".kpwpplpppwppk..",
        ".kkppppppppppkk.",
        "..kkkkkkkkkkkk..",
        "......kssk......",
        "......kssk......",
        "......kssk......",
        ".....ksssk......",
        ".....kssssk.....",
        ".....kkkkkk.....",
        "................",
    ]),
    "cogumelo_brilhante": ({"k": "#123030", "c": "#4fd1c5", "y": "#fff59a", "w": "#e8fff9", "s": "#cfe6d8"}, [
        "..y.........y...",
        "........y.......",
        ".....kkkkkk.....",
        "...kkccwcccky...",
        "..kcccccycccck..",
        ".kcwccyccccwck..",
        ".kcccccccyccck..",
        ".kkccccccccccck.",
        "..kkkkkkkkkkkk..",
        "......kssk......",
        "..y...kssk......",
        "......kssk...y..",
        ".....ksssk......",
        ".....kssssk.....",
        ".....kkkkkk.....",
        "................",
    ]),
    "escama_de_salamandra": ({"k": "#3a1208", "r": "#c4341b", "o": "#f08c3c", "y": "#ffd23f"}, [
        "................",
        "................",
        "......kkkk......",
        "....kkooyokk....",
        "...kooyyyooook..",
        "..koryyyooorrk..",
        "..korryoooorrk..",
        ".korrrooorrrrrk.",
        ".korrrrrrrrrrrk.",
        ".kkrrrrrrrrrrkk.",
        "..kkrrrrrrrrkk..",
        "....kkrrrrkk....",
        "......kkkk......",
        "................",
        "................",
        "................",
    ]),
    "pena_de_corvo": ({"k": "#0e0e14", "d": "#2b2b3a", "b": "#4a4a6a", "h": "#8a8ab0"}, [
        "................",
        "...........kk...",
        "..........kddk..",
        ".........kdbdk..",
        "........kdbhdk..",
        ".......kdbhddk..",
        "......kdbhddk...",
        ".....kdbhddk....",
        "....kdbhddk.....",
        "...kdbhddk......",
        "...kdhddk.......",
        "..kdhdkk........",
        "..khk...........",
        ".kk.............",
        "kk..............",
        "................",
    ]),
    "cristal_de_quartzo": ({"k": "#1c2a3a", "c": "#9fe7ff", "w": "#ffffff", "b": "#5fa8d8", "r": "#6b5a4a"}, [
        "................",
        "......kk........",
        ".....kwck.......",
        ".....kwck..kk...",
        "....kwccbk.kck..",
        "....kwccbkkcbk..",
        "...kkwccbkkcbk..",
        "...kcwccbkcwcbk.",
        "...kcwccbkcwcbk.",
        "...kcwccbkcwcbk.",
        "..kkkkkkkkkkkkk.",
        "..krrrrrrrrrrrk.",
        "...kkkkkkkkkkk..",
        "................",
        "................",
        "................",
    ]),
    "pepita_de_cobre": ({"k": "#3a1a08", "c": "#c8743c", "l": "#f0a868", "d": "#8a4a20", "w": "#ffe0c0"}, [
        "................",
        "................",
        "................",
        "......kkkk......",
        "....kkcllckk....",
        "...kcclwlccck...",
        "..kcclllcccdck..",
        "..kccllcccddck..",
        "..kcccccccdddk..",
        "..kdccccdddddk..",
        "...kddcdddddk...",
        "....kkkddkkk....",
        "......kkk.......",
        "................",
        "................",
        "................",
    ]),
}

# Os baús (bau_fechado.png e bau_aberto.png) foram desenhados à parte, com apoio do Gemini,
# e não são gerados por este script.

# Frasco de poção: 'L' recebe a cor da poção, 'H' é o brilho dela.
FRASCO = [
    "................",
    "......kkkk......",
    "......kttk......",
    ".......kk.......",
    "......kggk......",
    "......kggk......",
    ".....kgLLgk.....",
    "....kgLLLLgk....",
    "...kgHLLLLLgk...",
    "...kgHLLLLLgk...",
    "...kgLLLLLLgk...",
    "...kgLLLLLLgk...",
    "....kgLLLLgk....",
    ".....kggggk.....",
    "......kkkk......",
    "................",
]


def hex_rgba(cor):
    cor = cor.lstrip("#")
    return tuple(int(cor[i:i + 2], 16) for i in (0, 2, 4)) + (255,)


def clarear(rgba, fator):
    return tuple(min(255, int(c + (255 - c) * fator)) for c in rgba[:3]) + (255,)


def desenhar(mapa, paleta, destino):
    img = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    for y, linha in enumerate(mapa):
        for x, ch in enumerate(linha):
            if ch != ".":
                img.putpixel((x, y), paleta[ch])
    img.save(destino)


def main():
    SAIDA.mkdir(parents=True, exist_ok=True)
    dados = json.loads((RAIZ / "data" / "alquimia.json").read_text(encoding="utf-8"))

    for id_ingrediente, (cores, mapa) in INGREDIENTES.items():
        paleta = {k: hex_rgba(v) for k, v in cores.items()}
        desenhar(mapa, paleta, SAIDA / f"{id_ingrediente}.png")

    for receita in dados["receitas"]:
        pocao = receita["pocao"]
        liquido = hex_rgba(pocao["cor"])
        paleta = {
            "k": hex_rgba("#1a1420"),
            "t": hex_rgba("#8a5a3a"),
            "g": hex_rgba("#d8eef5"),
            "L": liquido,
            "H": clarear(liquido, 0.6),
        }
        desenhar(FRASCO, paleta, SAIDA / f"{pocao['id']}.png")

    print(f"Ícones gerados em {SAIDA}")


if __name__ == "__main__":
    main()
