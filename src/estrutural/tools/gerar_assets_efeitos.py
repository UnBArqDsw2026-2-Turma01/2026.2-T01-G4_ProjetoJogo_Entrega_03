"""Gera em pixel art os assets dos efeitos das poções (padrão Decorator).

Complementa as artes feitas com IA de imagem (tools/processar_brutos.py) com
tudo o que é pequeno ou geométrico o bastante para ser desenhado por código:

- 22 ícones de status 16x16 (assets/icones/status/);
- 4 sobreposições animadas de status (assets/combate/status/);
- 12 efeitos visuais de combate (assets/combate/vfx/), os neutros em tons de
  cinza para serem recoloridos no jogo;
- a interface de combate (assets/combate/ui/);
- a luz e os segredos da exploração (assets/mundo/luz/ e assets/mundo/segredos/).

Os efeitos animados usam sorteio com semente fixa: rodar de novo gera
exatamente as mesmas imagens.

Uso (de dentro de src/estrutural/):  python3 tools/gerar_assets_efeitos.py [--previa saida.png]
Requer Pillow (pip install pillow).
"""
import argparse
import math
import random
from pathlib import Path

from PIL import Image, ImageDraw

RAIZ = Path(__file__).resolve().parent.parent
ASSETS = RAIZ / "assets"

# Cores compartilhadas.
CONTORNO = "#1a1420"  # o mesmo contorno dos frascos de poção
BRANCO, CLARO, MEDIO, ESCURO = "#ffffff", "#d8dde6", "#a3abb8", "#6b7280"  # neutros (recoloríveis)


# --- Utilidades ---

def cor(hexa):
    """'#rrggbb' ou '#rrggbbaa' para RGBA."""
    hexa = hexa.lstrip("#")
    canais = [int(hexa[i:i + 2], 16) for i in range(0, len(hexa), 2)]
    return tuple(canais) + (() if len(canais) == 4 else (255,))


def grade(nome, mapa, paleta):
    """Desenha um mapa de caracteres ('.' = transparente)."""
    largura = len(mapa[0])
    for i, linha in enumerate(mapa):
        if len(linha) != largura:
            raise ValueError(f"{nome}: linha {i} tem {len(linha)} colunas, esperado {largura}")
    img = Image.new("RGBA", (largura, len(mapa)), (0, 0, 0, 0))
    for y, linha in enumerate(mapa):
        for x, ch in enumerate(linha):
            if ch != ".":
                img.putpixel((x, y), cor(paleta[ch]))
    return img


def ponto(img, x, y, c):
    x, y = int(round(x)), int(round(y))
    if 0 <= x < img.width and 0 <= y < img.height:
        img.putpixel((x, y), cor(c) if isinstance(c, str) else c)


def circulo(img, cx, cy, r, c):
    for y in range(img.height):
        for x in range(img.width):
            if (x + 0.5 - cx) ** 2 + (y + 0.5 - cy) ** 2 <= r * r:
                ponto(img, x, y, c)


def anel(img, cx, cy, r, espessura, c, inicio=0.0, fim=360.0, pontilhado=False):
    """Anel (ou arco, em graus) com centro em (cx, cy)."""
    for y in range(img.height):
        for x in range(img.width):
            d = math.hypot(x + 0.5 - cx, y + 0.5 - cy)
            if r - espessura < d <= r:
                angulo = math.degrees(math.atan2(y + 0.5 - cy, x + 0.5 - cx)) % 360
                if inicio <= angulo <= fim and not (pontilhado and (x + y) % 2):
                    ponto(img, x, y, c)


def anel_eliptico(img, cx, cy, rx, ry, c, pontilhado=False):
    """Contorno de elipse com 1 px (anel visto no chão)."""
    for i in range(360):
        x = int(cx + rx * math.cos(math.radians(i)))
        y = int(cy + ry * math.sin(math.radians(i)))
        if not (pontilhado and (x + y) % 2):
            ponto(img, x, y, c)


def contornar(img, c):
    """Contorno de 1 px em volta dos pixels opacos (vizinhança de 4)."""
    origem = img.copy()
    px = origem.load()
    for y in range(img.height):
        for x in range(img.width):
            if px[x, y][3]:
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                nx, ny = x + dx, y + dy
                if 0 <= nx < img.width and 0 <= ny < img.height and px[nx, ny][3] == 255:
                    img.putpixel((x, y), cor(c))
                    break
    return img


def nova(largura, altura=None):
    return Image.new("RGBA", (largura, altura or largura), (0, 0, 0, 0))


def colar(destino, mapa, paleta, x, y, nome="sprite"):
    destino.alpha_composite(grade(nome, mapa, paleta), (x, y))


# --- 1. Ícones de status (16x16) ---

ESCUDO = [
    "................",
    "..kkkkkkkkkkkk..",
    "..kHHHHHHHHMMk..",
    "..kHMMMMMMMMDk..",
    "..kHMMMMMMMMDk..",
    "..kHMMMMMMMMDk..",
    "..kHMMMMMMMMDk..",
    "..kHMMMMMMMMDk..",
    "..kHMMMMMMMMDk..",
    "...kHMMMMMMDk...",
    "...kHMMMMMMDk...",
    "....kHMMMMDk....",
    ".....kMMMMk.....",
    "......kMMk......",
    ".......kk.......",
    "................",
]

# Emblemas 6x7, desenhados sobre o escudo na coluna 5, linha 3.
EMBLEMAS = {
    "seta_cima": ["..YY..", ".YYYY.", "YYYYYY", "..YY..", "..YY..", "..YY..", "......"],
    "seta_baixo": ["..RR..", "..RR..", "..RR..", "RRRRRR", ".RRRR.", "..RR..", "......"],
    "estrela": ["..WW..", "..WW..", "WWWWWW", "WWWWWW", "..WW..", "..WW..", "......"],
    "chama": ["..R...", "..RR..", ".ROR..", ".ROOR.", "ROYYOR", "ROYYOR", ".RRRR."],
    "floco": ["C.C.C.", ".CCC..", "CCWCC.", ".CCC..", "C.C.C.", "......", "......"],
    "gota": ["..GG..", "..GG..", ".GLGG.", "GLGGGG", "GGGGGG", ".GGGG.", "..GG.."],
}

ACO = {"k": CONTORNO, "H": "#e0e6ee", "M": "#a8b4c4", "D": "#6f7c8f"}


def escudo(nome, emblema, paleta, extras=()):
    mapa = [list(linha) for linha in ESCUDO]
    for y, linha in enumerate(EMBLEMAS[emblema]):
        for x, ch in enumerate(linha):
            if ch != ".":
                mapa[3 + y][5 + x] = ch
    for x, y in extras:  # rachaduras etc.
        mapa[y][x] = "k"
    return grade(nome, ["".join(linha) for linha in mapa], paleta)


ICONES = {
    "forca": ({"k": CONTORNO, "S": "#f0b890", "D": "#c88060", "R": "#c4341b"}, [
        "................",
        "................",
        "....kkkkkkk.....",
        "...kSSkSSkSk....",
        "...kSSkSSkSkk...",
        "..kkSSkSSkSkSk..",
        "..kSkkkkkkkkSk..",
        "..kSSSSSSSSSSk..",
        "..kSSSSSSSSSDk..",
        "..kSSSSSSSSDDk..",
        "...kSSSSSSDDk...",
        "....kRRRRRRk....",
        "....kRRRRRRk....",
        "....kRRRRRRk....",
        "....kkkkkkkk....",
        "................",
    ]),
    "velocidade": ({"k": CONTORNO, "B": "#8a5a3a", "D": "#5a3a22", "W": "#f2f2f2"}, [
        "................",
        "................",
        "..k.............",
        ".kWk..kkkk......",
        "kWWWk.kBBk......",
        "kWWWWkkBBk......",
        ".kWWWWkBBk......",
        "..kWWWkBBk......",
        "...kkWkBBk......",
        ".....kkBBBkk....",
        "......kBBBBBkk..",
        "......kBBBBBBBk.",
        "......kDDDDDDDk.",
        "......kkkkkkkkk.",
        "................",
        "................",
    ]),
    "precisao": ({"k": CONTORNO, "R": "#d83a2a", "W": "#f2e8e0"}, [
        "................",
        "................",
        ".....kkkkkk.....",
        "....kRRRRRRk....",
        "...kRWWWWWWRk...",
        "..kRWRRRRRRWRk..",
        "..kRWRWWWWRWRk..",
        "..kRWRWRRWRWRk..",
        "..kRWRWRRWRWRk..",
        "..kRWRWWWWRWRk..",
        "..kRWRRRRRRWRk..",
        "...kRWWWWWWRk...",
        "....kRRRRRRk....",
        ".....kkkkkk.....",
        "................",
        "................",
    ]),
    "dano_magico": ({"k": CONTORNO, "P": "#b46fe0", "W": "#f2e8f7"}, [
        "................",
        ".......k........",
        "......kWk.......",
        "......kPk.......",
        "......kPk.......",
        ".....kPPPk......",
        "..kkkPPWPPkkk...",
        ".kWPPPWWWPPPWk..",
        "..kkkPPWPPkkk...",
        ".....kPPPk......",
        "......kPk.......",
        "......kPk.......",
        "......kWk.......",
        ".......k........",
        "................",
        "................",
    ]),
    "sorte": ({"k": CONTORNO, "G": "#3f8f4f", "L": "#7fd18a", "D": "#2a5a30"}, [
        "................",
        "...kkk....kkk...",
        "..kGGGk..kGGGk..",
        "..kGLGGkkGGLGk..",
        "..kGGGGGGGGGGk..",
        "...kGGGGGGGGk...",
        "....kkGGGGkk....",
        "...kGGGGGGGGk...",
        "..kGGGGGGGGGGk..",
        "..kGLGGkkGGLGk..",
        "..kGGGk..kGGGk..",
        "...kkkk..kDk....",
        "..........kDk...",
        "...........kDk..",
        "............kk..",
        "................",
    ]),
    "regeneracao": ({"k": CONTORNO, "G": "#4fbf5f", "L": "#a8f0b0", "D": "#2a7a3a", "W": "#ffffff"}, [
        "................",
        "...kkk....kkk...",
        "..kGGGk..kGGGk..",
        ".kGLLGGkkGGGGGk.",
        ".kGLGGGWWGGGGGk.",
        ".kGGGGGWWGGGGGk.",
        ".kGGGWWWWWWGGDk.",
        ".kGGGWWWWWWGGDk.",
        "..kGGGGWWGGGDk..",
        "...kGGGWWGGDk...",
        "....kGGGGGDk....",
        ".....kGGGDk.....",
        "......kGDk......",
        ".......kk.......",
        "................",
        "................",
    ]),
    "veneno": ({"k": CONTORNO, "G": "#7ac74f", "L": "#c8f08a", "D": "#3f7a2a"}, [
        "................",
        ".......kk.......",
        "......kGGk......",
        "......kGGk......",
        ".....kGLGGk.....",
        ".....kGLGGk.....",
        "....kGLGGGGk....",
        "....kGGGGGGk....",
        "...kGGGGGGGGk...",
        "...kGGkGGkGGk...",
        "...kGGGGGGGDk...",
        "...kGGGkkGGDk...",
        "....kGGGGGDk....",
        ".....kkkkkk.....",
        "................",
        "................",
    ]),
    "sono": ({"k": CONTORNO, "Y": "#fff59a", "W": "#ffffff"}, [
        "................",
        ".........kkkkkk.",
        ".........kWWWWk.",
        "...kkk...kkkWkk.",
        "..kYYk.....kWk..",
        ".kYYk.....kWkkk.",
        ".kYk.....kWWWWk.",
        "kYYk.....kkkkkk.",
        "kYYk............",
        "kYYk............",
        "kYYYk.......kk..",
        ".kYYYk.....kYk..",
        ".kYYYYkkkkkYYk..",
        "..kYYYYYYYYYk...",
        "...kkYYYYYkk....",
        ".....kkkkk......",
    ]),
    "invisivel": ({"k": CONTORNO, "g": "#c8d0dcd0", "F": "#ffffff60"}, [
        "................",
        "......gggg......",
        "....ggFFFFgg....",
        "...gFFFFFFFFg...",
        "..gFFFFFFFFFFg..",
        "..gFFkkFFkkFFg..",
        "..gFFkkFFkkFFg..",
        ".gFFFFFFFFFFFFg.",
        ".gFFFFFFFFFFFFg.",
        ".gFFFFFFFFFFFFg.",
        ".gFFFFFFFFFFFFg.",
        ".gFFFFFFFFFFFFg.",
        ".gFFFgFFFFgFFFg.",
        ".gFg.gFFg.gFFg..",
        "..g...gg...gg...",
        "................",
    ]),
    "esquiva": ({"k": CONTORNO, "W": "#f2f2f2", "G": "#8a93a3", "m": "#9fe7ff"}, [
        "................",
        "...........kkk..",
        ".........kkWWGk.",
        "........kWWWGWk.",
        ".mmm...kWWWGWWk.",
        ".......kWWGWWk..",
        "..mm..kWWGWWWk..",
        "......kWGWWWk...",
        "mmm..kWGWWWk....",
        ".....kGWWWk.....",
        "....kGWWkk......",
        "...kGkkk........",
        "..kGk...........",
        ".kGk............",
        ".kk.............",
        "................",
    ]),
    "reflexo": ({"k": CONTORNO, "W": "#ffffff", "C": "#9fe7ff", "R": "#e04040", "O": "#f08c3c",
                 "Y": "#ffd23f", "G": "#4fbf5f", "B": "#4a7ae0", "P": "#9a4ad8"}, [
        "................",
        "................",
        ".....k..........",
        "....kWk.........",
        "....kWCk........",
        "...kWCCk........",
        "...kWCCCk..RRRRR",
        "..kWCCCCk.OOOOOO",
        "WWkWCCCCCkYYYYYY",
        ".kWCCCCCCkGGGGGG",
        ".kWCCCCCCCkBBBBB",
        "kWCCCCCCCCkPPPP.",
        "kkkkkkkkkkkk....",
        "................",
        "................",
        "................",
    ]),
    "arma_fogo": ({"k": CONTORNO, "W": "#f2f2f2", "S": "#a8b4c4", "H": "#ffd23f", "B": "#8a5a3a",
                   "R": "#c4341b", "O": "#f08c3c", "Y": "#ffd23f"}, [
        ".......kk.......",
        "......kWWk......",
        "..R...kWSk...R..",
        ".RO...kWSk..ROR.",
        ".ROR..kWSk..OYO.",
        "ROYO..kWSk.ROYR.",
        "RYYO..kWSk.OYYO.",
        ".OYR..kWSk..OYR.",
        "..R...kWSk...R..",
        "...kkkkWSkkkk...",
        "...kHHHHHHHHk...",
        "...kkkkBBkkkk...",
        "......kBBk......",
        "......kBBk......",
        "......kHHk......",
        ".......kk.......",
    ]),
    "visao_noturna": ({"k": CONTORNO, "N": "#1d2a4a", "S": "#e8f4ff", "Y": "#ffd23f", "K": "#0a0a12"}, [
        "................",
        ".....kkkkkk.....",
        "...kkNNNNNNkk...",
        "..kNNNNNNNNSNk..",
        ".kNNNNNNNNNNNNk.",
        ".kNNNYYYYYYNNNk.",
        "kNNYYYYKKYYYYNNk",
        "kNYYYYYKKYYYYYNk",
        "kNYYYYYKKYYYYYNk",
        "kNNYYYYKKYYYYNNk",
        ".kNNNYYYYYYNNNk.",
        ".kNNNNNNNNNNNNk.",
        "..kNNNNNNNNNNk..",
        "...kkNNNNNNkk...",
        ".....kkkkkk.....",
        "................",
    ]),
    "luz": ({"k": CONTORNO, "C": "#5fd3ff", "W": "#e8fff9"}, [
        "................",
        "........k.......",
        ".....k.kCk......",
        "....kCkkCk......",
        "....kCkCCk......",
        ".....kCWCk......",
        "....kCWWWCk.....",
        "...kCCWWWCCk....",
        "...kCWWWWWCk....",
        "..kCWWWWWWWCk...",
        "..kCWWWWWWWCk...",
        "..kCCWWWWWCCk...",
        "...kCCWWWCCk....",
        "....kkCCCkk.....",
        "......kkk.......",
        "................",
    ]),
}

OLHO = [
    "................",
    "................",
    "................",
    "................",
    ".....kkkkkk.....",
    "...kkWWWWWWkk...",
    "..kWWWBBBBWWWk..",
    ".kWWWBBKKBBWWWk.",
    ".kWWWBBKKBBWWWk.",
    "..kWWWBBBBWWWk..",
    "...kkWWWWWWkk...",
    ".....kkkkkk.....",
    "................",
    "................",
    "................",
    "................",
]


def icone_cegueira():
    img = grade("cegueira", OLHO, {"k": CONTORNO, "W": "#f2f2f2", "B": "#4a7ae0", "K": "#0a0a12"})
    for i in range(12):  # faixa diagonal riscando o olho
        ponto(img, 13 - i, 2 + i, "#d83a2a")
        ponto(img, 14 - i, 2 + i, "#ff6a5a")
    return contornar(img, CONTORNO)


def icone_confusao():
    img = nova(16)
    voltas, r0, r1 = 1.75, 1.0, 6.3
    for i in range(400):
        t = i / 399
        angulo = t * voltas * 2 * math.pi
        r = r0 + (r1 - r0) * t
        ponto(img, 7.5 + r * math.cos(angulo) - 0.5, 7.5 + r * math.sin(angulo) - 0.5, "#ffd23f")
    return contornar(img, CONTORNO)


def gerar_icones():
    saida = ASSETS / "icones" / "status"
    imagens = {nome: grade(nome, mapa, paleta) for nome, (paleta, mapa) in ICONES.items()}
    imagens["defesa"] = escudo("defesa", "seta_cima", {**ACO, "Y": "#ffd23f"})
    imagens["defesa_reduzida"] = escudo(
        "defesa_reduzida", "seta_baixo",
        {"k": CONTORNO, "H": "#e0a070", "M": "#b0673a", "D": "#6f3a1f", "R": "#d83a2a"},
        extras=[(4, 3), (4, 4), (5, 5), (5, 6), (4, 7), (4, 8), (5, 9)])
    imagens["defesa_magica"] = escudo(
        "defesa_magica", "estrela", {"k": CONTORNO, "H": "#d9c8ff", "M": "#8a6ad8", "D": "#4a2f8a", "W": "#e8fff9"})
    imagens["resistencia_fogo"] = escudo(
        "resistencia_fogo", "chama", {**ACO, "R": "#c4341b", "O": "#f08c3c", "Y": "#ffd23f"})
    imagens["resistencia_frio"] = escudo("resistencia_frio", "floco", {**ACO, "C": "#5fd3ff", "W": "#ffffff"})
    imagens["imune_veneno"] = escudo("imune_veneno", "gota", {**ACO, "G": "#7ac74f", "L": "#c8f08a"})
    imagens["cegueira"] = icone_cegueira()
    imagens["confusao"] = icone_confusao()
    return [salvar(img, saida / f"{nome}.png") for nome, img in sorted(imagens.items())]


# --- 2. Sobreposições de status (16x16, animadas) ---

Z_PEQUENO = ["zzzz", "..zz", ".zz.", "zzzz"]
Z_GRANDE = ["ZZZZZ", "...ZZ", "..ZZ.", ".ZZ..", "ZZZZZ"]


def sobreposicao_zzz():
    paleta = {"z": "#e8f4ff", "Z": "#ffffff"}
    posicoes = [[("z", 3, 11)], [("z", 5, 7), ("z", 2, 11)], [("Z", 9, 2), ("z", 5, 7)]]
    quadros = []
    for letras in posicoes:
        img = nova(16)
        for letra, x, y in letras:
            colar(img, Z_PEQUENO if letra == "z" else Z_GRANDE, paleta, x, y)
        quadros.append(contornar(img, CONTORNO))
    return quadros


def sobreposicao_estrelas():
    quadros = []
    for t in range(4):
        img = nova(16)
        for i in range(3):
            angulo = math.radians(t * 30 + i * 120)
            x, y = 7.5 + 5.5 * math.cos(angulo), 6 + 2.2 * math.sin(angulo)
            for dx, dy in ((0, 0), (1, 0), (-1, 0), (0, 1), (0, -1)):
                ponto(img, x + dx, y + dy, "#ffffff" if (dx, dy) == (0, 0) else "#ffd23f")
        quadros.append(contornar(img, CONTORNO))
    return quadros


def sobreposicao_bolhas():
    # (x, y inicial, raio): sobem 3 px por quadro; a última estoura no quadro 4.
    bolhas = [(4, 13, 1.6), (9, 15, 2.2), (12, 12, 1.2)]
    quadros = []
    for t in range(4):
        img = nova(16)
        for i, (x, y, r) in enumerate(bolhas):
            y -= 3 * t
            if i == 1 and t == 3:
                anel(img, x, y, r + 1, 1, "#7ac74f", pontilhado=True)
                continue
            circulo(img, x, y, r, "#7ac74f")
            ponto(img, x - r / 2, y - r / 2, "#c8f08a")
        quadros.append(contornar(img, "#2a4a1a"))
    return quadros


def sobreposicao_nuvem_cega():
    quadros = []
    for t in range(3):
        img = nova(16)
        for cx, cy, r in ((4.5, 7, 2.3), (8, 6, 3.0), (11.5, 7, 2.3)):
            circulo(img, cx + t - 1, cy, r, "#8b93a1")
        for cx, cy, r in ((7, 5.5, 1.4), (10, 6, 1.1)):
            circulo(img, cx + t - 1, cy, r, "#c0c6d0")
        quadros.append(contornar(img, "#3f4450"))
    return quadros


# --- 3. Efeitos visuais de combate ---

def efeito_fogo():
    rng = random.Random(1)
    alturas = [10, 18, 24, 22, 16, 8]
    faixas = [(0.8, "#fff59a"), (0.6, "#ffd23f"), (0.4, "#f08c3c"), (0.18, "#c4341b")]
    quadros = []
    for t, altura in enumerate(alturas):
        img = nova(32)
        fase = t * 1.3
        for y in range(32):
            for x in range(32):
                dy = (29 - y) / altura
                if not 0 <= dy <= 1:
                    continue
                meia = 7 * (1 - dy ** 1.4) * (1 + 0.15 * math.sin(dy * 8 + fase))
                dx = abs(x + 0.5 - 16 - math.sin(dy * 5 + fase) * 2 * dy)
                if meia <= 0 or dx > meia:
                    continue
                v = (1 - dx / meia) * (1 - dy * 0.6) + rng.uniform(-0.12, 0.12)
                for limite, c in faixas:
                    if v > limite:
                        ponto(img, x, y, c)
                        break
        quadros.append(contornar(img, "#5a1408"))
    return quadros


def caminho_raio(rng, x0=16):
    pontos, x = [(x0, 0)], x0
    for y in range(6, 48, 6):
        x = max(6, min(25, x + rng.randint(-4, 4)))
        pontos.append((x, y))
    pontos.append((x, 47))
    return pontos


def efeito_raio():
    rng = random.Random(2)
    quadros = []
    caminhos = [caminho_raio(rng), caminho_raio(rng), caminho_raio(rng)]
    for t in range(5):
        img = nova(32, 48)
        desenho = ImageDraw.Draw(img)
        if t == 0:
            trecho = caminhos[0][:4]
        elif t in (1, 3):
            trecho = caminhos[0]
        else:
            trecho = caminhos[1]
        if t < 3:
            desenho.line(trecho, fill=cor("#ffe95c"), width=3)
            desenho.line(trecho, fill=cor(BRANCO), width=1)
            if t == 2:  # ramificação
                meio = trecho[len(trecho) // 2]
                ramo = [meio, (meio[0] + 6, meio[1] + 5), (meio[0] + 9, meio[1] + 12)]
                desenho.line(ramo, fill=cor("#ffe95c"), width=1)
            img = contornar(img, "#5fd3ff")
        elif t == 3:  # sumindo: só o núcleo, tracejado
            for i in range(0, len(trecho) - 1, 2):
                desenho.line(trecho[i:i + 2], fill=cor("#fff59a"), width=1)
        else:  # impacto no chão
            x = trecho[-1][0]
            for dx, dy in ((0, 0), (1, 0), (-1, 0), (0, -1), (2, 0), (-2, 0), (0, -2)):
                ponto(img, x + dx, 44 + dy, "#ffffff")
            for _ in range(8):
                ponto(img, x + rng.randint(-7, 7), 44 + rng.randint(-6, 2), "#ffe95c")
            img = contornar(img, "#5fd3ff")
        quadros.append(img)
    return quadros


def efeito_nuvem():
    rng = random.Random(3)
    bolhas = [(16 + rng.uniform(-7, 7), 18 + rng.uniform(-3, 3), rng.uniform(4, 7)) for _ in range(6)]
    escalas = [0.45, 0.7, 0.9, 1.0, 1.05, 1.1]
    buracos = [0, 0, 0, 0, 0.25, 0.5]
    quadros = []
    for t, escala in enumerate(escalas):
        img = nova(32)
        for bx, by, r in bolhas:
            cx, cy = 16 + (bx - 16) * escala, 18 + (by - 18) * escala
            for y in range(32):
                for x in range(32):
                    d = math.hypot(x + 0.5 - cx, y + 0.5 - cy)
                    if d <= r * escala:
                        relativo = (y + 0.5 - cy) / (r * escala)
                        c = BRANCO if relativo < -0.4 else CLARO if relativo < 0.35 else MEDIO
                        atual = img.getpixel((x, y))
                        if not atual[3] or c == MEDIO or atual == cor(MEDIO):
                            ponto(img, x, y, c)
        if buracos[t]:
            for y in range(32):
                for x in range(32):
                    if img.getpixel((x, y))[3] and rng.random() < buracos[t]:
                        ponto(img, x, y, (0, 0, 0, 0))
        quadros.append(contornar(img, ESCURO))
    return quadros


def efeito_cinzas():
    rng = random.Random(4)
    particulas = [(16 + rng.uniform(-4, 4), 8 + rng.uniform(-3, 3), rng.uniform(-1.6, 1.6), rng.uniform(1.5, 3.5),
                   rng.choice([1, 1, 2]), rng.choice(["#2b2b2b", "#2b2b2b", "#5b5b5b", "#f08c3c"]))
                  for _ in range(22)]
    quadros = []
    for t in range(6):
        img = nova(32)
        vivas = len(particulas) if t < 4 else len(particulas) * (6 - t) // 3
        for x, y, vx, vy, tamanho, c in particulas[:vivas]:
            px = x + vx * t * 1.5 + math.sin(t + y) * 1.2
            py = y + vy * t
            for dx in range(tamanho):
                for dy in range(tamanho):
                    ponto(img, px + dx, py + dy, c)
        quadros.append(img)
    return quadros


def raios(img, cx, cy, comprimento, quantos, c, inicio=0):
    for i in range(quantos):
        angulo = math.radians(inicio + i * 360 / quantos)
        for passo in range(2, comprimento):
            ponto(img, cx + math.cos(angulo) * passo - 0.5, cy + math.sin(angulo) * passo - 0.5, c)


def efeito_clarao():
    quadros = []
    img = nova(32)
    raios(img, 16, 16, 5, 4, "#fff59a")
    circulo(img, 16, 16, 2, BRANCO)
    quadros.append(img)
    img = nova(32)
    raios(img, 16, 16, 15, 8, "#fff59a", inicio=22.5)
    raios(img, 16, 16, 12, 4, BRANCO)
    anel(img, 16, 16, 6, 1.2, "#fff59a")
    circulo(img, 16, 16, 4.5, BRANCO)
    quadros.append(img)
    img = nova(32)
    anel(img, 16, 16, 10, 2, "#fff59a")
    anel(img, 16, 16, 8, 1, BRANCO, pontilhado=True)
    quadros.append(img)
    img = nova(32)
    anel(img, 16, 16, 13, 1, "#fff59a", pontilhado=True)
    quadros.append(img)
    return quadros


def efeito_brilho_cura():
    rng = random.Random(5)
    brilhos = [(rng.uniform(5, 27), rng.uniform(20, 30), rng.uniform(2, 4), rng.randint(0, 2)) for _ in range(7)]
    quadros = []
    for t in range(6):
        img = nova(32)
        for i, (x, y, velocidade, fase) in enumerate(brilhos):
            if t >= 4 and i % 2:
                continue  # some aos poucos
            py = y - velocidade * t
            if (t + fase) % 3 == 0:  # cruz grande
                for d in (-2, -1, 1, 2):
                    ponto(img, x + d, py, CLARO)
                    ponto(img, x, py + d, CLARO)
                ponto(img, x, py, BRANCO)
            elif (t + fase) % 3 == 1:  # cruz pequena
                for d in (-1, 1):
                    ponto(img, x + d, py, CLARO)
                    ponto(img, x, py + d, CLARO)
                ponto(img, x, py, BRANCO)
            else:
                ponto(img, x, py, BRANCO)
                ponto(img, x + 1, py, BRANCO)
        quadros.append(contornar(img, MEDIO))
    return quadros


def efeito_aura_buff():
    """Setas subindo e um anel no chão; a versão de debuff é este efeito de cabeça para baixo."""
    seta = [(0, 2), (1, 1), (2, 0), (3, 1), (4, 2), (1, 2), (2, 1), (3, 2)]
    colunas = [(5, 0), (14, 2), (23, 1)]
    quadros = []
    for t in range(6):
        img = nova(32)
        if t < 4:
            anel_eliptico(img, 16, 27, 6 + 3 * t, 2 + t * 0.7, CLARO, pontilhado=t == 3)
        for x, fase in colunas:
            y = 24 - 4 * ((t + fase) % 6)
            c = BRANCO if y > 10 else CLARO
            for dx, dy in seta:
                ponto(img, x + dx, y + dy, c)
        quadros.append(contornar(img, MEDIO))
    return quadros


def efeito_escudo():
    arcos = [120, 240, 360, 360, 360, 360]
    quadros = []
    for t, fim in enumerate(arcos):
        img = nova(32)
        if t == 2:
            for y in range(32):
                for x in range(32):
                    if math.hypot(x + 0.5 - 16, y + 0.5 - 16) < 11 and (x + y) % 2 == 0:
                        ponto(img, x, y, (216, 221, 230, 110))
        anel(img, 16, 16, 13, 2, CLARO, inicio=0, fim=fim, pontilhado=t == 5)
        if t in (3, 4):
            inicio = 200 + 30 * (t - 3)
            anel(img, 16, 16, 12, 1, BRANCO, inicio=inicio, fim=inicio + 50)
        quadros.append(contornar(img, MEDIO) if t < 5 else img)
    return quadros


def efeito_prisma():
    cores_raio = ["#e04040", "#f08c3c", "#ffd23f", "#4fbf5f", "#4a7ae0", "#9a4ad8"]
    comprimentos = [0, 6, 12, 16, 16, 16]
    quadros = []
    for t, comprimento in enumerate(comprimentos):
        img = nova(32)
        for i, c in enumerate(cores_raio):
            angulo = math.radians(-30 + i * 12)
            for lado in (1, -1):
                for passo in range(6, 6 + comprimento):
                    if t >= 4 and (passo + t) % 2:
                        continue  # raios se apagando
                    ponto(img, 16 + lado * math.cos(angulo) * passo - 0.5, 16 + math.sin(angulo) * passo - 0.5, c)
        faces = ("#ffffff", "#9fe7ff") if t % 2 == 0 else ("#9fe7ff", "#f2c0f0")
        for y in range(8, 24):
            meia = 5 * (1 - abs(y + 0.5 - 16) / 8)
            for x in range(32):
                if abs(x + 0.5 - 16) <= meia:
                    ponto(img, x, y, faces[0] if x < 16 else faces[1])
        quadros.append(contornar(img, "#3a3a5a"))
    return quadros


PENA = ["......kk", ".....kWk", "....kWGk", "...kWGWk", "..kWGWk.", ".kWGWk..", ".kGkk...", "kGk....."]


def efeito_penas():
    paleta = {"k": ESCURO, "W": BRANCO, "G": MEDIO}
    penas = [(4, 1, 0.0), (19, 0, 1.5), (11, 9, 3.0)]
    quadros = []
    for t in range(4):
        img = nova(32)
        for x, y, fase in penas:
            px = x + 3 * math.sin(t * math.pi / 2 + fase)
            pena = grade("pena", PENA, paleta)
            if (t + int(fase)) % 2:
                pena = pena.transpose(Image.FLIP_LEFT_RIGHT)
            img.alpha_composite(pena, (int(round(px)), y + 4 * t))
        quadros.append(contornar(img, ESCURO))
    return quadros


def efeito_dissipar():
    raios_t = [3, 6, 9, 11, 13, 14]
    quadros = []
    for t, r in enumerate(raios_t):
        img = nova(32)
        if t == 0:
            circulo(img, 16, 16, 3, BRANCO)
        for i in range(12):
            if (t >= 3 and i % 3 == 0) or (t >= 5 and i % 2):
                continue
            angulo = math.radians(i * 30 + t * 8)
            x, y = 16 + r * math.cos(angulo) - 0.5, 16 + r * math.sin(angulo) - 0.5
            ponto(img, x, y, BRANCO)
            if t < 4:
                ponto(img, x + 1, y, CLARO)
                ponto(img, x, y + 1, CLARO)
                ponto(img, x + 1, y + 1, CLARO)
        quadros.append(contornar(img, MEDIO))
    return quadros


# --- 4. Interface de combate ---

def interface():
    imagens = {}
    sombra = nova(32, 8)
    for y in range(8):
        for x in range(32):
            d = ((x + 0.5 - 16) / 15.5) ** 2 + ((y + 0.5 - 4) / 3.5) ** 2
            if d <= 1:
                ponto(sombra, x, y, (0, 0, 0, 120 if d < 0.45 else 80))
    imagens["sombra"] = sombra

    # Barras no formato do TextureProgressBar: fundo, preenchimento e moldura do mesmo tamanho.
    def barra(cores):
        img = nova(64, 8)
        for y in range(1, 7):
            c = cores[0] if y == 1 else cores[1] if y < 5 else cores[2]
            for x in range(1, 63):
                ponto(img, x, y, c)
        return img

    imagens["barra_fundo"] = barra(["#2a1f24", "#2a1f24", "#1a1216"])
    imagens["barra_hp"] = barra(["#ff8a80", "#d83a2a", "#9a1f1f"])
    imagens["barra_mp"] = barra(["#8ac8ff", "#3a7ad8", "#1f3f9a"])
    moldura = nova(64, 8)
    for x in range(1, 63):
        ponto(moldura, x, 0, "#3b2a2a")
        ponto(moldura, x, 7, "#3b2a2a")
    for y in range(1, 7):
        ponto(moldura, 0, y, "#3b2a2a")
        ponto(moldura, 63, y, "#3b2a2a")
    imagens["barra_moldura"] = moldura

    seta = ["..kkkk..", "..kYYk..", "..kYYk..", "kkkYYkkk", ".kYYYYk.", "..kYYk..", "...kk...", "........"]
    paleta = {"k": CONTORNO, "Y": "#ffd23f"}
    imagens["cursor_alvo_frame_1"] = grade("cursor", seta, paleta)
    imagens["cursor_alvo_frame_2"] = grade("cursor", ["........"] + seta[:-1], paleta)
    return imagens


# --- 5. Exploração: luz e segredos ---

FOGO_FATUO = [
    ["...C....", "..CC.C..", "..CWCC..", ".CWWC...", ".CWWWC..", ".CWWWC..", "..CCC...", "........"],
    ["....C...", ".C.CC...", ".CCWCC..", ".CWWWC..", ".CWWWC..", ".CWWWC..", "..CCC...", "........"],
    ["..C.C...", "..CCC...", "..CWC...", ".CWWWC..", ".CWWWC..", ".CWWWC..", "..CCC...", "........"],
    ["........", "...C..C.", "..CWCCC.", ".CWWWC..", ".CWWWC..", ".CWWWC..", "..CCC...", "........"],
]

RUNA = [
    "................",
    "................",
    "................",
    ".....kkkkkk.....",
    "...kkSSSSSSkk...",
    "..kSSSRRRRSSSk..",
    ".kSSRRSSSSRRSSk.",
    ".kSSRSSRRSSRSSk.",
    ".kSSRSRRRRSRSSk.",
    ".kSSRRSSSSRRSSk.",
    "..kSSSRRRRSSSk..",
    "...kkSSSSSSkk...",
    ".....kkkkkk.....",
    "................",
    "................",
    "................",
]

MARCA = ["...kk...", "..kYYk..", "kkYYYYkk", "kYYWWYYk", ".kYYYYk.", ".kYkkYk.", "kYk..kYk", "kk....kk"]


def exploracao():
    imagens = {}
    luz = nova(128)
    for y in range(128):
        for x in range(128):
            d = math.hypot(x + 0.5 - 64, y + 0.5 - 64) / 64
            if d < 1:
                faixa = math.ceil((1 - d) ** 1.5 * 8) / 8  # 8 degraus, no estilo pixel art
                ponto(luz, x, y, (255, 255, 255, int(255 * faixa)))
    imagens["luz/luz_radial"] = luz
    paleta = {"C": "#5fd3ff", "W": "#e8fff9"}
    for i, mapa in enumerate(FOGO_FATUO, start=1):
        imagens[f"luz/fogo_fatuo_frame_{i}"] = contornar(grade("fogo_fatuo", mapa, paleta), "#1f4a7a")
    brilhos = ["#3a6a8a", "#5fa8d8", "#9fe7ff", "#5fa8d8"]
    for i, brilho in enumerate(brilhos, start=1):
        img = grade("runa", RUNA, {"k": CONTORNO, "S": "#4a4a55", "R": brilho})
        if i == 3:
            for x, y in ((7, 1), (9, 2), (5, 2)):
                ponto(img, x, y, "#9fe7ff")
        imagens[f"segredos/runa_secreta_frame_{i}"] = img
    imagens["segredos/marca_minimapa"] = grade("marca", MARCA, {"k": CONTORNO, "Y": "#ffd23f", "W": "#fff59a"})
    return imagens


# --- Execução ---

def salvar(img, caminho):
    caminho.parent.mkdir(parents=True, exist_ok=True)
    img.save(caminho)
    return caminho


def salvar_quadros(quadros, pasta):
    return [salvar(q, pasta / f"frame_{i}.png") for i, q in enumerate(quadros, start=1)]


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--previa", type=Path, help="salva uma folha de contato com tudo o que foi gerado")
    args = parser.parse_args()

    gerados = gerar_icones()

    status = ASSETS / "combate" / "status"
    for nome, gerador in (("zzz", sobreposicao_zzz), ("estrelas", sobreposicao_estrelas),
                          ("bolhas_veneno", sobreposicao_bolhas), ("nuvem_cega", sobreposicao_nuvem_cega)):
        gerados += salvar_quadros(gerador(), status / nome)

    vfx = ASSETS / "combate" / "vfx"
    aura = efeito_aura_buff()
    efeitos = {
        "fogo": efeito_fogo(), "raio": efeito_raio(), "nuvem": efeito_nuvem(), "cinzas": efeito_cinzas(),
        "clarao": efeito_clarao(), "brilho_cura": efeito_brilho_cura(), "aura_buff": aura,
        "aura_debuff": [q.transpose(Image.FLIP_TOP_BOTTOM) for q in aura], "escudo": efeito_escudo(),
        "prisma": efeito_prisma(), "penas": efeito_penas(), "dissipar": efeito_dissipar(),
    }
    for nome, quadros in efeitos.items():
        gerados += salvar_quadros(quadros, vfx / nome)

    for nome, img in interface().items():
        gerados.append(salvar(img, ASSETS / "combate" / "ui" / f"{nome}.png"))
    for nome, img in exploracao().items():
        gerados.append(salvar(img, ASSETS / "mundo" / f"{nome}.png"))

    print(f"{len(gerados)} arquivos gerados.")
    if args.previa:
        from processar_brutos import salvar_previa
        salvar_previa(gerados, args.previa, zoom=6)


if __name__ == "__main__":
    main()
