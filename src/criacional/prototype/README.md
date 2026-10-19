# Prototype na criação de inimigos

**Responsável: Yogi Nam de Souza Barbosa.** Aplicação do GoF criacional Prototype em C++17. Um catálogo mantém inimigos configurados e produz objetos independentes pelo método virtual `clonar()`.

## Executar em C++

Com GCC/Clang com suporte a C++17 e Make, na raiz do repositório:

```bash
make -C src/criacional/prototype run
make -C src/criacional/prototype test
```

O menu permite escolher um modelo, criar inimigos, selecionar uma instância, aplicar dano, fortalecer seu ataque e clonar seu estado atual. `bin/prototype --roteiro` imprime oito estados em JSON usados na comparação entre a versão nativa e a versão web.

## Demonstração visual

Abra o [HTML publicado](../../../docs/Base/Relatórios/SubEquipe_01/prototype/index.html) no navegador. Funciona offline: o WebAssembly e a fonte estão incluídos nos arquivos publicados. A interface usa o mesmo núcleo C++ do terminal, compilado com Emscripten; o JavaScript envia comandos e desenha o estado retornado pelo módulo.

1. Clique duas vezes em **Criar do modelo**: ambas as cópias começam com vida 24 e ataque 5.
2. Selecione a cópia 1. Aplique **Aplicar 7 de dano** e **Ataque +2**. Compare seus valores 17 e 7 com o modelo e a cópia 2, que mantêm 24 e 5.
3. Clone o modelo novamente. A cópia 3 começa com os valores originais.
4. Selecione a cópia 1 e use **Clonar selecionado**. A cópia 4 herda vida 17 e ataque 7.
5. Cause dano na cópia 4: a cópia 1 mantém seu estado.

O limite de quatro instâncias mantém a cena legível. Os números identificam as instâncias na demonstração; não fazem parte do estado do inimigo.

Para abrir dentro do site, na raiz:

```bash
python3 -m http.server 8000 --directory docs
```

Acesse `http://localhost:8000/#/Base/Relatórios/SubEquipe_01/Prototype.md`.

## Organização

| Arquivo | Responsabilidade |
|---------|------------------|
| `src/inimigo.hpp` e `.cpp` | Contrato abstrato `Inimigo`, implementação `InimigoElemental` e objeto de valor `Ataque`. |
| `src/catalogo.hpp` e `.cpp` | Registro de protótipos e configuração dos três modelos. |
| `src/demonstracao.hpp` e `.cpp` | Estado da cena, comandos e leitura dos objetos. |
| `src/main.cpp` | Cliente de terminal e roteiro reproduzível. |
| `src/web.cpp` | Duas funções C exportadas para a interface web. |
| `web/` | HTML, CSS, apresentação JavaScript e assets locais. |
| `tests/testes.cpp` | 13 casos nativos: cópia, independência, polimorfismo, catálogo, validações e cena. |
| `tests/paridade.cjs` | Comparação de oito estados entre executável nativo e WebAssembly. |
| `tests/interface.cjs` | Testes de interface, captura e gravação da execução. |
| `publicar.py` | Publicação, comparação byte a byte e manifesto SHA-256 dos fontes C++ e módulo compilado. |

## Compilar para o navegador

A compilação publicada usa **Emscripten 4.0.23**. Instale o [SDK oficial](https://emscripten.org/docs/getting_started/downloads.html) e ative seu ambiente. Depois:

```bash
make -C src/criacional/prototype publish
make -C src/criacional/prototype check
node src/criacional/prototype/tests/paridade.cjs
```

Se o compilador estiver fora do PATH, use `make -C src/criacional/prototype publish EMXX=/caminho/emsdk/upstream/emscripten/em++`. A versão nativa não depende desse SDK. O alvo `publish` gera `bin/motor.js` com o WebAssembly embutido e publica junto com a interface em `docs/.../prototype/`. Edite os fontes nesta pasta e publique novamente. `check` exige o módulo compilado e verifica a correspondência dos arquivos e hashes.

## Testar a interface e gravar

Node.js e Playwright são ferramentas de verificação, não dependências do programa C++. Instalação isolada de desenvolvimento:

```bash
npm install --prefix /tmp/prototype-validacao playwright@1.62.1
PLAYWRIGHT_BROWSERS_PATH=/tmp/prototype-browsers /tmp/prototype-validacao/node_modules/.bin/playwright install chromium
PLAYWRIGHT_MODULE=/tmp/prototype-validacao/node_modules/playwright PLAYWRIGHT_BROWSERS_PATH=/tmp/prototype-browsers node src/criacional/prototype/tests/interface.cjs
```

Acrescente `--gravar` ao último comando para salvar o vídeo WebM em `/tmp/prototype-video-v2/`. O roteiro executa as operações acima, com pausas de leitura, e imprime o caminho do vídeo. Também atualiza a captura do relatório. Use `PROTOTYPE_URL` para verificar a demonstração servida por HTTP. A verificação inclui teclado, imagens, reinício, limite de cópias, estado derrotado e larguras de 360, 390, 768 e 1280 pixels.

## Decisões de cópia

- `clonar()` devolve `std::unique_ptr<Inimigo>` contendo uma nova instância do mesmo tipo concreto e com o estado atual, inclusive vida reduzida ou zero.
- `std::vector<Ataque>` e `std::string` são membros por valor. O construtor de cópia padrão copia seus valores para armazenamento próprio; não é preciso escrever um laço manual de cópia. Isso seria diferente com ponteiros para ataques mutáveis compartilhados.
- O catálogo clona o objeto recebido no registro. A consulta do modelo retorna `const Inimigo&`; a criação retorna outro objeto, sob responsabilidade do cliente.
- A destruição virtual e `unique_ptr` permitem liberar as instâncias pela interface base. Não há `new`/`delete` manual.
- Espécies são configurações de uma classe. Um protótipo adicional no teste verifica que o catálogo trabalha pelo contrato virtual, sem selecionar construtores concretos.

Essas decisões seguem a intenção do Prototype em Gamma et al. (2000), a discussão de clonagem em [Nystrom](https://gameprogrammingpatterns.com/prototype.html) e a recomendação [C.130 das C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c130-for-making-deep-copies-of-polymorphic-classes-prefer-a-virtual-clone-function-instead-of-public-copy-constructionassignment).

## Origem dos dados e das imagens

Nomes, PV (24, 36 e 18), força 5 do cogumelo e do morcego e Bafo Sulfuroso de dano 9 vêm de [`src/estrutural/data/inimigos.json`](../../estrutural/data/inimigos.json), revisão `9c4e632`. A fraqueza elétrica do gólem corresponde ao multiplicador 1,5 ali registrado. Fogo para o cogumelo, luz para o morcego e os nomes Investida e Mordida são escolhas didáticas. A fraqueza é exibida; o botão de dano aplica sete pontos para facilitar a comparação.

Cenários floresta/caverna e quatro quadros `idle_l_frame` de cada inimigo foram copiados sem edição de [`src/estrutural/assets/combate/`](../../estrutural/assets/combate/). A produção desses assets está registrada por Marcos Vinícius Gündel da Silva no [relato da SubEquipe 02](../../../docs/Base/Relatórios/SubEquipe_02/IAGenerativa.md). A fonte Pixelify Sans segue a tipografia do site; sua licença OFL acompanha `web/assets/`. A autoria desta contribuição refere-se ao Prototype, à interface e à documentação.

## Documentação

[Relatório com UML, referências, análise crítica, vídeo e rastreabilidade](../../../docs/Base/Relatórios/SubEquipe_01/Prototype.md).

| Versão | Data | Descrição | Autor |
|--------|------|-----------|-------|
| 1.0 | 19/10/2026 | Implementação inicial, demonstração visual e testes. | Yogi Nam de Souza Barbosa |
| 1.1 | 19/10/2026 | Núcleo C++17, execução nativa e WebAssembly, interface com assets do jogo e validação de paridade. | Yogi Nam de Souza Barbosa |
