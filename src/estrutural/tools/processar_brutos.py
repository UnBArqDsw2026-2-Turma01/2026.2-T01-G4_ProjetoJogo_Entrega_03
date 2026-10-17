"""Transforma as imagens geradas por IA (assets/_bruto/) nos assets do jogo.

As IAs de imagem não entregam pixel art de verdade: o fundo vem em magenta
(ou branco) com ruído de JPEG, cada "pixel" do desenho ocupa dezenas de pixels
reais e os quadros de uma animação não ficam em células iguais. Este script:

1. recorta o fundo (chroma key tolerante ao ruído);
2. separa os quadros de cada folha pelos espaços vazios entre eles;
3. espelha os quadros que vieram virados para o lado errado;
4. reduz tudo para o tamanho final, com a mesma escala em todas as animações
   de um inimigo e os pés (ou o centro, para quem voa) sempre no mesmo lugar;
5. limita a paleta, para limpar o ruído que sobra da redução;
6. salva cada quadro no caminho que o jogo espera.

Os brutos ficam fora do repositório; só o resultado é versionado.

Uso (de dentro de src/estrutural/):  python3 tools/processar_brutos.py [--previa saida.png]
Requer Pillow (pip install pillow).
"""
import argparse
from pathlib import Path

from PIL import Image, ImageChops, ImageDraw

RAIZ = Path(__file__).resolve().parent.parent
BRUTO = RAIZ / "assets" / "_bruto"
COMBATE = RAIZ / "assets" / "combate"
MUNDO = RAIZ / "assets" / "mundo"

# Por inimigo: célula de cada quadro (px), como alinhar os quadros e, por
# animação, quantos quadros a folha tem e se ela veio virada para a direita.
INIMIGOS = {
    "golem_de_enxofre": {
        "celula": (32, 32),
        "modo": "chao",
        "animacoes": {"idle": (4, True), "ataque": (4, True), "dano": (2, False), "derrota": (4, False)},
    },
    "cogumelo_andante": {
        "celula": (24, 24),
        "modo": "chao",
        "animacoes": {"idle": (4, False), "ataque": (4, False), "dano": (4, False), "derrota": (4, False)},
    },
    "morcego_da_caverna": {
        "celula": (24, 24),
        "modo": "voo",
        "animacoes": {"idle": (4, False), "ataque": (4, False), "dano": (2, False), "derrota": (4, False)},
    },
}

# Imagens inteiras, sem fundo a recortar: (bruto, destino, tamanho, cores, gerar _3x).
CENARIOS = [
    ("fundo_floresta", COMBATE / "fundos" / "fundo_floresta.png", (320, 180), 48, False),
    ("fundo_caverna", COMBATE / "fundos" / "fundo_caverna.png", (320, 180), 48, False),
    ("mapa_caverna", MUNDO / "caverna" / "mapa_caverna.png", (640, 384), 48, True),
]

CORES_SPRITE = 16


# --- Recorte do fundo ---

def abrir(nome):
    arquivos = sorted(BRUTO.glob(nome + ".*"))
    if not arquivos:
        raise SystemExit(f"Bruto não encontrado: {BRUTO / nome}.*")
    return Image.open(arquivos[0]).convert("RGB")


def alfa_magenta(img):
    """Quanto mais magenta (R e B altos, G baixo), mais transparente."""
    r, g, b = img.split()
    magenta = ImageChops.subtract(ImageChops.darker(r, b), g)
    # magenta <= 60: opaco; >= 140: transparente; entre os dois, gradual.
    return magenta.point(lambda v: 255 if v <= 60 else 0 if v >= 140 else int(255 * (140 - v) / 80))


def alfa_branco(img):
    """Para o bruto que veio com fundo branco: só o quase-branco vira transparente."""
    claro = ImageChops.darker(ImageChops.darker(*img.split()[:2]), img.split()[2])
    return claro.point(lambda v: 0 if v >= 235 else 255 if v <= 215 else int(255 * (235 - v) / 20))


# --- Medidas na imagem bruta ---

def pixels(img):
    # getdata() foi descontinuado no Pillow 13; o substituto não existe nas versões antigas.
    return img.get_flattened_data() if hasattr(img, "get_flattened_data") else img.getdata()


def perfil(mascara, eixo):
    """Quantos pixels opacos há em cada coluna (eixo 0) ou linha (eixo 1)."""
    w, h = mascara.size
    if eixo == 0:
        linha = mascara.resize((w, 1), Image.BOX)
        return [v * h / 255 for v in pixels(linha)]
    coluna = mascara.resize((1, h), Image.BOX)
    return [v * w / 255 for v in pixels(coluna)]


def trechos(valores, minimo):
    """Intervalos [inicio, fim) onde o perfil passa do mínimo."""
    saida, inicio = [], None
    for i, v in enumerate(valores):
        if v >= minimo and inicio is None:
            inicio = i
        elif v < minimo and inicio is not None:
            saida.append([inicio, i])
            inicio = None
    if inicio is not None:
        saida.append([inicio, len(valores)])
    return saida


def separar_quadros(mascara, esperados):
    """Colunas de cada quadro da folha.

    A IA não respeita células iguais, mas deixa os quadros mais ou menos
    espaçados. Cada trecho opaco (corpo, partícula, escombro) vai para o quadro
    cujo centro está mais perto, começando pelos centros de células iguais e
    refinando pela massa de cada trecho (k-médias em uma dimensão)."""
    colunas = perfil(mascara, 0)
    partes = trechos(colunas, 3)
    massas = [sum(colunas[a:b]) for a, b in partes]
    limite = max(massas) * 0.003  # descarta ruído do JPEG
    partes = [(a, b, m, sum(i * colunas[i] for i in range(a, b)) / m)
            for (a, b), m in zip(partes, massas) if m >= limite]
    centros = [(i + 0.5) * mascara.width / esperados for i in range(esperados)]
    for _ in range(20):
        grupos = [[] for _ in centros]
        for parte in partes:
            grupos[min(range(esperados), key=lambda k: abs(parte[3] - centros[k]))].append(parte)
        novos = [sum(p[2] * p[3] for p in g) / sum(p[2] for p in g) if g else c for g, c in zip(grupos, centros)]
        if novos == centros:
            break
        centros = novos
    if any(not g for g in grupos):
        # Quadros encostados ou faltando: cai para células iguais.
        largura = mascara.width / esperados
        return [[int(i * largura), int((i + 1) * largura)] for i in range(esperados)]
    return [[min(p[0] for p in g), max(p[1] for p in g)] for g in grupos]


def medir_quadro(mascara, x0, x1, modo):
    """Caixa do quadro e a coluna de ancoragem (pés, ou centro de massa se voa)."""
    faixa = mascara.crop((x0, 0, x1, mascara.height))
    linhas = trechos(perfil(faixa, 1), 2)
    topo, base = linhas[0][0], linhas[-1][1]
    if modo == "chao":
        pes = faixa.crop((0, base - max(1, (base - topo) // 5), x1 - x0, base))
        pesos = perfil(pes, 0)
    else:
        pesos = perfil(faixa.crop((0, topo, x1 - x0, base)), 0)
    total = sum(pesos) or 1
    ancora = sum(i * p for i, p in enumerate(pesos)) / total
    return {"x0": x0, "x1": x1, "topo": topo, "base": base, "ancora": ancora}


# --- Redução ---

def reduzir(rgb, alfa, tamanho):
    """Redução por média com alfa pré-multiplicado (a borda magenta não vaza para a cor)."""
    premult = ImageChops.multiply(rgb, Image.merge("RGB", (alfa, alfa, alfa)))
    premult = premult.resize(tamanho, Image.BOX)
    alfa = alfa.resize(tamanho, Image.BOX)
    saida = Image.new("RGBA", tamanho, (0, 0, 0, 0))
    px_cor, px_alfa, px_saida = premult.load(), alfa.load(), saida.load()
    for y in range(tamanho[1]):
        for x in range(tamanho[0]):
            a = px_alfa[x, y]
            if a >= 128:
                r, g, b = px_cor[x, y]
                px_saida[x, y] = (min(255, r * 255 // a), min(255, g * 255 // a), min(255, b * 255 // a), 255)
    return saida


def limitar_paleta(quadros, cores):
    """Uma paleta só para todos os quadros da folha, calculada nos pixels opacos."""
    opacos = [p for q in quadros for p in pixels(q) if p[3] == 255]
    if not opacos:
        return quadros
    amostra = Image.new("RGB", (len(opacos), 1))
    amostra.putdata([p[:3] for p in opacos])
    paleta = amostra.quantize(colors=cores, method=Image.Quantize.FASTOCTREE)
    saida = []
    for q in quadros:
        cor = q.convert("RGB").quantize(palette=paleta, dither=Image.Dither.NONE).convert("RGB")
        r, g, b = cor.split()
        saida.append(Image.merge("RGBA", (r, g, b, q.getchannel("A"))))
    return saida


def salvar(img, caminho):
    caminho.parent.mkdir(parents=True, exist_ok=True)
    img.save(caminho)
    return caminho


# --- Inimigos ---

def processar_inimigo(id_inimigo, cfg):
    largura, altura = cfg["celula"]
    modo = cfg["modo"]
    folhas = {}
    for animacao, (esperados, espelhar) in cfg["animacoes"].items():
        img = abrir(f"{id_inimigo}_{animacao}")
        alfa = alfa_magenta(img)
        quadros = [medir_quadro(alfa, a, b, modo) for a, b in separar_quadros(alfa, esperados)]
        if espelhar:
            for q in quadros:
                q["ancora"] = (q["x1"] - q["x0"]) - q["ancora"]
        corpo = max(q["base"] - q["topo"] for q in quadros)  # o quadro mais alto = corpo inteiro
        folhas[animacao] = {"img": img, "alfa": alfa, "quadros": quadros, "corpo": corpo, "espelhar": espelhar}

    # Medidas em "alturas de corpo", para comparar folhas desenhadas em escalas diferentes.
    esquerda = max(q["ancora"] / f["corpo"] for f in folhas.values() for q in f["quadros"])
    direita = max((q["x1"] - q["x0"] - q["ancora"]) / f["corpo"] for f in folhas.values() for q in f["quadros"])
    def extensao(f):
        return (max(q["base"] for q in f["quadros"]) - min(q["topo"] for q in f["quadros"])) / f["corpo"]
    vertical = max(extensao(f) for f in folhas.values())
    corpo_px = min(largura / (esquerda + direita), altura / vertical)
    ancora_x = largura * esquerda / (esquerda + direita)

    gerados = []
    for animacao, f in folhas.items():
        escala = corpo_px / f["corpo"]
        topo_folha = min(q["topo"] for q in f["quadros"])
        base_folha = max(q["base"] for q in f["quadros"])
        if modo == "chao":
            desloc_y = altura - (base_folha - topo_folha) * escala
        else:
            desloc_y = (altura - (base_folha - topo_folha) * escala) / 2
        quadros = []
        for q in f["quadros"]:
            caixa = (q["x0"], q["topo"], q["x1"], q["base"])
            rgb, alfa = f["img"].crop(caixa), f["alfa"].crop(caixa)
            if f["espelhar"]:
                rgb = rgb.transpose(Image.FLIP_LEFT_RIGHT)
                alfa = alfa.transpose(Image.FLIP_LEFT_RIGHT)
            tamanho = (max(1, round(rgb.width * escala)), max(1, round(rgb.height * escala)))
            sprite = reduzir(rgb, alfa, tamanho)
            celula = Image.new("RGBA", (largura, altura), (0, 0, 0, 0))
            x = round(ancora_x - q["ancora"] * escala)
            y = round(desloc_y + (q["topo"] - topo_folha) * escala)
            celula.alpha_composite(sprite, (x, y))
            quadros.append(celula)
        for i, q in enumerate(limitar_paleta(quadros, CORES_SPRITE), start=1):
            gerados.append(salvar(q, COMBATE / "inimigos" / id_inimigo / f"{animacao}_l_frame_{i}.png"))
    print(f"{id_inimigo}: corpo com {corpo_px:.1f}px de altura, {len(gerados)} quadros")
    return gerados


# --- Efeito do Sopro do Dragão ---

def processar_sopro():
    img = abrir("sopro_dragao")
    alfa = alfa_magenta(img)
    largura, altura = 96, 48
    partes = separar_quadros(alfa, 8)
    # A chama sai da esquerda: todo quadro começa na borda esquerda da célula,
    # com a mesma escala e a altura que tinha na folha.
    linhas = trechos(perfil(alfa, 1), 2)
    topo, base = linhas[0][0], linhas[-1][1]
    escala = min(largura / max(b - a for a, b in partes), altura / (base - topo))
    desloc_y = (altura - (base - topo) * escala) / 2
    quadros = []
    for a, b in partes:
        rgb, alfa_quadro = img.crop((a, topo, b, base)), alfa.crop((a, topo, b, base))
        sprite = reduzir(rgb, alfa_quadro, (round(rgb.width * escala), round(rgb.height * escala)))
        celula = Image.new("RGBA", (largura, altura), (0, 0, 0, 0))
        celula.alpha_composite(sprite, (0, round(desloc_y)))
        quadros.append(celula)
    gerados = [salvar(q, COMBATE / "vfx" / "sopro_dragao" / f"frame_{i}.png")
            for i, q in enumerate(limitar_paleta(quadros, CORES_SPRITE), start=1)]
    print(f"sopro_dragao: {len(gerados)} quadros")
    return gerados


# --- Objetos e cenários ---

def ampliar_3x(caminho):
    img = Image.open(caminho)
    destino = caminho.with_name(caminho.stem + "_3x.png")
    return salvar(img.resize((img.width * 3, img.height * 3), Image.NEAREST), destino)


def processar_entrada_caverna():
    img = abrir("entrada_caverna")
    alfa = alfa_branco(img)
    colunas = trechos(perfil(alfa, 0), 3)
    linhas = trechos(perfil(alfa, 1), 3)
    caixa = (colunas[0][0], linhas[0][0], colunas[-1][1], linhas[-1][1])
    rgb, alfa = img.crop(caixa), alfa.crop(caixa)
    largura, altura = 48, 32
    escala = min(largura / rgb.width, altura / rgb.height)
    sprite = reduzir(rgb, alfa, (round(rgb.width * escala), round(rgb.height * escala)))
    [sprite] = limitar_paleta([sprite], CORES_SPRITE)
    celula = Image.new("RGBA", (largura, altura), (0, 0, 0, 0))
    celula.alpha_composite(sprite, ((largura - sprite.width) // 2, altura - sprite.height))  # apoiada no chão
    destino = salvar(celula, MUNDO / "cenario" / "entrada_caverna.png")
    print("entrada_caverna: 48x32 e _3x")
    return [destino, ampliar_3x(destino)]


def processar_cenario(nome, destino, tamanho, cores, gerar_3x):
    img = abrir(nome)
    # Corta o excesso para a proporção final (centralizado) antes de reduzir.
    proporcao = tamanho[0] / tamanho[1]
    if img.width / img.height > proporcao:
        largura = round(img.height * proporcao)
        img = img.crop(((img.width - largura) // 2, 0, (img.width - largura) // 2 + largura, img.height))
    else:
        altura = round(img.width / proporcao)
        img = img.crop((0, (img.height - altura) // 2, img.width, (img.height - altura) // 2 + altura))
    img = img.resize(tamanho, Image.BOX)
    img = img.quantize(colors=cores, method=Image.Quantize.FASTOCTREE, dither=Image.Dither.NONE).convert("RGB")
    gerados = [salvar(img, destino)]
    if gerar_3x:
        gerados.append(ampliar_3x(destino))
    print(f"{nome}: {tamanho[0]}x{tamanho[1]}" + (" e _3x" if gerar_3x else ""))
    return gerados


# --- Prévia para conferência ---

def salvar_previa(arquivos, caminho, zoom=4):
    """Folha de contato com fundo xadrez, ampliada sem suavização."""
    imagens = [(f, Image.open(f).convert("RGBA")) for f in arquivos if "_3x" not in f.stem]
    largura = 1400
    linhas, linha, x, alt = [], [], 0, 0
    for f, img in imagens:
        w, h = img.width * zoom, img.height * zoom
        if x + w > largura and linha:
            linhas.append((linha, alt))
            linha, x, alt = [], 0, 0
        linha.append((f, img, x))
        x += w + 8
        alt = max(alt, h + 14)
    linhas.append((linha, alt))
    folha = Image.new("RGBA", (largura, sum(a for _, a in linhas) + 8), (255, 255, 255, 255))
    desenho = ImageDraw.Draw(folha)
    y = 4
    for linha, alt in linhas:
        for f, img, x in linha:
            grande = img.resize((img.width * zoom, img.height * zoom), Image.NEAREST)
            xadrez = Image.new("RGBA", grande.size, (200, 200, 200, 255))
            dx = ImageDraw.Draw(xadrez)
            for cy in range(0, grande.height, 8):
                for cx in range((cy // 8) % 2 * 8, grande.width, 16):
                    dx.rectangle((cx, cy, cx + 7, cy + 7), fill=(235, 235, 235, 255))
            xadrez.alpha_composite(grande)
            folha.alpha_composite(xadrez, (x, y + 12))
            desenho.text((x, y), f"{f.parent.name}/{f.stem}"[-28:], fill=(0, 0, 0, 255))
        y += alt
    folha.convert("RGB").save(caminho)
    print(f"Prévia salva em {caminho}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--previa", type=Path, help="salva uma folha de contato com tudo o que foi gerado")
    args = parser.parse_args()

    gerados = []
    for id_inimigo, cfg in INIMIGOS.items():
        gerados += processar_inimigo(id_inimigo, cfg)
    gerados += processar_sopro()
    gerados += processar_entrada_caverna()
    for cenario in CENARIOS:
        gerados += processar_cenario(*cenario)
    print(f"{len(gerados)} arquivos gerados.")
    if args.previa:
        salvar_previa(gerados, args.previa)


if __name__ == "__main__":
    main()
