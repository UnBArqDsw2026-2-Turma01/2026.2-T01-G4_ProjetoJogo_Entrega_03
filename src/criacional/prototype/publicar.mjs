// Copia o mesmo código testado para o GitPages. Não há uma segunda implementação.
import { cp, mkdir, readFile, readdir } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import { join } from 'node:path';

const origem = fileURLToPath(new URL('./', import.meta.url));
const destino = fileURLToPath(new URL('../../../docs/Base/Relatórios/SubEquipe_01/prototype/', import.meta.url));
const arquivos = ['index.html', 'style.css', 'modelo.js', 'catalogo.js', 'app.js',
  ...(await readdir(join(origem, 'assets'))).map(nome => `assets/${nome}`)];
const conferir = process.argv.includes('--check');
for (const arquivo of arquivos) {
  if (conferir) {
    const [a, b] = await Promise.all([readFile(join(origem, arquivo)), readFile(join(destino, arquivo))]);
    if (!a.equals(b)) throw new Error(`Publicação desatualizada: ${arquivo}`);
  } else {
    await mkdir(join(destino, 'assets'), { recursive: true });
    await cp(join(origem, arquivo), join(destino, arquivo));
  }
}
console.log(conferir ? 'Publicação idêntica ao código-fonte.' : 'Demonstração copiada para docs.');
