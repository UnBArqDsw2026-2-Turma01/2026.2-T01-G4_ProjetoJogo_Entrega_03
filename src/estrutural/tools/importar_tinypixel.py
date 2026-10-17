"""Copia para o projeto os assets do TinyPixel e gera o mapa.

A licença do TinyPixel Starter Edition proíbe redistribuir os arquivos do pack,
por isso eles não ficam no repositório. Cada integrante baixa o pack em
https://onelevelstudio.itch.io/tinypixel e roda (de dentro de src/estrutural/):

    py tools/importar_tinypixel.py "<pasta TinyPixel_StarterEdition_v1.0>"
"""
import shutil
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent


def main(origem_pack):
    origem = Path(origem_pack)
    temp_dir = None

    if origem.is_file() and (origem.suffix.lower() == ".zip" or zipfile.is_zipfile(origem)):
        print(f"Descompactando arquivo zip '{origem}'...")
        temp_dir = tempfile.TemporaryDirectory()
        with zipfile.ZipFile(origem, "r") as z:
            z.extractall(temp_dir.name)
        # Procura a pasta que contém "Assets - For all game engines and tools"
        candidatos = list(Path(temp_dir.name).rglob("Assets - For all game engines and tools"))
        if not candidatos:
            sys.exit(f"Erro: 'Assets - For all game engines and tools' não encontrado no zip {origem}")
        assets = candidatos[0]
        pack = assets.parent
    else:
        pack = origem
        assets = pack / "Assets - For all game engines and tools"
        if not assets.is_dir():
            # Tenta busca recursiva na pasta caso o caminho informado seja o diretório pai
            candidatos = list(pack.rglob("Assets - For all game engines and tools"))
            if candidatos:
                assets = candidatos[0]
                pack = assets.parent
            else:
                sys.exit(f"Pasta não encontrada: {assets}")

    ui = RAIZ / "assets" / "ui"
    jogador = RAIZ / "assets" / "mundo" / "jogador"
    galinha = RAIZ / "assets" / "mundo" / "galinha"
    for pasta in (ui, jogador, galinha):
        pasta.mkdir(parents=True, exist_ok=True)

    for nome in ("board.png", "button_a.png", "button_b.png"):
        shutil.copy(assets / "UI" / nome, ui / nome)
    shutil.copy(pack / "LICENSES.txt", ui / "TINYPIXEL_LICENSES.txt")

    for estado in ("idle", "walk"):
        for quadro in (assets / "Actor1" / f"{estado}_split").glob("*.png"):
            shutil.copy(quadro, jogador / f"{estado}_{quadro.name}")
    for quadro in (assets / "Chicken").glob("*.png"):
        shutil.copy(quadro, galinha / quadro.name)

    subprocess.run([sys.executable, str(RAIZ / "tools" / "gerar_mapa.py"), str(assets)], check=True)
    print("Assets do TinyPixel importados.")


if __name__ == "__main__":
    main(sys.argv[1])
