# SubEquipe_03 — IA Generativa

## Descrição

Registro dos **participantes no FOCO_03: GoFs Comportamentais & IA Generativa**. Reúne os usos de IA generativa pela SubEquipe_03 na Entrega 03, os erros da ferramenta identificados pela equipe e os pontos de vista de cada integrante sobre as lições aprendidas e o uso da IA.

## Objetivo

Registrar, com senso crítico, o que cada membro aprendeu sobre padrões de projeto GoF Comportamentais e como utilizou IA Generativa no trabalho. **TODOS DEVEM PARTICIPAR.**

## Metodologia

As saídas da IA foram conferidas pela equipe antes de qualquer uso. Os pontos de vista dos integrantes foram redigidos a partir das transcrições das reuniões ([Ata 01](/Atas/AtaSub03_01.md) e [Ata 02](/Atas/AtaSub03_02.md)) e revisados por cada um. Nesta entrega, a subequipe usou duas ferramentas:

- **Claude Code (Anthropic, modelo Claude Opus 5.5).** Usado por Renan Pereira Reis no VS Code para aplicar nos diagramas as correções que a equipe combinava, gerar o código e apoiar a redação da documentação. Cada proposta da ferramenta foi apresentada à equipe e só seguiu depois de aprovada.
- **Gemini 3.1 Pro (Google).** Usado por Pedro Teixeira Moriel Sanchez para verificar a consistência do diagrama de classes corrigido e para resumir as transcrições automáticas do Google Meet nas atas.

## Conteúdo

### Usos da IA Generativa na Entrega

| Momento | Ferramenta | Uso | Verificação da equipe |
|---------|------------|-----|-----------------------|
| Revisão dos diagramas (17/10) | Claude Code | Aplicação das correções combinadas na reunião e geração da versão 1.3 dos diagramas da Entrega 02. | Revisão da equipe no Draw.io, que trocou a notação de classe abstrata pelo exemplo da professora. |
| Verificação do modelo (18/10) | Gemini | Verificação de consistência do diagrama de classes corrigido. | O retorno foi discutido na reunião e não levou a mudanças. |
| Modelagem do padrão (18/10) | Claude Code | Geração do diagrama de classes do padrão e da máquina de estados a partir do desenho discutido pela equipe. | Revisão e aprovação da equipe antes de qualquer código. |
| Código e testes (18/10) | Claude Code | Implementação em C++17, demonstração com roteiro fixo e testes automáticos. | Compilação sem avisos, 37 verificações e revisão do código na reunião. |
| Atas (18/10) | Gemini | Resumo das transcrições automáticas das reuniões. | Revisão de Pedro e conferência dos dados nas transcrições. |
| Documentação (18/10) | Claude Code | Apoio na redação dos artefatos desta entrega e exportação das figuras. | Revisão e ajustes da equipe. |

<p align="center">Tabela 1: Usos da IA generativa na Entrega 03. Fonte: SOUZA, Carlos; SANCHEZ, Pedro; REIS, Renan (2026).</p>

### Erros Identificados e Correções

| Erro da IA | Como a equipe percebeu e corrigiu |
|------------|-----------------------------------|
| O Claude Code sugeriu citar o combate implementado pela SubEquipe 02 como exemplo do problema que o State resolve. | A equipe recusou: as modelagens das subequipes são distintas, e misturá-las confundiria o modelo. A mesma conclusão aparece na [Ata 02](/Atas/AtaSub03_02.md). |
| O Claude Code marcou as classes abstratas com a notação entre chaves. | Carlos e Pedro trocaram a marcação por «abstract», seguindo o exemplo usado pela professora. |
| Os resumos do Gemini nas atas tiveram imprecisões, como os horários de término e a origem de uma decisão, atribuída à verificação do Gemini quando, na transcrição, ela veio de uma dúvida de Renan. | Cada ponto foi conferido na transcrição e corrigido na ata. |

<p align="center">Tabela 2: Erros da IA identificados pela equipe. Fonte: SOUZA, Carlos; SANCHEZ, Pedro; REIS, Renan (2026).</p>

### Participantes no Foco_03

| Nome do Membro | Lições Aprendidas | Uso da IA Generativa (senso crítico) |
|----------------|-------------------|--------------------------------------|
| Carlos Henrique Brasil de Souza | Os padrões GoF são muitos e profundos, e a UML não deixa a classe abstrata implícita nas setas de herança. | A saída da IA precisou ser conferida com a documentação da UML e com o material da professora. |
| Pedro Teixeira Moriel Sanchez | As setas da UML têm significado, e um padrão só se justifica quando resolve um problema real do modelo. | A verificação do Gemini ajudou, mas exigiu conhecimento do modelo para ser avaliada; os resumos das atas precisaram de revisão. |
| Renan Pereira Reis | Evoluir o modelo a partir do feedback rende mais do que refazê-lo, e o State troca as condicionais do turno por objetos de estado. | A IA acelerou as tarefas manuais, mas errou ao generalizar; cada proposta foi aprovada pela equipe antes de seguir. |

<p align="center">Tabela 3: Síntese dos pontos de vista dos participantes no Foco_03. Fonte: SOUZA, Carlos; SANCHEZ, Pedro; REIS, Renan (2026).</p>

### Carlos Henrique Brasil de Souza

#### Lições Aprendidas

Percebi que os padrões GoF são muitos, 23 ao todo, e que cada um tem bastante profundidade: vimos alguns com mais detalhe em aula, e aplicar um deles exige entender bem o problema do jogo. Para o nosso caso, o State foi o mais natural, porque um jogo com inimigos e turnos já é uma máquina de estados. Também aprendi que a UML não deixa a classe abstrata implícita: tínhamos achado que as setas de herança bastavam, mas é preciso marcar a classe, como no exemplo da professora com aspas francesas.

#### Uso da IA Generativa (senso crítico)

Minha relação com a IA nesta entrega foi revisar o que ela gerou. Conferi os diagramas com a documentação da UML e com o material da professora, e foi assim que, junto com o Pedro, troquei a marcação das classes abstratas e ajustei a legenda. Fiquei em dúvida sobre a notação das operações herdadas que a IA propôs; mantivemos porque ela é da UML e está explicada na legenda, mas isso mostra que a resposta da IA não dispensa a checagem de quem conhece o conteúdo.

### Pedro Teixeira Moriel Sanchez

#### Lições Aprendidas

Aprendi que as setas da UML têm significado. Na Entrega 02, ligamos as enumerações por herança sem querer, por costume de diagramas visuais sem semântica, e a professora apontou o erro. Também vi na prática que um padrão só se justifica quando resolve um problema do modelo: o State encaixou porque o jogo já é uma máquina de estados, enquanto o Memento resolveria apenas o salvamento. E reforcei a ideia de que vale mais um padrão bem feito do que vários.

#### Uso da IA Generativa (senso crítico)

Usei o Gemini para verificar a consistência do diagrama de classes corrigido e compartilhei o retorno com a equipe. Algumas observações eram genéricas e, discutidas na reunião, não levaram a mudanças: a IA ajuda a revisar, mas a resposta precisa ser avaliada por quem conhece o modelo. Também usei o Gemini do Google Meet para resumir as transcrições nas atas. O resumo poupou tempo, mas teve imprecisões, como os horários de término, que conferimos na transcrição e corrigimos.

### Renan Pereira Reis

#### Lições Aprendidas

Aprendi que evoluir um modelo a partir do feedback rende mais do que refazê-lo: corrigindo os diagramas da Entrega 02, a equipe passou a usar a semântica da UML com mais cuidado, como a diferença entre herança e dependência e a marcação das classes abstratas, e ganhou uma base pronta para o padrão. Também vi como o State organiza o combate: as condicionais sobre de quem é a vez dão lugar a objetos que representam cada momento da luta, e o mesmo pedido passa a ter efeitos diferentes conforme o estado. Por fim, ficou clara a diferença entre herdar um método e redefini-lo, que apareceu quando o Carlos e o Pedro propuseram mostrar nas subclasses as operações herdadas.

#### Uso da IA Generativa (senso crítico)

Usei o Claude Code (Anthropic), no VS Code, para aplicar nos diagramas as correções que a equipe combinava nas reuniões, gerar o código e os testes e apoiar a redação da documentação. A maior vantagem foi a velocidade: tarefas manuais, como redesenhar os diagramas, ficaram com a ferramenta, e a equipe pôde se concentrar nas decisões. A limitação é que a IA também erra quando generaliza: ela chegou a sugerir que usássemos o combate da SubEquipe 02 como exemplo do nosso problema, o que recusamos, porque as modelagens das subequipes são distintas. Por isso, cada proposta da ferramenta foi apresentada à equipe e só seguiu depois de aprovada.

## Referências

> ANTHROPIC. **Claude Code**. [S. l.]: Anthropic, 2026. Disponível em: <https://claude.com/claude-code>. Acesso em: 18 out. 2026.

> GOOGLE. **Gemini**. Versão 3.1 Pro. [S. l.]: Google, 2026. Disponível em: <https://gemini.google.com>. Acesso em: 18 out. 2026.

## Nível de Contribuição dos Integrantes

| Nome | % de Contribuição |
|------|-------------------|
| Carlos Henrique Brasil de Souza | 33% |
| Pedro Teixeira Moriel Sanchez | 33% |
| Renan Pereira Reis | 33% |

<p align="center">Tabela 4: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | 18/10/2026 | Registro dos usos de IA generativa na Entrega 03, dos erros identificados e dos pontos de vista dos integrantes, a partir das transcrições das reuniões | Carlos Henrique Brasil de Souza, Pedro Teixeira Moriel Sanchez, Renan Pereira Reis | |

<p align="center">Tabela 5: Histórico de versão.</p>

Ver também: [GoF Comportamental](GoFComportamental.md)
