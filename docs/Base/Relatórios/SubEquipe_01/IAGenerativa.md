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
| Yogi Nam de Souza Barbosa | Prototype copia o estado de um objeto pronto; a independência precisa abranger também a lista de ataques. Diferença entre clonar o modelo e clonar uma instância alterada. | Codex (GPT-6) na análise, implementação, documentação e verificação do Prototype. Conferência por referências, testes de isolamento e demonstração visual; cuidado com complexidade desnecessária e afirmações sem evidência. |

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

- **revisão da documentação**: correção de texto, organização das seções do [GoF Criacional](GoFCriacional.md) segundo o modelo da disciplina, legendas das figuras e tabelas e instruções de como rodar e testar o código;
- **apoio ao planejamento**: discussão de como transformar o diagrama de classes que modelei (Builder, CharacterBuilder, Character e AttackAction) em código C++, de como estruturar o programa interativo no terminal e de quais casos os testes deveriam cobrir.
- **apoio no desenvolvimento**: Fazendo uso de tecnicas como SDD, e agentes de code-review para garantir que padrões de código estavam sendo seguidos sem possuir bugs.

A modelagem UML e as decisões sobre o comportamento do programa, como escolher os atributos digitando 0 ou 1 e manter a interação no terminal, foram minhas. As sugestões da IA foram conferidas com a literatura (GAMMA et al., 2000) e com a execução do programa e dos testes antes de entrarem na entrega.

### Yogi Nam de Souza Barbosa

#### Lições Aprendidas

A principal lição do [Prototype](Prototype.md) é que criar outro objeto com a mesma aparência não basta. Ele também precisa ter estado próprio. No exemplo dos inimigos, a vida é um número simples, mas os ataques ficam dentro de uma lista de objetos. Se apenas a lista for copiada, fortalecer o ataque de um inimigo pode alterar os demais. Por isso, o construtor copia cada ataque, e os testes verificam tanto a vida quanto os elementos dessa lista.

Outro ponto importante é a origem da cópia. Clonar o modelo do catálogo cria um inimigo com os atributos preservados. Clonar um inimigo que já recebeu dano mantém a vida reduzida. A demonstração coloca essas duas situações lado a lado: a cópia alterada tem vida 17 e ataque 7, enquanto uma nova cópia do modelo continua com vida 24 e ataque 5. Isso ajuda a explicar o padrão pelo comportamento, além do diagrama.

A comparação com os outros criacionais também esclarece a escolha. O Builder de Gabriel trata da montagem por etapas; o Prototype reutiliza uma configuração pronta. Para este exemplo, criar uma subclasse para cada espécie acrescentaria estrutura sem mudar o comportamento. Cogumelo, gólem e morcego ficaram como configurações da mesma classe. O ponto central é o contrato de clonagem, não a quantidade de classes.

O cuidado com a documentação foi ligar essa decisão ao domínio já modelado. A Entrega 02 contém listas com numerações diferentes de requisitos. O relatório identifica a lista adotada no Diagrama de Classes e usa seus RF13 e RF17 para relacionar fraquezas e estatísticas aos dados preservados na clonagem.

#### Uso da IA Generativa (senso crítico)

Usei o **Codex, baseado em GPT-6**, para apoiar a análise do repositório e a produção do artefato. A orientação inicial foi escolher um criacional diferente do Singleton e do Builder, aproveitar o contexto das entregas anteriores e manter uma solução simples, com uma demonstração visual fácil de entender.

A ferramenta auxiliou em quatro frentes:

- **Análise e escolha:** leitura dos artefatos, comparação dos criacionais e definição do recorte de criação de inimigos.
- **Código e apresentação:** implementação da clonagem, proteção do catálogo, UML e interface para alterar uma cópia e observar as demais.
- **Verificação:** geração e execução dos testes de identidade, estado, ataques aninhados e entradas inválidas; interação automatizada no navegador e gravação da execução.
- **Documentação:** organização da rastreabilidade, das referências, das instruções de reprodução e deste relato.

O ganho foi conseguir observar a mesma decisão em três formas: diagrama, código e execução. O principal cuidado é que uma explicação convincente pode esconder um problema de referência compartilhada. Por isso, a verificação exige resultados concretos: mudar um ataque da cópia deve preservar o original, outra cópia e o catálogo. A mesma atenção vale para os atributos usados na demonstração, cuja origem e ajustes didáticos foram registrados no README.

Também considero importante avaliar se o padrão está sendo usado para resolver o problema ou apenas para aumentar a estrutura. O relatório compara a solução com uma função de criação simples e discute o custo de copiar os ataques. O capítulo de Nystrom ajuda a sustentar essa avaliação, ao tratar das escolhas de cópia e das alternativas ao Prototype em jogos.

As evidências desta assistência são o [código e os testes][codigo-prototype], a [saída dos 12 testes](Base/Relatórios/SubEquipe_01/assets/prototype/testes.txt ':ignore'), o [diagrama e a análise](Prototype.md) e o [vídeo da execução](Prototype.md?id=vídeo). Os testes automatizados mostram o comportamento nos casos exercitados; o entendimento das decisões continua sendo parte da responsabilidade de quem apresenta o trabalho.

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000.

> NYSTROM, Robert. **Game Programming Patterns**. 2014. Capítulo Prototype. Disponível em: https://gameprogrammingpatterns.com/prototype.html. Acesso em: 19 out. 2026.

## Nível de Contribuição dos Integrantes

| Nome | Participação no relato |
|------|-------------------|
| Cibelly Lourenço Ferreira | *A preencher* |
| Gabriel Andrade Magioli | *A preencher* |
| Yogi Nam de Souza Barbosa | Relato individual sobre Prototype, lições aprendidas, análise crítica e evidências. |

<p align="center">Tabela 2: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | | | | |
| 1.1 | 17/10/2026 | Adição do uso de IA Generativa de Gabriel Andrade Magioli | Gabriel Andrade Magioli | |
| 1.2 | 19/10/2026 | Registro do relato de Yogi sobre Prototype, decisões de cópia, uso do Codex e evidências de verificação. | Yogi Nam de Souza Barbosa | — |

<p align="center">Tabela 3: Histórico de versão.</p>

[codigo-prototype]: https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/tree/main/src/criacional/prototype
