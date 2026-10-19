"""Publica o módulo C++ compilado e a interface, conferindo os arquivos."""
from pathlib import Path
import hashlib
import json
import shutil
import sys

origem = Path(__file__).resolve().parent
destino = origem.parents[2] / 'docs/Base/Relatórios/SubEquipe_01/prototype'
arquivos = {p.relative_to(origem / 'web'): p for p in (origem / 'web').rglob('*') if p.is_file()}
arquivos[Path('motor.js')] = origem / 'bin/motor.js'

fontes = sorted((origem / 'src').glob('*.cpp')) + sorted((origem / 'src').glob('*.hpp'))
manifesto = {str(p.relative_to(origem)): hashlib.sha256(p.read_bytes()).hexdigest() for p in fontes}
manifesto['motor.js'] = hashlib.sha256((origem / 'bin/motor.js').read_bytes()).hexdigest()

if '--check' in sys.argv:
    if json.loads((destino / 'compilacao.json').read_text()) != manifesto:
        raise SystemExit('Fontes ou módulo alterados: recompile e publique novamente.')
    for relativo, fonte in arquivos.items():
        if fonte.read_bytes() != (destino / relativo).read_bytes():
            raise SystemExit(f'Publicação diferente do arquivo {relativo}')
    print('Publicação confere com os fontes e o módulo C++ compilado.')
else:
    for relativo, fonte in arquivos.items():
        alvo = destino / relativo
        alvo.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(fonte, alvo)
    (destino / 'compilacao.json').write_text(json.dumps(manifesto, indent=2) + '\n')
    print('C++/WebAssembly e interface publicados em docs.')
