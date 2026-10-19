# SubEquipe_01 — GoFs Criacionais

## Descrição

Relatório do **FOCO_01: GoFs Criacionais & IA Generativa** do **G4_ProjetoJogo**. Os padrões criacionais organizam como objetos são criados, compostos e representados (GAMMA et al., 2000). Cada aplicação tem uma página própria, acessível também pela barra lateral.

## Objetivo

Relacionar os problemas de criação de personagens, inimigos e da partida com os padrões adotados, reunindo UML, código C++, execução, justificativas e evidências de participação.

## Metodologia

O foco foi desenvolvido por contribuições individuais. Cada artefato apresenta o problema no domínio do jogo, a escolha fundamentada, o modelo UML, a implementação e a verificação do comportamento. A organização segue o [Artefato Padrão](/Guias/ArtefatoPadrao.md) e os [critérios da entrega](/Base/1.PadroesDeProjeto.md). Os pontos de vista sobre o processo estão no [relatório de IA Generativa](IAGenerativa.md).

## Conteúdo

| Padrão e responsável | Problema tratado | Modelo, código e execução |
|----------------------|------------------|--------------------------|
| **[Builder — Gabriel Andrade Magioli](Builder.md)** | Montar um personagem por etapas, escolhendo os atributos desejados. | UML, código C++17, menu no terminal, 32 verificações e vídeo. |
| **[Prototype — Yogi Nam de Souza Barbosa](Prototype.md)** | Criar inimigos a partir de objetos configurados e manter o estado de cada cópia independente. | UML de classes e sequência, C++17, demonstração visual em WebAssembly, 13 casos nativos, comparação entre versões e vídeo. |
| **[Singleton — Cibelly Lourenço Ferreira](Singleton.md)** | Manter uma única partida em execução, acessível pela exploração, alquimia, combate e menu de pausa, para que o salvamento reúna todo o progresso (RF03). | UML de classes e sequência, C++17, 8 testes, garantia em tempo de compilação, visualização do roteiro executado e vídeo. |

<p align="center">Tabela 1: Aplicações criacionais documentadas. Fonte: Gabriel Andrade Magioli, Yogi Nam de Souza Barbosa e Cibelly Lourenço Ferreira, 2026.</p>

O **Builder** define as partes antes de entregar um objeto. O **Prototype** parte de um objeto pronto e copia seu estado. São decisões complementares: um objeto montado por etapas poderia se tornar um protótipo, embora cada aplicação aqui permita estudar seu mecanismo separadamente.

O **Singleton** responde a outra pergunta: não *como* criar, mas *quantas* instâncias podem existir. Enquanto o Prototype multiplica inimigos independentes, o Singleton impede que surja uma segunda partida. Os três padrões controlam a criação de formas diferentes: por etapas, por cópia e por restrição.

### Rastreabilidade

| Artefato | Relação com o foco |
|----------|-------------------|
| [Diagrama de Classes da Entrega 02](https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_02/#/Base/Relatórios/SubEquipe_01/DiagramaClasses.md) | Domínio de personagens, inimigos, estatísticas e ataques. As páginas individuais explicitam o recorte adotado. |
| [Ata Geral 04](/Atas/AtaGeral04.md) | Organização dos três focos da entrega. |
| [Participações](/Base/1.2.ParticipacoesPadroesDeProjeto.md) | Comprobatórios das contribuições individuais. |
| [IA Generativa](IAGenerativa.md) | Relatos do processo, lições aprendidas e avaliação das sugestões das ferramentas. |

<p align="center">Tabela 2: Elos com os demais artefatos. Fonte: Autores, 2026.</p>

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000. Padrões criacionais.

## Nível de Contribuição dos Integrantes

| Nome | Contribuição registrada |
|------|------------------------|
| Gabriel Andrade Magioli | Builder: modelagem, código, testes, vídeo, documentação e relato de IA. |
| Yogi Nam de Souza Barbosa | Prototype: modelagem, código, testes, demonstração, vídeo, documentação e relato de IA; organização das páginas e revisão técnica do Builder. |
| Cibelly Lourenço Ferreira | Singleton: escolha do problema a partir da classe `Partida` da Entrega 02, modelagem, código, testes, visualização, vídeo, documentação e relato de IA. |

<p align="center">Tabela 3: Contribuições vinculadas aos artefatos individuais.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|-----------|-----------|---------|
| 1.0 | 17/10/2026 | Inclusão do padrão Builder: contexto, UML, código e instruções. | Gabriel Andrade Magioli | — |
| 1.1 | 17/10/2026 | Inclusão do vídeo do Builder. | Gabriel Andrade Magioli | — |
| 1.2 | 19/10/2026 | Inclusão do Prototype, metodologia e elos com artefatos individuais. | Yogi Nam de Souza Barbosa | — |
| 1.3 | 19/10/2026 | Organização da visão geral e das páginas individuais de Builder e Prototype. | Yogi Nam de Souza Barbosa | — |
| 1.4 | 19/10/2026 | Inclusão do Singleton na tabela de aplicações, comparação entre os três criacionais e contribuição de Cibelly. | Cibelly Lourenço Ferreira | — |

<p align="center">Tabela 4: Histórico de versão.</p>
