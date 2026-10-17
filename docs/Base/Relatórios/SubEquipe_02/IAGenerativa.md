# SubEquipe_02 — IA Generativa

## Descrição

Registro dos **participantes no FOCO_02: GoFs Estruturais & IA Generativa**. Reúne os pontos de vista de cada integrante da SubEquipe_02 sobre as lições aprendidas e o uso de IA Generativa na entrega.

> **Página em elaboração.** Substitua cada trecho marcado com *A preencher* e remova esta nota quando o artefato estiver concluído.

## Objetivo

Registrar, com senso crítico, o que cada membro aprendeu sobre padrões de projeto GoF Estruturais e como utilizou IA Generativa no trabalho. **TODOS DEVEM PARTICIPAR.**

## Metodologia

Cada integrante registra, em uma subseção própria, as lições aprendidas e uma análise crítica do uso da IA Generativa, acompanhada de evidências, como os históricos das conversas ou os resultados gerados pela ferramenta. As saídas da IA devem ser conferidas na literatura antes de qualquer uso, e a ferramenta não deve aparecer como autora nos históricos de versão.

*A preencher: em quais momentos da entrega a IA Generativa foi empregada pela subequipe.*

## Conteúdo

### Participantes no Foco_02

| Nome do Membro | Lições Aprendidas | Uso da IA Generativa (senso crítico) |
|----------------|-------------------|--------------------------------------|
| João Igor Pereira da Costa | *A preencher* | *A preencher* |
| João Victor da Silva Batista de Farias | Padrão Composite aplicado a interfaces em C++; separação entre hierarquia visual e lógica de negócio; integração nativa com GDExtension e Godot 4 | Uso do Antigravity (Gemini) com a skill `grill-me` para sabatinar decisões arquiteturais; apoio na refatoração da UI, resolução de compilação da GDExtension (.so) no Linux e scripts de assets |
| Marcelo de Araújo Lopes | Facade como interface única de um subsistema; fronteira C++/Godot com GDExtension; layout de diagramas UML legíveis | Claude e Gemini para entender o Facade, gerar assets, apoiar o C++, depurar e organizar a documentação; falhas no diagrama e na versão web corrigidas após testes |
| Marcos Vinícius Gündel da Silva | *A preencher* | *A preencher* |

<p align="center">Tabela 1: Síntese dos pontos de vista dos participantes no Foco_02. Fonte: Autores, 2026.</p>

### João Igor Pereira da Costa

#### Lições Aprendidas

*A preencher.*

#### Uso da IA Generativa (senso crítico)

*A preencher: ferramenta e modelo utilizados, finalidade, vantagens, limitações observadas e evidências, como o histórico da conversa.*

### João Victor da Silva Batista de Farias

#### Lições Aprendidas

A implementação do padrão Composite em C++ para a interface gráfica mostrou como tratar elementos individuais (botões, rótulos e barras de progresso) e agrupamentos complexos (painéis e menus) sob a mesma interface comum (`UIComponent`).

A principal lição prática foi delimitar com clareza as fronteiras entre os padrões da equipe: a `AlquimiaFacade` deve responder unicamente pelas regras e estado do subsistema de alquimia, enquanto a árvore de UI do Composite gerencia a composição visual, a renderização hierárquica e o repasse de comandos de entrada. Essa divisão evitou que a interface ficasse fortemente acoplada às regras de síntese das poções.

#### Uso da IA Generativa (senso crítico)

Utilizei o assistente Antigravity com o modelo Gemini. Para conduzir o planejamento arquitetural, ativei a skill `grill-me`. Essa skill funciona como uma sabatina técnica iterativa: em vez de gerar código diretamente, a IA assume o papel de entrevistadora e faz perguntas críticas sobre o design, apontando riscos, suposições frágeis e casos de borda antes de qualquer linha ser escrita.

Com essa abordagem, validei a estrutura de classes do Composite, a transferência de responsabilidades da `MochilaUI` para o modelo em árvore e a divisão de escopo entre os quatro integrantes da subequipe.

A ferramenta também auxiliou na depuração do ambiente Linux (construção da biblioteca compartilhada `.so` com SCons/godot-cpp, correção do `alquimia.gdextension`) e na criação de scripts utilitários em Python para importação e geração de placeholders visuais.

Como limitação, a IA sugeriu inicialmente configurações de extensão restritas a Windows e Web, além de tentar carregar assets ausentes sem tratamento de exceção, o que exigiu intervenção manual, análise dos logs do motor e testes headlessly no terminal para garantir a estabilidade do jogo.

### Marcelo de Araújo Lopes

#### Lições Aprendidas

A principal lição foi entender o Facade como uma decisão sobre **quem pode enxergar o quê**. No sistema de alquimia, uma única mistura passa por seis classes, e a interface da mochila não precisa conhecer nenhuma delas. Ao implementar em C++ com GDExtension, isso ficou concreto: só a `AlquimiaFacade` é registrada no Godot, e o subsistema simplesmente não existe para o resto do jogo. O padrão deixou de ser um desenho no slide e virou uma fronteira imposta pela própria ferramenta.

#### Uso da IA Generativa (senso crítico)

Usei duas ferramentas: o **Claude** (Anthropic), pelo Claude Code integrado ao VS Code, e o **Gemini** (Google). Os usos foram:

- **entender o padrão Facade**, discutindo como a mistura de ingredientes se encaixaria nos papéis de Client, Facade e Subsistema antes de modelar;
- **gerar os assets que faltavam no jogo**: o pacote TinyPixel não tinha ícones de ingredientes e poções nem baús. Os ícones (Figura 1) e o mapa (Figura 2) foram gerados por scripts escritos com o Claude, e o baú foi desenhado com o Gemini (Figura 3);
- **apoiar a implementação do código C++** com GDExtension, seguindo as decisões que eu tomei sobre as regras do jogo e a estrutura das classes;
- **depurar erros** de compilação, de exportação para a web e de exibição na página;
- **organizar a entrega e a documentação no GitHub Pages**: a estrutura dos arquivos `.md` no Docsify, as seções exigidas pelo modelo da disciplina, as tabelas, as legendas, o histórico de versão, a página de participações e a publicação do jogo e do vídeo na página do artefato.

![Ícones de ingredientes e poções gerados com o Claude](assets/subgrupo02_marcelo_icones_claude.png)

<p align="center">Figura 1: Os 8 ingredientes (linha de cima) e algumas das 36 poções (linha de baixo), em pixel art 16x16. Fonte: Claude (Anthropic), por meio de script gerado a pedido de Marcelo, 2026.</p>

![Mapa do mundo de teste montado com o Claude](assets/subgrupo02_marcelo_mapa_claude.jpg)

<p align="center">Figura 2: Mapa do mundo de teste, com vila, trilhas, floresta, lago, pedreira e acampamento, montado a partir dos autotiles do TinyPixel. Fonte: Claude (Anthropic), por meio de script gerado a pedido de Marcelo, 2026.</p>

![Baú fechado e aberto desenhados com o Gemini](assets/subgrupo02_marcelo_bau_gemini.png)

<p align="center">Figura 3: Baú fechado e aberto usados no jogo. Fonte: Gemini (Google), a partir de referência visual do pacote TinyPixel indicada por Marcelo, 2026.</p>

### Marcos Vinícius Gündel da Silva

#### Lições Aprendidas

*A preencher.*

#### Uso da IA Generativa (senso crítico)

*A preencher: ferramenta e modelo utilizados, finalidade, vantagens, limitações observadas e evidências, como o histórico da conversa.*

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000.

> REFACTORING.GURU. **Facade**. Disponível em: https://refactoring.guru/pt-br/design-patterns/facade. Acesso em: 16 out. 2026.

## Nível de Contribuição dos Integrantes

| Nome | % de Contribuição |
|------|-------------------|
| João Igor Pereira da Costa | *A preencher* |
| João Victor da Silva Batista de Farias | *A preencher* |
| Marcelo de Araújo Lopes | *A preencher* |
| Marcos Vinícius Gündel da Silva | *A preencher* |

<p align="center">Tabela 2: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | | | | |
| 1.1 | 16/10/2026 | Adição das lições aprendidas e do uso de IA Generativa de Marcelo de Araújo Lopes | [Marcelo de Araújo Lopes](https://github.com/MatielloAL) | |
| 1.2 | 16/10/2026 | Adição das lições aprendidas e do uso de IA Generativa de João Victor da Silva Batista de Farias | [João Victor da Silva Batista de Farias](https://github.com/beyondmagic) | |

<p align="center">Tabela 3: Histórico de versão.</p>
