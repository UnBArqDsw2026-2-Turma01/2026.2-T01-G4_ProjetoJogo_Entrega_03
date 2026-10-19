# Prototype na criação de inimigos

## Descrição

Um modelo de inimigo pode dar origem a várias instâncias com os mesmos atributos iniciais. Depois da criação, cada uma precisa ter seu próprio estado: ferir um cogumelo ou alterar seu ataque deve afetar somente aquele inimigo.

Este artefato aplica o **GoF criacional Prototype** a esse problema do **G4_ProjetoJogo**. A contribuição de **Yogi Nam de Souza Barbosa**, no FOCO_01, reúne modelo UML, implementação, demonstração interativa, vídeo e testes.

<p><a href="Base/Relatórios/SubEquipe_01/prototype/index.html" target="_blank" rel="noopener"><strong>Abrir a demonstração interativa ↗</strong></a> · <a href="Base/Relatórios/SubEquipe_01/assets/prototype/gravacao-prototype.webm" target="_blank" rel="noopener">Assistir ao vídeo</a></p>

![Demonstração: a cópia 1 tem vida 17 e ataque 7; o modelo e as cópias 2 e 3 mantêm vida 24 e ataque 5.](assets/prototype/demonstracao.png)

<p align="center">Figura 1: Comparação entre o modelo e três instâncias durante a execução. Fonte: Yogi Nam de Souza Barbosa, 2026. Sprites reutilizados do projeto, conforme a origem registrada no README do código.</p>

## Objetivo

Criar inimigos por clonagem de objetos configurados, centralizar os modelos em um catálogo e comprovar que as cópias preservam os atributos e possuem estado independente. O recorte permite observar o padrão diretamente, usando os inimigos do jogo.

## Metodologia

O trabalho foi organizado como uma contribuição individual do FOCO_01. Primeiro, foram consultados o Mapa Mental da Entrega 01 e o Diagrama de Classes e a lista de requisitos adotada na Entrega 02. Em seguida, o problema de criação de inimigos foi comparado com os padrões criacionais, considerando o Builder de Gabriel e o Singleton escolhido por Cibelly.

A implementação usa JavaScript puro para executar o mesmo código no navegador, no GitPages e nos testes. O modelo foi descrito em UML, com fonte PlantUML e representação SVG. A verificação combina testes do domínio com operações automatizadas no navegador, que também produzem a captura e a gravação desta página. O código-fonte é copiado para a documentação por um script que verifica a igualdade dos arquivos.

O apoio do Codex na análise, implementação, documentação e verificação está descrito no [relato individual de IA Generativa](IAGenerativa.md?id=yogi-nam-de-souza-barbosa). A separação em commits de implementação e documentação permite acompanhar os artefatos produzidos.

## Conteúdo

### Padrão Escolhido

**Prototype, categoria criacional.** Sua intenção é produzir novos objetos a partir de uma instância usada como modelo. O método de clonagem preserva o tipo e o estado do objeto escolhido (GAMMA et al., 2000; NYSTROM, 2014).

No exemplo de jogos discutido por Nystrom, o criador de inimigos delega a clonagem a um monstro já configurado. Essa organização orienta o catálogo desta implementação. O autor também chama atenção para a decisão entre cópia superficial e profunda, que aqui é verificada por testes e pela alteração visual dos ataques. [Referência: capítulo Prototype](https://gameprogrammingpatterns.com/prototype.html).

| Alternativa | Relação com o problema |
|------------|-----------------------|
| **Prototype** | Reaproveita o estado de um inimigo pronto, incluindo sua lista de ataques. É o foco desta implementação. |
| Builder | Monta um personagem passo a passo. Complementa a clonagem, pois um objeto montado poderia depois ser usado como modelo. |
| Factory Method | Delega a subclasses a escolha do produto criado. Neste recorte, as espécies diferem por dados, sem exigir uma hierarquia de criadores. |
| Abstract Factory | Organiza famílias de produtos relacionados. A criação aqui envolve um único tipo de entidade. |
| Singleton | Controla a existência de uma única instância. Os inimigos precisam de múltiplas instâncias independentes. |

<p align="center">Tabela 1: Comparação das alternativas para este recorte. Fonte: Yogi Nam de Souza Barbosa, 2026, a partir da classificação de Gamma et al. (2000).</p>

### Contexto (Problema)

O domínio do jogo já contém inimigos, estatísticas e fraquezas elementares. Ao criar novas instâncias espalhando construtores e configurações pelo cliente, torna-se fácil repetir atributos de forma inconsistente. Uma atribuição direta, por outro lado, apenas cria outra referência para o mesmo objeto.

A solução mantém os modelos em `CatalogoDeInimigos`. O cliente pede `criar("cogumelo")`; o catálogo localiza o modelo e chama `clonar()`. O construtor concreto fica no próprio protótipo. Novas configurações podem ser registradas usando o mesmo contrato.

Cogumelo, gólem e morcego usam `InimigoElemental`, com configurações diferentes. Isso mantém a hierarquia pequena. Seus nomes, PV e sprites vêm do jogo existente; os ajustes didáticos de ataques e fraquezas estão identificados no [README do código][codigo].

### Modelagem (Notação UML)

![UML: InimigoElemental herda de Inimigo, o catálogo mantém modelos privados e a interface solicita e manipula as cópias.](assets/prototype/classes.svg)

<p align="center">Figura 2: Participantes do Prototype e dependências do cliente. Fonte: Yogi Nam de Souza Barbosa, 2026. Notação de classes e relacionamentos conforme UML 2.5.1 (OMG, 2017).</p>

<p><a href="Base/Relatórios/SubEquipe_01/assets/prototype/classes.puml" download>Fonte PlantUML</a> · <a href="Base/Relatórios/SubEquipe_01/assets/prototype/classes.svg" target="_blank" rel="noopener">Ampliar SVG</a></p>

| Elemento | Papel e responsabilidade |
|----------|-------------------------|
| `Inimigo` | **Prototype**: declara o contrato `clonar()`. Em JavaScript, a implementação-base lança erro se o contrato não for implementado. |
| `InimigoElemental` | **ConcretePrototype**: conhece seu estado, devolve uma nova instância e copia os ataques. |
| `CatalogoDeInimigos` | **Registro de protótipos e cliente de `clonar()`**: mantém modelos privados e solicita cópias pelo contrato `Inimigo`. |
| `app.js` | **Cliente visual**: pede inimigos ao catálogo e permite inspecionar e alterar cada instância. |
| `Ataque` | Objeto de valor com nome e dano. A UML explicita sua composição, implementada como objetos JavaScript dentro de uma lista. |

<p align="center">Tabela 2: Correspondência entre a UML e o código. Fonte: Yogi Nam de Souza Barbosa, 2026.</p>

### Implementação (Código & Vídeo com Padrão em Execução)

#### Código

O código fica em [`src/criacional/prototype/`][codigo]. O núcleo está em [`modelo.js`][modelo], a configuração em [`catalogo.js`][catalogo] e a interface em [`app.js`][cliente].

O método principal é curto:

```javascript
clonar() {
  return new InimigoElemental(this.#estado);
}
```

O cuidado decisivo está no construtor. Além de copiar os atributos simples, ele cria uma lista nova e um objeto novo para cada ataque:

```javascript
this.#estado = {
  nome, sprite, fraqueza, vidaMaxima, vida,
  ataques: ataques.map(({ nome, dano }) => ({ nome, dano }))
};
```

Assim, `fortalecerAtaque(0, 2)` modifica somente a cópia escolhida. Copiar apenas a lista manteria os objetos internos compartilhados. A política de cópia é explícita para os campos deste modelo; se um ataque ganhar outros objetos internos, essa política e os testes precisam acompanhar a mudança.

O catálogo depende da operação de clonagem, sem escolher o construtor concreto:

```javascript
criar(chave) {
  const prototipo = this.#modelos.get(chave);
  if (!prototipo) throw new Error(`Modelo desconhecido: ${chave}`);
  return prototipo.clonar();
}
```

O registro guarda sua própria cópia do modelo recebido. Já `dados()` fornece à interface uma fotografia independente do estado interno. Essas duas decisões protegem o catálogo de alterações feitas por quem registra ou exibe um inimigo.

#### Demonstração interativa

<iframe src="Base/Relatórios/SubEquipe_01/prototype/index.html" title="Prototype: experimento de clonagem de inimigos" width="100%" height="900" style="border:1px solid #53624a;border-radius:10px;" loading="lazy"></iframe>

<p align="center">Demonstração 1: Código do Prototype em execução no navegador. Fonte: Yogi Nam de Souza Barbosa, 2026. <a href="Base/Relatórios/SubEquipe_01/prototype/index.html" target="_blank" rel="noopener">Abrir em uma janela maior</a>.</p>

O roteiro abaixo permite explicar o padrão em cerca de um minuto:

1. **Clonar duas vezes:** surgem objetos distintos com vida 24 e ataque 5.
2. **Alterar a cópia 1:** aplique sete de dano e aumente o ataque em dois. Compare vida 17 e ataque 7 com os valores preservados no modelo e na cópia 2.
3. **Clonar o modelo novamente:** a cópia 3 começa com vida 24 e ataque 5, comprovando que o catálogo permaneceu intacto.
4. **Clonar a cópia alterada:** a nova instância herda vida 17 e ataque 7. Ao receber outro dano, ela também evolui de forma independente.

O último passo mostra uma distinção importante: **clonar copia o estado atual; criar pelo catálogo copia o modelo preservado**. Os rótulos numéricos são atribuídos pela interface para facilitar a comparação.

#### Vídeo

<video controls preload="metadata" poster="Base/Relatórios/SubEquipe_01/assets/prototype/demonstracao.png" style="width:100%;border-radius:8px;" src="Base/Relatórios/SubEquipe_01/assets/prototype/gravacao-prototype.webm"></video>

<p align="center">Vídeo 1: Execução real da demonstração, com cliques automatizados por Playwright, comparação das cópias e troca de modelos. Fonte: Yogi Nam de Souza Barbosa, 2026. <a href="Base/Relatórios/SubEquipe_01/assets/prototype/gravacao-prototype.webm" target="_blank" rel="noopener">Abrir ou baixar o vídeo WebM</a>.</p>

#### Execução e verificação

Para executar offline, abra `src/criacional/prototype/index.html`. Para verificar o modelo, use Node.js 20 ou superior na raiz do repositório:

```bash
node src/criacional/prototype/tests/modelo.test.cjs
node src/criacional/prototype/publicar.mjs --check
```

**Resultado: 12 testes aprovados.** A [saída da execução](Base/Relatórios/SubEquipe_01/assets/prototype/testes.txt ':ignore') acompanha o artefato. O [roteiro de testes de interface][testes-ui] reproduz as interações do vídeo e verifica quatro larguras de tela: 360, 390, 768 e 1440 pixels.

| Verificação | Evidência observada |
|-------------|---------------------|
| Identidade, tipo e atributos | A cópia é outro objeto, da mesma classe, com os mesmos dados e operações. |
| Independência de vida e ataques | Alterar uma cópia preserva o modelo e as demais, inclusive o segundo ataque usado no teste. |
| Cópia do estado atual | Instâncias feridas ou derrotadas mantêm esse estado ao serem clonadas. |
| Proteção do catálogo | Alterar o objeto registrado ou os dados de leitura preserva os modelos internos. |
| Uso do contrato | Outro tipo de inimigo, definido no teste, funciona no mesmo catálogo. |
| Limites e entradas | Vida para em zero; operações inválidas são recusadas preservando o estado. |
| Interface | Clonagem, seis instâncias, reinício, teclado, carregamento de imagens e ausência de rolagem horizontal nas larguras verificadas. |

<p align="center">Tabela 3: Critérios de verificação do artefato. Fonte: testes automatizados do modelo e da interface, 19/10/2026.</p>

### Análise Crítica

O benefício é concentrar a criação em um objeto que conhece o próprio estado. O experimento com ataques aninhados torna verificável uma condição que a simples aparência de duas fichas iguais não comprova.

O custo também deve ser considerado: cada clonagem copia os ataques e consome memória própria. Com poucos modelos fixos, uma função de criação poderia ser suficiente. A escolha do Prototype aqui favorece o reaproveitamento de configurações e a clonagem de instâncias já modificadas, mantendo o contrato de criação pequeno. Essa avaliação acompanha a discussão crítica de Nystrom sobre o uso do padrão em jogos.

A implementação representa o ciclo de criação e alteração de inimigos. Os controles de dano fixo e fortalecimento servem para inspecionar a independência das cópias. O catálogo é uma extensão de apoio; o mecanismo essencial do padrão continua sendo a chamada polimórfica a `clonar()`.

Também é importante distinguir o **GoF Prototype** da propriedade `.prototype` do JavaScript: o primeiro organiza a criação por cópia de objetos; a segunda participa do mecanismo de delegação da linguagem.

### Rastreabilidade

| Origem | Elo concreto com o artefato |
|--------|----------------------------|
| [Mapa Mental da Entrega 01][mapa] | Ramos Personagens e Mecânicas: inimigos, combate em turnos e elementos químicos situam o problema no jogo. |
| [Diagrama de Classes da Entrega 02][classes-e2] | `Inimigo`, atributos de combate e fraqueza elemental orientam os dados copiados. A hierarquia Prototype é uma decisão de implementação desta entrega. |
| [Requisitos do Subgrupo 03 adotados na Entrega 02][requisitos] | **RF13:** fraquezas elementares. **RF17:** estatísticas dos inimigos. A clonagem preserva esses dados; os códigos se referem a essa lista específica. |
| [Dados de inimigos da Entrega 03][dados-jogo] | Nomes, PV e atributos selecionados fundamentam os três modelos da demonstração. |
| [Ata Geral 04](/Atas/AtaGeral04.md) | Organização da entrega em padrões criacionais, estruturais e comportamentais. |
| [GoF Criacional](GoFCriacional.md) e [Participações](/Base/1.2.ParticipacoesPadroesDeProjeto.md) | Localizam esta contribuição no FOCO_01, ao lado do Builder. |
| [Commit de implementação 53c28e5](https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/commit/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6) | Reúne o modelo, o catálogo, a demonstração, os testes e o script de publicação, com autoria de Yogi. |
| [IA Generativa](IAGenerativa.md?id=yogi-nam-de-souza-barbosa) | Registra a assistência utilizada, as decisões técnicas e os critérios de conferência. |

<p align="center">Tabela 4: Rastreabilidade do Prototype. Fonte: artefatos vinculados, organizados por Yogi Nam de Souza Barbosa, 2026.</p>

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000. Padrões criacionais, Prototype.

> GRUPO 04. **Mapa Mental, Diagrama de Classes e requisitos do Subgrupo 03**. Entregas 01 e 02, 2026. Artefatos vinculados na Tabela 4.

> NODE.JS. **Test runner**. Disponível em: https://nodejs.org/api/test.html. Acesso em: 19 out. 2026.

> NYSTROM, Robert. **Game Programming Patterns**. 2014. Capítulo Prototype. Disponível em: https://gameprogrammingpatterns.com/prototype.html. Acesso em: 19 out. 2026.

> OBJECT MANAGEMENT GROUP. **Unified Modeling Language, version 2.5.1**. 2017. Disponível em: https://www.omg.org/spec/UML/2.5.1/About-UML. Acesso em: 19 out. 2026.

## Nível de Contribuição dos Integrantes

| Nome | Contribuição neste artefato |
|------|----------------------------|
| Yogi Nam de Souza Barbosa | 100%: responsabilidade pela contribuição Prototype, código, UML, demonstração, testes, vídeo e documentação. Uso de IA descrito na metodologia e no relato individual. |

<p align="center">Tabela 5: Participação no artefato Prototype.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|-----------|-----------|---------|
| 1.0 | 19/10/2026 | Aplicação do Prototype à criação de inimigos, UML, demonstração interativa, testes, vídeo, análise crítica e rastreabilidade. | Yogi Nam de Souza Barbosa | — |

<p align="center">Tabela 6: Histórico de versão.</p>

[codigo]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/tree/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6/src/criacional/prototype
[modelo]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6/src/criacional/prototype/modelo.js
[catalogo]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6/src/criacional/prototype/catalogo.js
[cliente]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6/src/criacional/prototype/app.js
[testes-ui]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/53c28e5b8d4c68e9f3a5c85f25d2c4d5f01054e6/src/criacional/prototype/tests/interface.cjs
[mapa]: https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_01/#/Base/Relatórios/SubEquipe_03/MapaMental.md
[classes-e2]: https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_02/#/Base/Relatórios/SubEquipe_01/DiagramaClasses.md
[requisitos]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_02/blob/3c2f767ea022f938663d67236e7256513e2c8012/docs/Base/Relatórios/SubEquipe_01/assets/referencias/requisitos-subgrupo03.txt
[dados-jogo]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/9c4e632/src/estrutural/data/inimigos.json
