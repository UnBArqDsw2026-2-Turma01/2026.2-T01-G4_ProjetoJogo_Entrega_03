# Ata Geral 04 — Demonstração da Godot Engine e alinhamento da entrega de Padrões de Projeto

## Identificação

| Data | Horário de Início | Horário de Término | Local | Projeto | Redator |
| :--: | :---------------: | :----------------: | :---: | :-----: | :-----: |
| 06/10/2026 | 20:56 | 21:30 | Google Meet | G4_ProjetoJogo | Gemini 3.1 Pro, com revisão de Marcos Vinícius Gündel da Silva |

<p align="center">Tabela 1: Identificação da reunião. Fonte: Autores, 2026.</p>

## Participantes da Reunião

As anotações da reunião identificam apenas os integrantes que tiveram falas registradas. As presenças de Pedro e de Yogi, que não aparecem nas anotações, foram confirmadas na revisão da ata. A presença dos demais não consta nos registros.

| Convocados | Presente (Sim/Não) |
| :--------- | :----------------: |
| Carlos Henrique Brasil de Souza | Não registrado |
| Cibelly Lourenço Ferreira | Sim |
| Gabriel Andrade Magioli | Sim |
| João Igor Pereira da Costa | Não registrado |
| João Victor da Silva Batista de Farias | Sim |
| Marcelo de Araújo Lopes | Sim |
| Marcos Vinícius Gündel da Silva | Sim |
| Pedro Teixeira Moriel Sanchez | Sim |
| Renan Pereira Reis | Sim |
| Yogi Nam de Souza Barbosa | Sim |

<p align="center">Tabela 2: Participantes da reunião. Fonte: Autores, 2026.</p>

## Pauta

| Nº | Descrição |
| :-: | :-------- |
| 1 | Demonstração da Godot Engine |
| 2 | Escopo da terceira entrega |
| 3 | Divisão dos padrões de projeto por subgrupo |
| 4 | Cronograma, linguagem e repositório modelo |
| 5 | Modelagem e prazos da atividade |

<p align="center">Tabela 3: Pauta da reunião. Fonte: Autores, 2026.</p>

## Pendências Anteriores

| Nº | Pendência | Responsável | Data |
| :-: | :-------- | :---------- | :--: |
| — | As anotações da reunião não registram pendências de reuniões anteriores. | — | — |

<p align="center">Tabela 4: Pendências anteriores. Fonte: Autores, 2026.</p>

## Assuntos Tratados

| Nº | Descrição | Tipo |
| :-: | :-------- | :--: |
| 1 | Demonstração da Godot Engine: Marcelo apresentou a ferramenta, baseada em nós (*nodes*) e cenas agrupadas, e exibiu um protótipo rápido com movimentação, dano, repulsão (*knockback*) e reinício de cena. A linguagem padrão é o GDScript, baseada em Python, e a ferramenta também aceita C++ e C#. Destacou o apoio de inteligência artificial para agilizar o desenvolvimento. | 1. Apresentação |
| 1 | Requisitos de sistema da Godot Engine: a pedido de Renan, Marcelo informou que a ferramenta ocupa cerca de 2 GB de armazenamento, exige no mínimo 4 GB de memória RAM e é compatível com Windows, macOS, Linux e SteamOS. | 1. Apresentação |
| 2 | Escopo da terceira entrega: Gabriel questionou se a entrega, com prazo em 19/10/2026, exige a implementação de uma parte funcional do jogo ou se basta aplicar os padrões de projeto em código simplificado (*mocks*). João Victor apresentou o entendimento de que a implementação prática do jogo seria exigida apenas na quarta entrega. O grupo decidiu confirmar a diretriz com a professora na sexta-feira, 09/10/2026, antes de prosseguir. | 5. Pendência |
| 3 | Divisão dos padrões de projeto: ficou definido que o Subgrupo 01 aplicará o padrão criacional, o Subgrupo 02 o estrutural e o Subgrupo 03 o comportamental, seguindo a ordem dos subgrupos. | 2. Decisão |
| 4 | Cronograma, linguagem e repositório modelo: Marcelo exibiu o cronograma geral da disciplina. Renan e Marcos debateram a dificuldade de unificar a documentação e o comportamento do jogo caso cada subgrupo use uma linguagem diferente. João Victor compartilhou no chat um repositório modelo no GitHub, em Python, com execução pelo terminal. A linguagem a ser adotada não foi definida na reunião. | 1. Apresentação |
| 5 | Modelagem separada ou unificada: Renan, Gabriel e Marcos discutiram se a modelagem deve ser mantida separada por entrega ou unificada desde já, para facilitar a implementação na quarta entrega. O grupo optou por esclarecer a questão com a professora. | 5. Pendência |
| 5 | Prazo da modelagem: o grupo alinhou a conclusão da modelagem estática de classes do padrão de cada subgrupo entre a terça-feira, 13/10/2026, e a quarta-feira, 14/10/2026. | 2. Decisão |

<p align="center">Tabela 5: Assuntos tratados. Fonte: Autores, 2026.</p>

> **Tipos:** 1. Apresentação · 2. Decisão · 3. Definição · 4. Solicitação · 5. Pendência.

## Próxima Reunião

As anotações não registram a data da próxima reunião geral. O próximo marco acordado é a consulta à professora na sexta-feira, 09/10/2026.

## Compromissos

| Nº | Compromisso | Responsável | Data |
| :-: | :---------- | :---------- | :--: |
| 1 | Confirmar com a professora se a entrega exige implementação em jogo funcional ou apenas código simplificado aplicando os padrões de projeto, e se a modelagem deve ser separada por entrega ou unificada | O grupo | 09/10/2026 |
| 2 | Registrar a distribuição dos padrões de projeto no grupo de mensagens | Marcos Vinícius Gündel da Silva | Não registrada |
| 3 | Finalizar a modelagem estática de classes correspondente ao padrão de cada subgrupo | Cada subgrupo | 13/10/2026 a 14/10/2026 |
| 4 | Aplicar os padrões de projeto criacional, estrutural e comportamental conforme a divisão por subgrupo | Cada subgrupo | 19/10/2026 |

<p align="center">Tabela 6: Compromissos acordados. Fonte: Autores, 2026.</p>

## Anotações Automáticas da Reunião (Gemini 3.1 Pro)

### Resumo

Demonstração da engine Godot e alinhamento de padrões de projeto e prazos.

- **Demonstração da Godot Engine:** apresentação da ferramenta Godot, destacando o funcionamento em nós e o uso de inteligência artificial.
- **Divisão de padrões:** definição da distribuição dos padrões de projeto criacional, estrutural e comportamental entre os subgrupos.
- **Alinhamento de prazos:** discussão sobre a modelagem e definição do prazo para a conclusão da etapa entre terça e quarta-feira.

### Decisões

- **Distribuição dos padrões de projeto por subgrupo (alinhada):** o grupo definiu que o Subgrupo 01 aplicará o padrão criacional, o Subgrupo 02 o estrutural e o Subgrupo 03 o comportamental.
- **Validação de diretrizes de modelagem com a professora (precisa de mais conversa):** o grupo decidiu consultar a professora na sexta-feira para confirmar os requisitos de modelagem e o formato de execução do código antes de prosseguir.

### Próximas Etapas

- [ ] **[O grupo] Confirmar requisitos:** confirmar com a professora na sexta-feira se a entrega exige implementação em jogo funcional ou apenas código simplificado aplicando os padrões de projeto.
- [ ] **[O grupo] Distribuir padrões:** aplicar os padrões de projeto criacional, estrutural e comportamental conforme a divisão preestabelecida por subgrupo.
- [ ] **[O grupo] Entregar modelagem:** finalizar a modelagem estática de classes correspondente ao padrão de cada subgrupo para entrega até a próxima terça-feira.

### Detalhes

- **Demonstração da Godot Engine:** Marcelo Araújo apresentou a ferramenta Godot Engine, demonstrando o funcionamento baseado em nós (*nodes*) e cenas agrupadas, como a estrutura de um inimigo contendo *sprite*, *body* e formas de colisão (*collision shape* ou *hitbox*) (00:00:01). Explicou que a linguagem padrão é o GDScript, baseada em Python, embora o software também aceite C++ e C#, e exibiu um protótipo rápido com movimentação, dano, repulsão (*knockback*) e reinício de cena, destacando o apoio de inteligência artificial para agilizar o desenvolvimento (00:02:20).
- **Requisitos de sistema da Godot Engine:** Renan Pereira Reis solicitou a verificação dos requisitos mínimos da ferramenta, e Marcelo Araújo confirmou que o programa ocupa cerca de 2 GB de armazenamento, exige um mínimo de 4 GB de memória RAM e possui compatibilidade com múltiplos sistemas operacionais, incluindo Windows, macOS, Linux e SteamOS (00:03:24).
- **Dúvidas sobre o escopo da terceira entrega:** Gabriel Magioli levantou questionamentos sobre a necessidade de implementar uma parte funcional do jogo na terceira entrega, com prazo para 19 de outubro, ou se bastaria aplicar padrões de projeto usando códigos simulados (*mocks*), citando como exemplo a criação de instâncias de mapas de fogo, água e gelo com o padrão *Factory Method* (00:04:25). João Victor compartilhou o entendimento de que a implementação prática do jogo seria exigida apenas na quarta entrega, e o grupo concordou em confirmar essa diretriz com a docente na sexta-feira (00:05:15) (00:06:52).
- **Divisão dos padrões de projeto por subgrupo:** Gabriel Magioli apontou que cada subgrupo deverá implementar um tipo distinto de padrão de projeto: comportamental, criacional e estrutural (00:08:33). Diante disso, Marcos Gündel definiu formalmente a distribuição baseada na ordem dos subgrupos, com o Subgrupo 01 no padrão criacional, o Subgrupo 02 no estrutural e o Subgrupo 03 no comportamental, e encarregou-se de registrar essa definição no grupo de mensagens (00:14:28).
- **Análise do cronograma, da linguagem e do repositório modelo:** a pedido de Marcos Gündel, Marcelo Araújo exibiu o cronograma geral disponível na plataforma educacional. Renan Pereira Reis e Marcos Gündel debateram a complexidade de unificar a documentação e o comportamento do jogo caso cada subgrupo utilize linguagens diferentes, enquanto João Victor enviou no chat um link de repositório modelo (*template*) no GitHub que utiliza a linguagem Python com execução via terminal (00:10:44).
- **Definição de modelagem e prazos para a atividade:** Renan Pereira Reis, Gabriel Magioli e Marcos Gündel discutiram se a modelagem deve ser mantida separada por entrega ou unificada desde já para facilitar a implementação posterior na quarta entrega, optando por esclarecer a questão com a docente (00:17:48). Para o prazo de conclusão da modelagem, após Gabriel Magioli relatar uma viagem com retorno previsto para a segunda-feira, o grupo alinhou coletivamente, com a participação de Cibelly Lourenço, o direcionamento para finalizar a etapa entre a terça e a quarta-feira (00:19:27).

> As anotações acima foram geradas automaticamente pelo Gemini 3.1 Pro a partir da transcrição da reunião e revisadas por Marcos Vinícius Gündel da Silva.

## Gravação da Reunião

- **[Gravação em Vídeo (YouTube)](https://youtu.be/J0cauwJ3fxY)**

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | 11/10/2026 | Criação da ata | [Marcos Vinícius](https://github.com/MarcosViniciusG) | |

<p align="center">Tabela 7: Histórico de versão. Fonte: Autores, 2026.</p>
