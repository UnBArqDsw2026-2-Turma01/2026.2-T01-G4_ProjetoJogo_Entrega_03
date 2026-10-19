# SubEquipe_01 — IA Generativa

## Descrição

Registro dos **participantes no FOCO_01: GoFs Criacionais & IA Generativa**. Reúne os pontos de vista de cada integrante da SubEquipe_01 sobre as lições aprendidas e o uso de IA Generativa na entrega.

## Objetivo

Registrar, com senso crítico, o que cada membro aprendeu sobre padrões de projeto GoF Criacionais e como utilizou IA Generativa no trabalho. **TODOS DEVEM PARTICIPAR.**

## Metodologia

Cada integrante registra, em uma subseção própria, as lições aprendidas e uma análise crítica do uso da IA Generativa, acompanhada de evidências, como os históricos das conversas ou os resultados gerados pela ferramenta. As saídas da IA devem ser conferidas na literatura antes de qualquer uso, e a ferramenta não deve aparecer como autora nos históricos de versão.

Os relatos individuais descrevem o uso das ferramentas na documentação e implementação do Builder e do Prototype. No Prototype, a assistência abrangeu a leitura dos artefatos anteriores, a comparação dos padrões criacionais, o código, os diagramas, a interface e a execução automatizada dos testes.

## Conteúdo

### Participantes no Foco_01

| Nome do Membro | Lições Aprendidas | Uso da IA Generativa (senso crítico) |
|----------------|-------------------|--------------------------------------|
| Cibelly Lourenço Ferreira | *A preencher* | *A preencher* |
| Gabriel Andrade Magioli | Aprofundei conhecimentos em SDD (Spec driven development) um padrão que já venho acompanhando, usando a toolset (superpowers) onde consigo fazer o planejamento a partir do contexto do projeto e contexto fornecido, com isso podendo seguir para as etapas de documentação e código. Além disso, aprendi mais sobre os padrões de projeto de forma prática e a diagramação deles. O conhecimento não se limitou apenas ao Builder que escolhi, mas a outros padrões como prototype (cópias de instancias), Singleton(Instância única), etc. | IA usada para revisar a documentação e ajudar no planejamento e implementação do Builder; decisões de modelagem e o código validados pelo autor. |
| Yogi Nam de Souza Barbosa | Clonagem do estado atual, cópia por valor em C++ e relação entre modelo UML e implementação. | Codex (GPT-6) em análise, código, UML, testes e documentação. Questionamento da linguagem e da apresentação inicial, revisão com referências e registro das solicitações e dos resultados. |

<p align="center">Tabela 1: Síntese dos pontos de vista dos participantes no Foco_01. Fonte: Autores, 2026.</p>

### Cibelly Lourenço Ferreira

#### Lições Aprendidas

*A preencher.*

#### Uso da IA Generativa (senso crítico)

*A preencher: ferramenta e modelo utilizados, finalidade, vantagens, limitações observadas e evidências, como o histórico da conversa.*

### Gabriel Andrade Magioli

#### Lições Aprendidas

Aprofundei conhecimentos em SDD (Spec driven development) um padrão que já venho acompanhando, usando a toolset (superpowers) onde consigo fazer o planejamento a partir do contexto do projeto e contexto fornecido, com isso podendo seguir para as etapas de documentação e código. Além disso, aprendi mais sobre os padrões de projeto de forma prática e a diagramação deles. O conhecimento não se limitou apenas ao Builder que escolhi, mas a outros padrões como prototype (cópias de instancias), Singleton(Instância única), etc

#### Uso da IA Generativa (senso crítico)

Usei o **Claude Code**, em três momentos:

- **revisão da documentação**: correção de texto, organização das seções do [Builder](Builder.md) segundo o modelo da disciplina, legendas das figuras e tabelas e instruções de como rodar e testar o código;
- **apoio ao planejamento**: discussão de como transformar o diagrama de classes que modelei (Builder, CharacterBuilder, Character e AttackAction) em código C++, de como estruturar o programa interativo no terminal e de quais casos os testes deveriam cobrir.
- **apoio no desenvolvimento**: Fazendo uso de tecnicas como SDD, e agentes de code-review para garantir que padrões de código estavam sendo seguidos sem possuir bugs.

A modelagem UML e as decisões sobre o comportamento do programa, como escolher os atributos digitando 0 ou 1 e manter a interação no terminal, foram minhas. As sugestões da IA foram conferidas com a literatura (GAMMA et al., 2000) e com a execução do programa e dos testes antes de entrarem na entrega.

### Yogi Nam de Souza Barbosa

#### Lições Aprendidas

Nesta entrega, o ponto mais importante do [Prototype](Prototype.md) foi distinguir copiar a configuração inicial de copiar o estado atual. Um inimigo criado pelo catálogo começa com os valores do modelo. Já a cópia de um inimigo ferido deve continuar ferida. Isso ficou mais claro no experimento: a cópia 1 passa de 24 para 17 de vida; a clonagem dessa instância começa com 17, enquanto uma nova criação pelo catálogo continua com 24.

Outro aprendizado foi relacionar a ideia do padrão à forma de armazenar os dados em C++. Os ataques estão em `std::vector<Ataque>`, e cada ataque contém seus valores. Nesse caso, o construtor de cópia padrão já produz uma lista independente. A explicação precisa acompanhar a linguagem: repetir que seria necessário copiar manualmente cada ataque acrescentaria trabalho sem necessidade. Se os ataques fossem guardados por ponteiros para objetos mutáveis compartilhados, a conclusão seria diferente. A regra C.130 das C++ Core Guidelines ajudou a relacionar essa decisão à clonagem polimórfica e ao retorno pela classe base.

A comparação com o Builder também foi útil. O Builder do Gabriel monta um personagem por etapas; o Prototype reaproveita um objeto pronto. Cogumelo, gólem e morcego diferem por configuração neste exemplo, então criar uma subclasse para cada espécie deixaria o diagrama maior sem explicar melhor o comportamento. O catálogo trabalha pelo contrato de clonagem, e o teste com outro protótipo verifica essa possibilidade de extensão.

A revisão reforçou um cuidado que já apareceu nas entregas anteriores: o diagrama precisa corresponder ao que o código faz. No Builder, a relação entre o construtor e o produto passou a ser uma dependência de criação. A composição fica entre o personagem e os ataques que ele armazena. No Prototype, a sequência explicita quem solicita a cópia, onde ela passa a existir e qual objeto recebe o dano. Essas relações ajudam mais na explicação do que acrescentar classes apenas para aproximar o desenho de um exemplo genérico.

#### Uso da IA Generativa (senso crítico)

Usei o **Codex, baseado em GPT-6**, para analisar o repositório e apoiar a implementação, os diagramas, os testes e a documentação. Pedi que considerasse as entregas anteriores, escolhesse outro criacional além do Builder e do Singleton e mantivesse a solução simples. Também pedi uma apresentação visual, porque queria mostrar o comportamento do padrão durante a explicação.

A primeira proposta não me atendeu em dois pontos: o núcleo estava em JavaScript e a demonstração tinha um visual distante do projeto. Questionei a linguagem e pedi uma revisão do vídeo, da interface e da organização do subgrupo. O resultado foi a implementação em C++17, executável no terminal e compilada para o navegador, com os cenários, sprites, cores e tipografia que a entrega já utiliza. O ganho da ferramenta foi acelerar essa reformulação; a limitação foi precisar explicitar critérios que a primeira resposta não havia respeitado.

Segui o formato de registro usado na Entrega 02: relacionar trechos das solicitações, avaliação do resultado e evidência concreta. Os trechos abaixo são da interação desta entrega, agrupados por assunto.

#### Iterações e decisões

<div role="region" aria-label="Iterações de Yogi com IA Generativa" tabindex="0" style="overflow-x:auto;padding:0 6px 6px 0;">

| ID | Trecho da solicitação | Resultado e avaliação | Evidência adotada |
|:--:|-----------------------|----------------------|-------------------|
| P01 | “a cibelly vai fazer singleton. o magiolli fez builder, escolhe outro criacional” | O recorte escolhido foi Prototype na criação de inimigos. A comparação ficou ligada ao problema de reutilizar configurações e copiar o estado atual de uma instância. | [Escolha e comparação dos padrões](Prototype.md?id=padrão-escolhido). |
| P02 | “basta o simples bem feito bem entendível” | A implementação ficou com um contrato abstrato e um protótipo concreto. As três espécies são dados; o catálogo delega a clonagem. A análise considera que uma função de criação seria suficiente para configurações fixas. | [Código e análise crítica](Prototype.md?id=análise-crítica). |
| P03 | “Acho que o código tem que ser em C++ não?” | A primeira versão priorizou a facilidade de rodar no navegador. Questionei essa escolha e o núcleo foi refeito em C++17, coerente com o Builder. A versão visual passou a executar o C++ compilado; oito estados são comparados com a execução nativa. | [Implementação C++](Prototype.md?id=código-c) e [teste de paridade][paridade]. |
| P04 | “O vídeo e a demonstração ficaram muito ruins” e “Siga o visual da entrega” | Rejeitei a apresentação inicial. A revisão reutilizou os assets do jogo e aproximou a interface da identidade do site. O vídeo mostra criar, alterar, clonar o estado alterado e comparar os valores na mesma cena. | [Demonstração](Prototype.md?id=demonstração-interativa) e [vídeo](Prototype.md?id=vídeo). |
| P05 | “Organize nossa pasta do subgrupo 1 para ter os gofs na side bar certinho” | A visão geral passou a apontar para páginas próprias de Builder e Prototype. Isso permite acessar cada contribuição diretamente e evita misturar a apresentação do foco com um único padrão. | [Visão geral](GoFCriacional.md), [Builder](Builder.md) e barra lateral. |
| P06 | “chque tudo que fez, lembre de relacionar com referencias bibliograficas, embase, siga as notações corretamente” | A revisão comparou classes, assinaturas e relações UML com a implementação. No Builder, corrigiu a dependência de criação e incluiu o cliente. No Prototype, explicitou a composição e acrescentou a sequência de clonagem. | UML de [Builder](Builder.md?id=modelagem-notação-uml) e [Prototype](Prototype.md?id=modelagem-notação-uml); Gamma et al. (2000), OMG (2017) e C++ Core Guidelines. |
| P07 | “reverefique o que fez para ver se está realmente bom” | A conferência incluiu 13 casos nativos do Prototype, paridade entre versões e interação no navegador. No Builder, a revisão encontrou a aceitação de valores não finitos e acrescentou cinco verificações, totalizando 32. | [Resultados do Prototype](Base/Relatórios/SubEquipe_01/assets/prototype/testes.txt ':ignore') e [validação do Builder](Builder.md?id=como-rodar-e-testar). |

</div>

<p align="center">Tabela 2: Solicitações, avaliação e resultados do uso de IA por Yogi Nam de Souza Barbosa. Fonte: interação de elaboração e artefatos vinculados, 2026.</p>

#### Avaliação do processo

O uso mais produtivo foi transformar uma explicação abstrata em algo verificável. A demonstração permite observar o estado dos objetos; os testes conferem identidade, tipo e armazenamento independente, aspectos que a imagem sozinha não demonstra. A ferramenta também ajudou a consultar o contexto anterior. Na Entrega 02, há listas com numerações diferentes, então a rastreabilidade aponta para a lista efetivamente adotada pelo Diagrama de Classes e relaciona seus RF13 e RF17 aos atributos preservados na cópia.

A principal limitação foi a tendência de produzir uma solução apresentável antes de resolver a adequação ao projeto. Rodar no navegador não justificava escolher uma linguagem diferente do restante do foco, e ter uma interface visual não garantia que ela combinasse com a entrega. Minhas solicitações de revisão foram importantes para corrigir essa direção. O mesmo vale para o relato: listar vantagens genéricas da IA explica pouco; registrar o que foi questionado e o que mudou torna a avaliação mais concreta.

Também há um limite na conferência automática. A mesma ferramenta pode gerar o código e testes que repetem suas suposições. Por isso, as evidências foram organizadas em critérios de comportamento: modificar uma cópia deve preservar as demais, clonar um ferido deve manter sua vida e o catálogo deve funcionar por uma chamada virtual. As referências sustentam a interpretação do padrão e da notação; os testes sustentam os resultados dos casos executados. Esse conjunto oferece uma base melhor para explicar e defender as decisões na apresentação.

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000.

> NYSTROM, Robert. **Game Programming Patterns**. 2014. Capítulo Prototype. Disponível em: https://gameprogrammingpatterns.com/prototype.html. Acesso em: 19 out. 2026.

> OBJECT MANAGEMENT GROUP. **Unified Modeling Language, version 2.5.1**. 2017. Disponível em: https://www.omg.org/spec/UML/2.5.1. Acesso em: 19 out. 2026.

> STROUSTRUP, Bjarne; SUTTER, Herb (ed.). **C++ Core Guidelines**. Regra C.130. Disponível em: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c130-for-making-deep-copies-of-polymorphic-classes-prefer-a-virtual-clone-function-instead-of-public-copy-constructionassignment. Acesso em: 19 out. 2026.

## Nível de Contribuição dos Integrantes

| Nome | Participação no relato |
|------|-------------------|
| Cibelly Lourenço Ferreira | *A preencher* |
| Gabriel Andrade Magioli | *A preencher* |
| Yogi Nam de Souza Barbosa | Relato individual sobre Prototype, lições aprendidas, análise crítica e evidências. |

<p align="center">Tabela 3: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | | | | |
| 1.1 | 17/10/2026 | Adição do uso de IA Generativa de Gabriel Andrade Magioli | Gabriel Andrade Magioli | |
| 1.2 | 19/10/2026 | Registro do relato de Yogi sobre Prototype, decisões de cópia, uso do Codex e evidências de verificação. | Yogi Nam de Souza Barbosa | — |
| 1.3 | 19/10/2026 | Revisão do relato de Yogi: C++, críticas à proposta inicial, solicitações reais e evidências das correções. | Yogi Nam de Souza Barbosa | — |

<p align="center">Tabela 4: Histórico de versão.</p>


[paridade]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/blob/735073c689a9ebfd0a236ebe292e1f599e286a8b/src/criacional/prototype/tests/paridade.cjs
