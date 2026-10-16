"""Gera o mapa do mundo de teste a partir dos autotiles do TinyPixel.

Uso (de dentro de src/estrutural/):
    py tools/gerar_mapa.py "<pasta 'Assets - For all game engines and tools' do TinyPixel>"

Saídas:
    assets/mundo/mapa.png  -> chão inteiro já montado (uma imagem)
    data/mapa.json         -> tamanho, ponto de partida, colisões, baús e regiões de coleta

Os autotiles seguem o formato A2 do RPG Maker (96x144): cada tile de 48 px é
montado com 4 quartos de 24 px escolhidos pelos vizinhos da célula.
"""
import json
import math
import random
import sys
from pathlib import Path

from PIL import Image

RAIZ = Path(__file__).resolve().parent.parent
T = 48  # tamanho do tile
M = 24  # tamanho do quarto de tile
LARGURA, ALTURA = 80, 50

random.seed(2026)


# --- Autotile A2 ---------------------------------------------------------

def quarto(folha, coluna, linha, base_y=48):
    """Recorta o quarto (coluna, linha) da caixa 96x96 do autotile."""
    x, y = coluna * M, base_y + linha * M
    return folha.crop((x, y, x + M, y + M))


def montar_tile(folha, mesmo):
    """mesmo(dx, dy) -> True se o vizinho pertence à mesma camada."""
    tile = Image.new("RGBA", (T, T), (0, 0, 0, 0))
    for qx in (0, 1):
        for qy in (0, 1):
            sx = -1 if qx == 0 else 1
            sy = -1 if qy == 0 else 1
            h, v, d = mesmo(sx, 0), mesmo(0, sy), mesmo(sx, sy)
            if h and v and d:  # miolo
                pedaco = quarto(folha, 2 - qx, 2 - qy)
            elif h and v:  # canto interno
                pedaco = folha.crop((T + qx * M, qy * M, T + qx * M + M, qy * M + M))
            elif h:  # borda de cima/baixo
                pedaco = quarto(folha, 2 - qx, qy * 3)
            elif v:  # borda esquerda/direita
                pedaco = quarto(folha, qx * 3, 2 - qy)
            else:  # canto externo
                pedaco = quarto(folha, qx * 3, qy * 3)
            tile.paste(pedaco, (qx * M, qy * M))
    return tile


def pintar(imagem, folha, celulas):
    for (x, y) in celulas:
        def mesmo(dx, dy, x=x, y=y):
            nx, ny = x + dx, y + dy
            if not (0 <= nx < LARGURA and 0 <= ny < ALTURA):
                return True
            return (nx, ny) in celulas
        tile = montar_tile(folha, mesmo)
        imagem.alpha_composite(tile, (x * T, y * T))


# --- Formas --------------------------------------------------------------

def elipse(cx, cy, rx, ry, ruido=0.0):
    celulas = set()
    for y in range(ALTURA):
        for x in range(LARGURA):
            r = ((x - cx) / rx) ** 2 + ((y - cy) / ry) ** 2
            if r <= 1.0 + random.uniform(-ruido, ruido):
                celulas.add((x, y))
    return celulas


def retangulo(x0, y0, x1, y1):
    return {(x, y) for x in range(x0, x1 + 1) for y in range(y0, y1 + 1)}


def borda(x0, y0, x1, y1):
    return {c for c in retangulo(x0, y0, x1, y1) if c[0] in (x0, x1) or c[1] in (y0, y1)}


def trilha(pontos, largura=3):
    """Rasteriza uma linha ligando os pontos com um pincel quadrado (sempre conexa)."""
    celulas = set()
    for (ax, ay), (bx, by) in zip(pontos, pontos[1:]):
        passos = max(abs(bx - ax), abs(by - ay)) * 2
        for i in range(passos + 1):
            t = i / passos
            px, py = round(ax + (bx - ax) * t), round(ay + (by - ay) * t)
            for dx in range(largura):
                for dy in range(largura):
                    x, y = px + dx - largura // 2, py + dy - largura // 2
                    if 0 <= x < LARGURA and 0 <= y < ALTURA:
                        celulas.add((x, y))
    return celulas


def suavizar(celulas, passos=2):
    """Autômato celular: arredonda as bordas serrilhadas."""
    for _ in range(passos):
        novo = set()
        for y in range(ALTURA):
            for x in range(LARGURA):
                vizinhos = sum((x + dx, y + dy) in celulas for dx in (-1, 0, 1) for dy in (-1, 0, 1))
                if vizinhos >= 5:
                    novo.add((x, y))
        celulas = novo
    return celulas


def manchas(quantidade, regiao, tamanho=(1.5, 3.0)):
    celulas = set()
    candidatas = sorted(regiao)
    for _ in range(quantidade):
        cx, cy = random.choice(candidatas)
        r = random.uniform(*tamanho)
        celulas |= {c for c in elipse(cx, cy, r, r * 0.8, 0.3) if c in regiao}
    return celulas


def retangulos_de(celulas):
    """Agrupa células sólidas em retângulos (corridas horizontais)."""
    rets = []
    for y in range(ALTURA):
        x = 0
        while x < LARGURA:
            if (x, y) in celulas:
                inicio = x
                while x < LARGURA and (x, y) in celulas:
                    x += 1
                rets.append([inicio * T, y * T, (x - inicio) * T, T])
            else:
                x += 1
    return rets


# --- Mapa ----------------------------------------------------------------

def main(pasta_tinypixel):
    a2 = Path(pasta_tinypixel) / "_GroupA2"
    a1 = Path(pasta_tinypixel) / "_GroupA1"
    folha = lambda caminho: Image.open(caminho).convert("RGBA")

    tudo = retangulo(0, 0, LARGURA - 1, ALTURA - 1)
    vila = (40, 25)

    floresta = suavizar(elipse(16, 12, 15, 10, 0.35))
    pedreira = suavizar(elipse(42, 43, 14, 6, 0.3))
    lago = suavizar(elipse(64, 16, 9, 6, 0.2))
    praia = suavizar(elipse(64, 16, 11.5, 8, 0.2)) - lago
    acampamento = retangulo(5, 34, 16, 42)
    cerca = borda(5, 34, 16, 42) - retangulo(16, 37, 16, 39)  # abertura a leste
    praca = elipse(vila[0], vila[1], 5, 3.5)

    caminhos = (
        trilha([vila, (30, 22), (22, 16), (16, 12)])           # floresta
        | trilha([vila, (50, 24), (54, 20), (54, 16)])        # lago (margem oeste)
        | trilha([vila, (41, 33), (42, 41)])                  # pedreira
        | trilha([vila, (30, 30), (22, 38), (17, 38)])        # acampamento
        | trilha([(54, 20), (62, 26), (74, 30), (77, 39)], 2)  # trilha estreita para o leste
    ) - lago

    livre = tudo - lago - cerca - caminhos - praca
    roxas = suavizar(manchas(6, floresta & livre, (1.5, 2.5)), 1)
    amarelas = suavizar(manchas(10, pedreira & livre, (1.5, 2.5)), 1)
    gramas = suavizar(manchas(30, (livre - floresta - pedreira - praia), (1.5, 2.8)), 1)

    imagem = Image.new("RGBA", (LARGURA * T, ALTURA * T))
    pintar(imagem, folha(a2 / "plain_1.png"), tudo)
    pintar(imagem, folha(a2 / "grass_2.png"), floresta)
    pintar(imagem, folha(a2 / "sand_5.png"), pedreira)
    pintar(imagem, folha(a2 / "grass_7.png"), roxas)
    pintar(imagem, folha(a2 / "grass_4.png"), amarelas)
    pintar(imagem, folha(a2 / "grass_2.png"), gramas)
    pintar(imagem, folha(a2 / "sand_4.png"), praia | lago)
    pintar(imagem, folha(a1 / "ocean_1_frame_1.png"), lago)
    pintar(imagem, folha(a2 / "land_1.png"), caminhos | praca)
    pintar(imagem, folha(a2 / "land_3.png"), acampamento - cerca)
    pintar(imagem, folha(a2 / "fence_1.png"), cerca)

    saida_img = RAIZ / "assets" / "mundo" / "mapa.png"
    imagem.save(saida_img)

    centro = lambda c: [c[0] * T + T // 2, c[1] * T + T // 2]
    regioes = [
        {"nome": "Floresta", "ingredientes": ["erva_lunar", "salvia_de_fogo", "cogumelo_roxo", "cogumelo_brilhante"], "quantidade": 14, "celulas": floresta},
        {"nome": "Pedreira", "ingredientes": ["cristal_de_quartzo", "pepita_de_cobre"], "quantidade": 8, "celulas": pedreira},
        {"nome": "Margem do lago", "ingredientes": ["escama_de_salamandra", "cristal_de_quartzo"], "quantidade": 5, "celulas": praia},
        {"nome": "Acampamento", "ingredientes": ["pena_de_corvo", "escama_de_salamandra"], "quantidade": 6, "celulas": elipse(11, 38, 9, 7)},
    ]
    proibidas = lago | cerca | caminhos | praca
    for regiao in regioes:
        validas = sorted(regiao.pop("celulas") - proibidas)
        regiao["pontos"] = [centro(c) for c in random.sample(validas, min(len(validas), regiao["quantidade"] * 3))]

    baus = [
        {"tipo": "antigo", "celula": (35, 21)},  # vila: perto do início
        {"tipo": "floresta", "celula": (46, 28)},  # vila
        {"tipo": "floresta", "celula": (9, 8)},
        {"tipo": "floresta", "celula": (24, 6)},
        {"tipo": "pedreira", "celula": (33, 45)},
        {"tipo": "pedreira", "celula": (52, 42)},
        {"tipo": "cacador", "celula": (8, 36)},
        {"tipo": "antigo", "celula": (77, 41)},
    ]
    for bau in baus:
        assert bau["celula"] not in proibidas, bau
        bau["posicao"] = centro(bau.pop("celula"))

    galinhas_area = [(vila[0] - 6) * T, (vila[1] - 5) * T, 12 * T, 10 * T]

    dados = {
        "tamanho": [LARGURA * T, ALTURA * T],
        "inicio": centro(vila),
        "colisoes": retangulos_de(lago | cerca),
        "baus": baus,
        "regioes": regioes,
        "area_galinhas": galinhas_area,
    }
    (RAIZ / "data" / "mapa.json").write_text(json.dumps(dados, ensure_ascii=False), encoding="utf-8")
    print(f"Mapa {LARGURA}x{ALTURA} gerado em {saida_img}")


if __name__ == "__main__":
    main(sys.argv[1])
