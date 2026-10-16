"""Gera os assets de cenario (arvores, arbustos, matos e rochas) em pixel art.

Cria tanto a versao 1x nativa (pixel art) quanto a versao 3x (tamanho do jogo).
Saida: assets/mundo/cenario/
Uso: py tools/gerar_cenario.py
"""
from pathlib import Path
from PIL import Image

RAIZ = Path(__file__).resolve().parent.parent
SAIDA = RAIZ / "assets" / "mundo" / "cenario"

def hex_rgba(cor):
    cor = cor.lstrip("#")
    return tuple(int(cor[i:i + 2], 16) for i in (0, 2, 4)) + (255,)

def desenhar(mapa, paleta, destino_1x, escala=3):
    h = len(mapa)
    w = len(mapa[0])
    img = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    for y, linha in enumerate(mapa):
        for x, ch in enumerate(linha):
            if ch != ".":
                img.putpixel((x, y), paleta[ch])
    img.save(destino_1x)
    
    # Versao 3x com Nearest Neighbor para o Godot
    nome_3x = destino_1x.stem + "_3x.png"
    img_3x = img.resize((w * escala, h * escala), Image.NEAREST)
    img_3x.save(destino_1x.parent / nome_3x)

# --------------------------------------------------------------------------
# Paletas de cores (baseadas na paleta oficial do TinyPixel e do projeto)
# --------------------------------------------------------------------------
PALETA_MATO = {
    "k": "#192c20",  # contorno escuro
    "d": "#2d5438",  # sombra verde
    "m": "#4f8c5b",  # verde medio
    "l": "#78b87c",  # verde claro
    "h": "#aad795",  # pontas iluminadas pelo sol
    "w": "#ffffff",  # petala flor branca
    "y": "#ffd23f",  # miolo amarelo
    "r": "#c4341b",  # flor vermelha / frutinha
    "o": "#f08c3c",  # brilho fruta
}

PALETA_ARBUSTO = {
    "k": "#152418",  # contorno
    "d": "#234229",  # sombra profunda da folhagem
    "m": "#3e7048",  # corpo da folhagem
    "l": "#5fa169",  # folhas superiores
    "h": "#8ec985",  # iluminacao topo
    "r": "#c4341b",  # frutinhas
    "o": "#f08c3c",  # brilho da frutinha
    "w": "#ffe8d6",  # florzinha silvestre
    "y": "#ffd23f",  # miolo flor
}

PALETA_ARVORE = {
    "k": "#121f14",  # contorno copa
    "d": "#1f3b25",  # sombra densa
    "m": "#35663f",  # folhagem media
    "l": "#52945d",  # folhagem clara
    "h": "#7dc27e",  # brilho das copas
    "x": "#a8e099",  # luz solar direta
    "t": "#361b12",  # contorno tronco
    "s": "#593020",  # sombra casca
    "b": "#8a4f32",  # tronco marrom
    "c": "#b8764e",  # casca clara
    "g": "#35663f",  # musgo na base
}

PALETA_PINHEIRO = {
    "k": "#0f1c16",  # contorno
    "d": "#183325",  # verde pinheiro escuro
    "m": "#274f3b",  # verde agulha medio
    "l": "#3d7356",  # verde agulha claro
    "h": "#5c9e78",  # pontas iluminadas
    "t": "#2e1810",  # contorno tronco
    "s": "#4d291b",  # tronco escuro
    "b": "#73432d",  # tronco marrom
}

PALETA_ALQUIMIA = {
    "k": "#181024",  # contorno
    "d": "#2f1b47",  # sombra magica
    "m": "#522b7a",  # purpura medio
    "l": "#824ab5",  # lavanda mistica
    "h": "#b479e3",  # brilho mistico
    "x": "#5ce1e6",  # folhinha luminescente (ciano)
    "t": "#22232e",  # casca prateada escura
    "s": "#3f4052",  # sombra casca
    "b": "#686a82",  # tronco prateado
    "c": "#9ea0b8",  # casca clara
}

PALETA_ROCHA = {
    "k": "#282630",  # contorno pedra
    "d": "#423e4f",  # sombra pedra
    "m": "#635f73",  # pedra media
    "l": "#8a869c",  # pedra clara
    "h": "#b5b1c7",  # topo pedra
    "g": "#35663f",  # musgo escuro
    "e": "#64b082",  # musgo claro
}

PALETA_TOCO = {
    "k": "#361b12",  # contorno
    "s": "#593020",  # casca sombra
    "b": "#8a4f32",  # casca
    "c": "#b8764e",  # casca clara
    "w": "#b07d60",  # anel madeira exterior
    "l": "#d4a27a",  # anel madeira centro
    "h": "#f0c99e",  # cerne claro
    "g": "#35663f",  # musgo
    "e": "#78b87c",  # musgo claro
}

# --------------------------------------------------------------------------
# Sprites 16x16: Matos, Arbustos, Rochas e Tocos
# --------------------------------------------------------------------------
SPRITES_16x16 = {
    "mato_alto": (PALETA_MATO, [
        "................",
        ".....h..........",
        "....hll......h..",
        "....lmh.....hl..",
        "...hmmm.....lm..",
        "..hmlmmk...kmmk.",
        "..lmmmdk..kmmd..",
        ".kmmmmdk..kmmdk.",
        ".kmdmmdk..kmdmk.",
        ".kdmdmdkkkdmdmk.",
        "kddmdmddkddmdmdk",
        "kddddddddddddddk",
        ".kkddddddddddkk.",
        "...kkkkkkkkkk...",
        "................",
        "................",
    ]),
    
    "mato_com_flores": (PALETA_MATO, [
        "................",
        "....www.........",
        "...wwyww...h....",
        "....www...hl....",
        ".....k...hll....",
        "..h..k..hmmm.r..",
        ".hlm.k..lmmkror.",
        ".lmmkk.kmmmdk.k.",
        ".mmmdk.kmmddk.k.",
        "kmmdmdkkmdmdkk..",
        "kddmdmddddmdmdk.",
        "kdddddddddddddk.",
        ".kkddddddddddk..",
        "...kkkkkkkkkk...",
        "................",
        "................",
    ]),
    
    "arbusto_verde": (PALETA_ARBUSTO, [
        "................",
        "......kkkk......",
        "....kkhhllkk....",
        "...khhhhhlllkk..",
        "..khhhllhhllllk.",
        ".khhllllhhhllllk",
        ".khllllmhhhhllmk",
        ".kllllmmmmhllmmk",
        ".kllmmmmddmmmdmk",
        ".kmmmdmdddddddmk",
        ".kmmddddddddddmk",
        "..kmdddddddddmk.",
        "...kkdddddddkk..",
        "....kkdddddk....",
        "......kkkkk.....",
        "................",
    ]),
    
    "arbusto_com_frutas": (PALETA_ARBUSTO, [
        "................",
        "......kkkk......",
        "....kkhhllkk....",
        "...khhorlllkk...",
        "..khhorrhhllllk.",
        ".khhlllrhhhllllk",
        ".khllllmhhhhllmk",
        ".klorrmmmhllmmk.",
        ".klorrmmddmormmk",
        ".kmmmdmddddorrmk",
        ".kmmddddddddmmmk",
        "..kmdddddddddmk.",
        "...kkdddddddkk..",
        "....kkdddddk....",
        "......kkkkk.....",
        "................",
    ]),
    
    "rocha_musgo": (PALETA_ROCHA, [
        "................",
        "................",
        "......kkkkk.....",
        "....kkhhlllmk...",
        "...khhhhllllek..",
        "..khllhhhhgleemk",
        ".khllllhhglemddk",
        ".klllllglleddddk",
        ".kllmmgleeeddddk",
        ".klmmmmegddddddk",
        ".kmmmmmddddddddk",
        ".kmmdddddddddddk",
        "..kdddddddddddk.",
        "...kkddddddddk..",
        ".....kkkkkkkk...",
        "................",
    ]),
    
    "toco_de_arvore": (PALETA_TOCO, [
        "................",
        "................",
        "......kkkkk.....",
        "....kkwhhhllkk..",
        "...kwwhhhhllllk.",
        "..kwwlhhhhhllllk",
        ".kwwllhhhhhhlllk",
        ".kwwwwwlllllgglk",
        ".kccbbbbbbbegggk",
        ".kcbbbbbbbbegggk",
        ".kcbbssbbbbsssbk",
        ".kcbbssbbbbsssbk",
        ".kccbssssbbssssk",
        ".kcccbbsssbbbssk",
        "kkkkkkkkkkkkkkkk",
        "................",
    ]),
}

# --------------------------------------------------------------------------
# Sprites Maiores: Pinheiro (24x40), Arvore Media (32x32), Arvore Grande (32x48)
# --------------------------------------------------------------------------
SPRITE_PINHEIRO = (PALETA_PINHEIRO, [
    "...........kk...........",
    "..........khhk..........",
    ".........khllhk.........",
    ".........khlllhk........",
    "........khhllllk........",
    "........khhllmdk........",
    ".......khhhllmddk.......",
    ".......klllllmmdk.......",
    "......khhllllmmddk......",
    "......khhhllmmdddk......",
    ".....khhhhlllmmdddk.....",
    ".....kkkkkkhhkkkkkk.....",
    "........khhllhk.........",
    ".......khhhlllhk........",
    ".......khhhllllk........",
    "......khhhlllmddk.......",
    "......khhllllmmdk.......",
    ".....khhhllllmmddk......",
    ".....khhhlllmmmddk......",
    "....khhhhlllmmmdddk.....",
    "...khhhhhllllmmddddk....",
    "...kkkkkkkhhllkkkkkk....",
    "........khhhllhk........",
    ".......khhhhlllhk.......",
    "......khhhhhlllllk......",
    "......khhhlllmldddk.....",
    ".....khhhllllmmmddk.....",
    "....khhhhllllmmmdddk....",
    "...khhhhhllllmmmmdddk...",
    "..khhhhhhlllllmmmmdddk..",
    "..khhhhhhlllllmmmmdddk..",
    ".khhhhhhhllllllmmmddddk.",
    ".kkkkkkkkkhhlllkkkkkkkk.",
    ".........ktbbst.........",
    ".........ktbbst.........",
    ".........ktbbst.........",
    ".........ktbbst.........",
    "........ktbbbsst........",
    ".......ktbbbbssst.......",
    ".......kkkkkkkkkk.......",
])

SPRITE_ARVORE_MEDIA = (PALETA_ARVORE, [
    "...........kkkkkkkkk............",
    ".........kkxhhhhhhllkk..........",
    ".......kkxxhhhhhhhlllkk.........",
    "......kxxhhhhhhhhlllllkk........",
    ".....kxxhhhhhhxhhhlllllmk.......",
    "....kxxhhhhhxxxxhhlllllmmk......",
    "...kxxhhhhhxxllxxhllllmmmmk.....",
    "..kxhhhhhhhxllhhxhllmmmmmmmk....",
    "..kxhhhhhhhxhhhhhhllmmmdmmmk....",
    ".kxhhhhhxhhhhhhhlllllmmddmmmk...",
    ".kxhhhxxxxhhhhhlllllmmddddmmk...",
    ".khhhxxllxxhhhllllllmmdddddmk...",
    ".khhhxllllxxhhlllllllmmddddmk...",
    ".khhhhhlllxhhhlllllllmmddddmk...",
    ".khhhhhlllhhhhllllllmmmmdddmk...",
    "..khhhhlllhhhhlllllmmmmmdddmk...",
    "..kxhhhhhxhhhhllllmmmmmddddmk...",
    "..kxxhhhhxhhhllllmmmmmmddddk....",
    "...kxxhhxxhhllllmmmmmdddddk.....",
    "...kxxhhxxhlllmmmmmmdddddk......",
    "....kkkhhhllllmmmmmdddddk.......",
    "......kkhlllllmmmmdddddk........",
    "........kklkktbbstkddk..........",
    "..........kkktbbstkk............",
    ".............tbbst..............",
    ".............tbbst..............",
    ".............tbbst..............",
    ".............tbbst..............",
    "............ktbbstk.............",
    "...........ktcbbbstk............",
    "..........ktcbbbbssstk..........",
    "..........kkkkkkkkkkkk..........",
])

SPRITE_ARVORE_GRANDE = (PALETA_ARVORE, [
    "............kkkkkkkkkk..........",
    "..........kkxxhhhhhhllkk........",
    "........kkxxxxhhhhhhlllkk.......",
    ".......kxxxxxxhhhhhhllllkk......",
    "......kxxxxhhhhxxhhhhllllmk.....",
    ".....kxxxxhhhhxxxxhhhllllmmk....",
    "....kxxxxhhhhxxllxxhhllllmmmk...",
    "...kxxhhhhhhhxllllxhhlllllmmmk..",
    "...kxhhhhhhhxhhhhhxhhllllmmmmk..",
    "..kxhhhhhxhhhhhhhhhhhllllmmmmmk.",
    "..kxhhhxxxxhhhhhhhhhhllllmmmmmk.",
    ".kxhhhxxllxxhhhhhhhhllllmmmmmmk.",
    ".kxhhhxllllxxhhhhhhlllllmmmmmmk.",
    ".khhhhhllllxhhhhhhllllllmmmmmdk.",
    ".khhhhhllllhhhhhhlllllllmmmmmdk.",
    ".khhhhhhlllhhhhhhllllllmmmmmddk.",
    ".khhhhhhlllhhhhhhllllllmmmmmddk.",
    "kxxhhhhhxhhhhhhhxhhllllmmmmddddk",
    "kxxxxhhhxxxxhhhhxxxxlllmmmmddddk",
    "kxxllxxhxxllxxhxxllxxllmmmdddddk",
    "kxhlllxhxhlllxhxhlllxhhmmmdddddk",
    "kxhhhhxhxhhhhxhxhhhhxhlmmmdddddk",
    "khhhhhhhkhhhhhhkhhhhhhlmmmdddddk",
    ".khhhhllmkhhhllmkhhhllmmmdddddk.",
    ".khhhllmmkhhllmmkhhllmmmmddddk..",
    "..khllmmmkkhllmmkkhllmmmmdddk...",
    "...kllmmdk.kllmmk.kllmmmdddk....",
    "....kmmddk..klmdk..kmmddddk.....",
    ".....kdddk...kddk...kddddk......",
    "......kkk....kddk....kkkk.......",
    ".............kddk...............",
    "............ktbbst..............",
    "............ktbbst..............",
    "............ktbbst..............",
    "............ktbbst..............",
    "............ktbbst..............",
    "............ktbbst..............",
    "...........ktcbbstk.............",
    "...........ktcbbstk.............",
    "..........ktccbbbstk............",
    "..........ktcbbbbstk............",
    ".........ktcbbbbbsstk...........",
    "........ktcbbbbbbssstk..........",
    ".......ktccbbbbbbbssstk.........",
    "......ktcccbbbbbbbbssstk........",
    ".....ktccccbbbbbbbbbsssstk......",
    "....ktccccbbbbbbbbbbbssssstk....",
    "....kkkkkkkkkkkkkkkkkkkkkkkk....",
])

SPRITE_ARVORE_ALQUIMIA = (PALETA_ALQUIMIA, [
    "...........kkkkkkkkk............",
    ".........kkxhhhhhhllkk..........",
    ".......kkxxhhhhhhhlllkk.........",
    "......kxxhhhhhhhhlllllkk........",
    ".....kxxhhhhhhxhhhlllllmk.......",
    "....kxxhhhhhxxxxhhlllllmmk......",
    "...kxxhhhhhxxllxxhllllmmmmk.....",
    "..kxhhhhhhhxllhhxhllmmmmmmmk....",
    "..kxhhhhhhhxhhhhhhllmmmdmmmk....",
    ".kxhhhhhxhhhhhhhlllllmmddmmmk...",
    ".kxhhhxxxxhhhhhlllllmmddddmmk...",
    ".khhhxxllxxhhhllllllmmdddddmk...",
    ".khhhxllllxxhhlllllllmmddddmk...",
    ".khhhhhlllxhhhlllllllmmddddmk...",
    ".khhhhhlllhhhhllllllmmmmdddmk...",
    "..khhhhlllhhhhlllllmmmmmdddmk...",
    "..kxhhhhhxhhhhllllmmmmmddddmk...",
    "..kxxhhhhxhhhllllmmmmmmddddk....",
    "...kxxhhxxhhllllmmmmmdddddk.....",
    "...kxxhhxxhlllmmmmmmdddddk......",
    "....kkkhhhllllmmmmmdddddk.......",
    "......kkhlllllmmmmdddddk........",
    "........kklkktbbstkddk..........",
    "..........kkktbbstkk............",
    ".............tbbst..............",
    ".............tbbst..............",
    ".............tbbst..............",
    ".............tbbst..............",
    "............ktbbstk.............",
    "...........ktcbbbstk............",
    "..........ktcbbbbssstk..........",
    "..........kkkkkkkkkkkk..........",
])

def main():
    SAIDA.mkdir(parents=True, exist_ok=True)
    
    print("Gerando elementos 16x16...")
    for nome, (paleta_hex, mapa) in SPRITES_16x16.items():
        paleta = {k: hex_rgba(v) for k, v in paleta_hex.items()}
        desenhar(mapa, paleta, SAIDA / f"{nome}.png")
        print(f" -> {nome}.png (16x16 e 48x48)")
        
    print("Gerando grandes arvores...")
    for nome, (paleta_hex, mapa) in [
        ("pinheiro", SPRITE_PINHEIRO),
        ("arvore_media", SPRITE_ARVORE_MEDIA),
        ("arvore_grande", SPRITE_ARVORE_GRANDE),
        ("arvore_alquimia", SPRITE_ARVORE_ALQUIMIA),
    ]:
        paleta = {k: hex_rgba(v) for k, v in paleta_hex.items()}
        desenhar(mapa, paleta, SAIDA / f"{nome}.png")
        h, w = len(mapa), len(mapa[0])
        print(f" -> {nome}.png ({w}x{h} e {w*3}x{h*3})")
        
    print(f"Todos os assets de cenario gerados em {SAIDA}")

if __name__ == "__main__":
    main()
