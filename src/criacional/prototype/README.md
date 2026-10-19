# Prototype na criação de inimigos

**Responsável: Yogi Nam de Souza Barbosa.** Demonstração do GoF criacional Prototype no G4_ProjetoJogo. Um catálogo mantém inimigos configurados e cria instâncias independentes chamando `clonar()`.

## Executar

Abra [`index.html`](index.html) no navegador. Funciona offline, com os arquivos desta pasta juntos. Também há uma cópia publicada em [`docs/Base/Relatórios/SubEquipe_01/prototype/`](../../../docs/Base/Relatórios/SubEquipe_01/prototype/index.html).

Para servir o site inteiro, na raiz do repositório:

```bash
python3 -m http.server 8000 --directory docs
```

Abra `http://localhost:8000/#/Base/Relatórios/SubEquipe_01/Prototype.md` para ler o relatório ou `http://localhost:8000/Base/Relatórios/SubEquipe_01/prototype/index.html` para a demonstração.

## Experimento de um minuto

1. Clique duas vezes em **Clonar modelo**. As duas cópias começam com vida 24 e ataque 5.
2. Na cópia 1, clique em **−7 de vida** e **+2 no ataque**. Ela fica com vida 17 e ataque 7. Confira o modelo e a cópia 2: ambos permanecem com vida 24 e ataque 5.
3. Clone o modelo novamente. A cópia 3 nasce com os dados originais.
4. Clique em **Clonar esta cópia** na cópia 1. A cópia 4 nasce com vida 17 e ataque 7: Prototype copia o estado atual do objeto escolhido.
5. Cause dano na cópia 4 e confira que a cópia 1 permanece intacta.

O limite de seis instâncias mantém a comparação legível. Os identificadores são rótulos da interface; o estado do inimigo é copiado integralmente.

## Organização

| Arquivo | Responsabilidade |
|---------|------------------|
| [`modelo.js`](modelo.js) | `Inimigo` (Prototype), `InimigoElemental` (ConcretePrototype) e `CatalogoDeInimigos` (registro e cliente de `clonar`). |
| [`catalogo.js`](catalogo.js) | Configuração inicial dos três modelos. |
| [`app.js`](app.js) | Cliente visual: pede cópias, aplica operações e exibe os dados. |
| [`index.html`](index.html), [`style.css`](style.css) | Interface responsiva, com controles nativos de teclado e anúncios de estado. |
| [`tests/modelo.test.cjs`](tests/modelo.test.cjs) | Testes de isolamento, cópia do estado, polimorfismo e limites. |
| [`tests/interface.cjs`](tests/interface.cjs) | Execução no navegador, captura e gravação opcional. |
| [`publicar.mjs`](publicar.mjs) | Copia os arquivos para o GitPages e confere igualdade byte a byte. |

JavaScript puro, sem dependências para executar a demonstração. Scripts clássicos permitem abrir o HTML diretamente pelo sistema de arquivos. O mesmo modelo é carregado pelos testes do Node.js.

## Testar

Com Node.js 20 ou superior, a partir da raiz:

```bash
node src/criacional/prototype/tests/modelo.test.cjs
node src/criacional/prototype/publicar.mjs --check
```

O primeiro comando executa 12 testes usando `node:test` e retorna erro se uma asserção falhar. O segundo verifica se a versão do site corresponde ao código-fonte.

Para testar a interface e gravar, instale a dependência de desenvolvimento em uma pasta temporária:

```bash
npm install --prefix /tmp/prototype-validacao playwright@1.62.1
PLAYWRIGHT_BROWSERS_PATH=/tmp/prototype-browsers /tmp/prototype-validacao/node_modules/.bin/playwright install chromium
PLAYWRIGHT_MODULE=/tmp/prototype-validacao/node_modules/playwright PLAYWRIGHT_BROWSERS_PATH=/tmp/prototype-browsers node src/criacional/prototype/tests/interface.cjs
```

Acrescente `--gravar` ao último comando para obter uma gravação WebM em `/tmp/prototype-video`. O roteiro usa cliques reais na interface, com pausas para leitura; também salva uma captura no relatório. Para testar por HTTP, defina `PROTOTYPE_URL` com a URL da demonstração. O WebM pode ser reproduzido diretamente no navegador. Para uma conversão opcional para MP4, use uma instalação do FFmpeg com o encoder libx264:

```bash
ffmpeg -i /tmp/prototype-video/ARQUIVO.webm -c:v libx264 -crf 24 -pix_fmt yuv420p -movflags +faststart gravacao-prototype.mp4
```

## Atualizar a versão publicada

Edite os arquivos desta pasta e execute, na raiz:

```bash
node src/criacional/prototype/publicar.mjs
node src/criacional/prototype/publicar.mjs --check
```

Os arquivos em `docs/.../prototype/` são cópias de publicação. A implementação e os testes ficam nesta pasta.

## Decisões de cópia

- `clonar()` devolve uma nova instância do mesmo tipo e preserva o estado atual, inclusive a vida de um inimigo ferido ou derrotado.
- O construtor copia a lista de ataques **e cada objeto da lista**. A política cobre os campos do modelo: números, strings e uma lista de objetos com nome e dano.
- `dados()` também devolve uma cópia, para que alterações feitas pela interface não alcancem o estado interno.
- O catálogo guarda uma cópia do objeto registrado e devolve novas cópias a cada criação. Alterar o objeto usado no registro não modifica o catálogo.
- As espécies são configurações da mesma classe. Uma nova classe só se justifica quando houver comportamento diferente. O teste com `OutroInimigo` demonstra que o catálogo aceita outro protótipo sem escolher seu construtor.

## Origem dos dados e das imagens

Os nomes, os PV (24, 36 e 18), a força 5 do cogumelo e do morcego e o Bafo Sulfuroso de dano 9 vêm de [`src/estrutural/data/inimigos.json`](../../estrutural/data/inimigos.json), na revisão `9c4e632` da Entrega 03. A fraqueza elétrica do gólem corresponde ao multiplicador 1,5 registrado ali. As fraquezas a fogo do cogumelo e a luz do morcego, assim como os nomes Investida e Mordida, são escolhas didáticas desta demonstração. A fraqueza é exibida como atributo; o botão de dano sempre aplica sete pontos para facilitar a comparação.

Os três PNGs são cópias, sem edição, de `idle_l_frame_1.png` dos diretórios `cogumelo_andante`, `golem_de_enxofre` e `morcego_da_caverna` em [`src/estrutural/assets/combate/inimigos/`](../../estrutural/assets/combate/inimigos/). A procedência da produção desses assets está documentada por Marcos Vinícius Gündel da Silva no [relato de IA da SubEquipe 02](../../../docs/Base/Relatórios/SubEquipe_02/IAGenerativa.md). A reutilização mantém a identidade visual do projeto; a autoria desta contribuição se refere ao Prototype, à interface e à documentação.

## Documentação

[Relatório com UML, análise, referências, vídeo e rastreabilidade](../../../docs/Base/Relatórios/SubEquipe_01/Prototype.md).

| Versão | Data | Descrição | Autor |
|--------|------|-----------|-------|
| 1.0 | 19/10/2026 | Implementação do Prototype, demonstração visual, testes e instruções de reprodução. | Yogi Nam de Souza Barbosa |
