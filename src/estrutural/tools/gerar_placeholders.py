"""Gera assets substitutos (placeholders) em pixel art de alta qualidade
para o mapa, jogador, galinhas e UI quando o pack TinyPixel não estiver instalado.

Dessa forma o jogo abre e roda imediatamente (F5) sem erros de recursos faltantes.
Se o integrante baixar o TinyPixel depois, basta rodar tools/importar_tinypixel.py
para substituir pelos assets oficiais.

Uso: python3 tools/gerar_placeholders.py
"""
import json
import math
import shutil
from pathlib import Path
from PIL import Image, ImageDraw

RAIZ = Path(__file__).resolve().parent.parent

def criar_diretorios():
    (RAIZ / "assets" / "ui").mkdir(parents=True, exist_ok=True)
    (RAIZ / "assets" / "mundo" / "jogador").mkdir(parents=True, exist_ok=True)
    (RAIZ / "assets" / "mundo" / "galinha").mkdir(parents=True, exist_ok=True)

# ---------------------------------------------------------------------------
# UI: board.png, button_a.png, button_b.png
# ---------------------------------------------------------------------------
def gerar_ui():
    ui_dir = RAIZ / "assets" / "ui"

    # board.png (96x96 com borda 9-slice)
    board = Image.new("RGBA", (96, 96), (0, 0, 0, 0))
    draw = ImageDraw.Draw(board)
    # Fundo pergaminho
    draw.rectangle([0, 0, 95, 95], fill="#2e1f18") # Contorno externo madeira escura
    draw.rectangle([3, 3, 92, 92], fill="#5c3e29") # Borda externa
    draw.rectangle([6, 6, 89, 89], fill="#8a6142") # Borda interna
    draw.rectangle([9, 9, 86, 86], fill="#e8d8be") # Pergaminho / madeira clara interior
    # Textura sutil
    for y in range(10, 86, 4):
        draw.line([(10, y), (85, y)], fill="#dfceb2")
    board.save(ui_dir / "board.png")

    # button_a.png (48x48 - estilo clássico de botão madeira/metal)
    btn_a = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    draw_a = ImageDraw.Draw(btn_a)
    draw_a.rectangle([0, 0, 47, 47], fill="#26170f")
    draw_a.rectangle([2, 2, 45, 45], fill="#6b4a31")
    draw_a.rectangle([4, 4, 43, 43], fill="#a07450")
    draw_a.rectangle([6, 6, 41, 41], fill="#c4976c")
    btn_a.save(ui_dir / "button_a.png")

    # button_b.png (48x48 - versão clara)
    btn_b = Image.new("RGBA", (48, 48), (0, 0, 0, 0))
    draw_b = ImageDraw.Draw(btn_b)
    draw_b.rectangle([0, 0, 47, 47], fill="#3b281c")
    draw_b.rectangle([2, 2, 45, 45], fill="#8f6b4a")
    draw_b.rectangle([4, 4, 43, 43], fill="#d6b88d")
    draw_b.rectangle([6, 6, 41, 41], fill="#edd3ad")
    btn_b.save(ui_dir / "button_b.png")

    licenca = ui_dir / "TINYPIXEL_LICENSES.txt"
    if not licenca.exists():
        licenca.write_text("Assets gerados automaticamente como placeholder para desenvolvimento.\n")

# ---------------------------------------------------------------------------
# Galinha: chicken_{l,r}_frame_{1..3}.png (48x48)
# ---------------------------------------------------------------------------
def gerar_galinha():
    galinha_dir = RAIZ / "assets" / "mundo" / "galinha"
    
    # 16x16 pixel art matriz
    # . = transp, w = branco, s = sombra, r = crista vermelha, b = bico amarelo, e = olho preto, p = pata laranja
    frames_dados = [
        # Frame 1: parado
        [
            "................",
            "......rrr.......",
            ".....rwww.......",
            ".....rwebe......",
            ".....wwwwb......",
            "....wwwwww......",
            "...wwwwwwww.....",
            "..swwwwwwwww....",
            "..swwwwwwwww....",
            "..swwwwwwww.....",
            "...swwwwww......",
            "....sswws.......",
            ".....p..p.......",
            ".....p..p.......",
            "....pp.pp.......",
            "................",
        ],
        # Frame 2: bico baixo
        [
            "................",
            "................",
            "......rrr.......",
            ".....rwww.......",
            ".....rwebe......",
            "....wwwwwwb.....",
            "...wwwwwwww.....",
            "..swwwwwwwww....",
            "..swwwwwwwww....",
            "..swwwwwwww.....",
            "...swwwwww......",
            "....sswws.......",
            "......p.p.......",
            ".....p..p.......",
            "....pp..pp......",
            "................",
        ],
        # Frame 3: passo
        [
            "................",
            "......rrr.......",
            ".....rwww.......",
            ".....rwebe......",
            ".....wwwwb......",
            "....wwwwww......",
            "...wwwwwwww.....",
            "..swwwwwwwww....",
            "..swwwwwwwww....",
            "..swwwwwwww.....",
            "...swwwwww......",
            "....sswws.......",
            "....p...p.......",
            "....p....p......",
            "...pp....pp.....",
            "................",
        ]
    ]

    paleta = {
        "r": (218, 44, 44, 255),
        "w": (245, 245, 245, 255),
        "s": (205, 205, 210, 255),
        "b": (245, 175, 25, 255),
        "e": (20, 20, 20, 255),
        "p": (230, 120, 20, 255),
    }

    for idx, grid in enumerate(frames_dados, start=1):
        img_16 = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
        for y, linha in enumerate(grid):
            for x, char in enumerate(linha):
                if char in paleta:
                    img_16.putpixel((x, y), paleta[char])
        
        # Escala 3x -> 48x48
        img_r = img_16.resize((48, 48), Image.NEAREST)
        img_l = img_r.transpose(Image.FLIP_LEFT_RIGHT)
        
        img_r.save(galinha_dir / f"chicken_r_frame_{idx}.png")
        img_l.save(galinha_dir / f"chicken_l_frame_{idx}.png")

# ---------------------------------------------------------------------------
# Jogador: {idle,walk}_{l,r}_frame_{1..8}.png (48x48)
# ---------------------------------------------------------------------------
def gerar_jogador():
    jogador_dir = RAIZ / "assets" / "mundo" / "jogador"

    # Alquimista com manto azul/índigo, chapéu pontudo e bolsa de poções
    # . = transp, h = chapéu azul, H = fivela chapéu dourada, s = pele rosto, e = olho,
    # r = manto azul escuro, c = manto azul claro, b = cinto marrom, B = fivela dourada,
    # v = frasco de poção verde, f = bota marrom
    alquimista_base = [
        ".......hh.......",
        "......hhhh......",
        ".....hhhhhh.....",
        ".....hHHHHh.....",
        "....hhhhhhhh....",
        "....ssseeess....",
        "....ssssssss....",
        "....rcsccsrr....",
        "...rccccccccr...",
        "...rcccbbccrr...",
        "...rcbBBBbbvr...",
        "...rcccccccvr...",
        "...rcccccccvr...",
        "....rcccccr.....",
        "....fffffff.....",
        "................",
    ]

    paleta = {
        "h": (35, 45, 120, 255),
        "H": (220, 180, 40, 255),
        "s": (240, 190, 150, 255),
        "e": (25, 25, 35, 255),
        "r": (25, 35, 95, 255),
        "c": (45, 65, 155, 255),
        "b": (80, 45, 25, 255),
        "B": (240, 200, 50, 255),
        "v": (50, 205, 50, 255),
        "f": (55, 30, 20, 255),
    }

    # Gera 8 frames de idle (leve respiração de 1px nos frames 4-6)
    for i in range(1, 9):
        img_16 = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
        shift_y = 1 if i in (4, 5, 6) else 0
        for y, linha in enumerate(alquimista_base):
            target_y = y + shift_y
            if target_y >= 16:
                continue
            for x, char in enumerate(linha):
                if char in paleta:
                    img_16.putpixel((x, target_y), paleta[char])
        
        img_r = img_16.resize((48, 48), Image.NEAREST)
        img_l = img_r.transpose(Image.FLIP_LEFT_RIGHT)
        img_r.save(jogador_dir / f"idle_r_frame_{i}.png")
        img_l.save(jogador_dir / f"idle_l_frame_{i}.png")

    # Gera 8 frames de walk (ciclo de caminhada alternando passos)
    for i in range(1, 9):
        img_16 = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
        # passo oscila pé esquerdo/direito
        fase = math.sin(i * math.pi / 4.0)
        offset_leg = int(round(fase * 1.5))
        for y, linha in enumerate(alquimista_base):
            for x, char in enumerate(linha):
                desloc_x = 0
                if char == "f" and y == 14:
                    desloc_x = offset_leg if x > 8 else -offset_leg
                target_x = max(0, min(15, x + desloc_x))
                if char in paleta:
                    img_16.putpixel((target_x, y), paleta[char])
        
        img_r = img_16.resize((48, 48), Image.NEAREST)
        img_l = img_r.transpose(Image.FLIP_LEFT_RIGHT)
        img_r.save(jogador_dir / f"walk_r_frame_{i}.png")
        img_l.save(jogador_dir / f"walk_l_frame_{i}.png")

# ---------------------------------------------------------------------------
# Mapa: mapa.png (3840x2400)
# ---------------------------------------------------------------------------
def gerar_mapa():
    mapa_png = RAIZ / "assets" / "mundo" / "mapa.png"
    dados_mapa = json.loads((RAIZ / "data" / "mapa.json").read_text(encoding="utf-8"))
    largura, altura = dados_mapa["tamanho"] # 3840, 2400
    
    print(f"Gerando chão do mapa {largura}x{altura}...")
    mapa_img = Image.new("RGBA", (largura, altura), (0, 0, 0, 255))
    draw = ImageDraw.Draw(mapa_img)

    # 1. Pinta grama base com variações de tiles 48x48
    cor_grama_a = (65, 120, 52, 255)
    cor_grama_b = (70, 128, 56, 255)
    cor_grama_c = (60, 112, 48, 255)
    
    t_tam = 48
    for ty in range(0, altura, t_tam):
        for tx in range(0, largura, t_tam):
            v = ((tx // t_tam * 13) + (ty // t_tam * 7)) % 10
            c = cor_grama_b if v > 6 else (cor_grama_c if v < 2 else cor_grama_a)
            draw.rectangle([tx, ty, tx + t_tam - 1, ty + t_tam - 1], fill=c)

    # 2. Pinta as áreas de colisão (lagos em azul e acampamento em terra)
    colisoes = dados_mapa.get("colisoes", [])
    for col in colisoes:
        x, y, w, h = col
        # Colisões no leste (x > 2500) correspondem ao grande lago
        if x > 2500:
            draw.rectangle([x - 12, y - 12, x + w + 12, y + h + 12], fill=(210, 180, 90, 255)) # Margem de areia
            draw.rectangle([x, y, x + w, y + h], fill=(45, 115, 175, 255)) # Água do lago
            draw.rectangle([x + 12, y + 12, x + w - 12, y + h - 12], fill=(35, 95, 150, 255)) # Água profunda
        else:
            # Acampamento / cabana no oeste
            draw.rectangle([x, y, x + w, y + h], fill=(110, 80, 55, 255)) # Solo de terra batida

    # 3. Caminho de terra central conectando início aos baús
    inicio_x, inicio_y = dados_mapa["inicio"]
    # Pátio inicial
    draw.ellipse([inicio_x - 120, inicio_y - 80, inicio_x + 120, inicio_y + 80], fill=(130, 95, 65, 255))
    draw.ellipse([inicio_x - 100, inicio_y - 65, inicio_x + 100, inicio_y + 65], fill=(155, 115, 80, 255))

    mapa_img.save(mapa_png, "PNG")
    print("mapa.png salvo com sucesso.")

    # bau_fechado.png em mundo se faltar
    bau_mundo = RAIZ / "assets" / "mundo" / "bau_fechado.png"
    bau_icone = RAIZ / "assets" / "icones" / "bau_fechado_48x48.png"
    if bau_icone.exists() and not bau_mundo.exists():
        shutil.copy(bau_icone, bau_mundo)

def main():
    print("Criando diretórios de assets...")
    criar_diretorios()
    print("Gerando assets da UI...")
    gerar_ui()
    print("Gerando sprites das Galinhas...")
    gerar_galinha()
    print("Gerando sprites do Jogador...")
    gerar_jogador()
    gerar_mapa()
    print("Concluído! Todos os placeholders foram gerados com sucesso.")

if __name__ == "__main__":
    main()
