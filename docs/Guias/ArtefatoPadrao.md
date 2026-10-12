# Artefato Padrão

Modelo padrão para a documentação de artefatos do projeto **G4_ProjetoJogo**. Todo artefato produzido pelo grupo deve seguir esta estrutura, garantindo consistência, rastreabilidade e facilidade de verificação entre as subequipes.

> Para criar um novo artefato, copie o modelo da seção [Modelo para Cópia](#modelo-para-cópia) e preencha todas as seções. Não omita tópicos. Para os padrões de projeto da Entrega 03, veja também [Artefatos de Padrões de Projeto](#artefatos-de-padrões-de-projeto).

## Estrutura

| Seção | Obrigatória | O que deve conter |
|-------|:-----------:|-------------------|
| Descrição | Sim | O que é o artefato e qual seu contexto no projeto. |
| Objetivo | Sim | Por que o artefato foi produzido e o que se pretende alcançar com ele. |
| Metodologia | Sim | Como o artefato foi elaborado: técnicas, ferramentas, reuniões e etapas. Embasar na literatura. |
| Conteúdo | Sim | O artefato em si (diagramas, tabelas, modelos). Toda tabela e figura deve ter fonte. |
| Referências | Sim | Fontes utilizadas, em ordem alfabética. |
| Nível de Contribuição dos Integrantes | Sim | Participação de cada membro no artefato. |
| Histórico de Versão | Sim | Versionamento com autor e revisor. |

<p align="center">Tabela 1: Estrutura do artefato padrão. Fonte: feito por LOPES, Marcelos (2026) baseado nas diretrizes da disciplina.</p>

## Regras de Preenchimento

- **Fontes:** toda tabela, figura ou diagrama deve ter legenda com fonte logo abaixo, no formato `Autor, ano`.
- **Referências:** listadas em ordem alfabética, seguindo a ABNT.
- **Revisor:** o revisor deve ser um membro diferente do autor.
- **Rastreabilidade:** sempre que o artefato derivar de outro, cite e crie o elo com link.
- **Senso crítico:** justificar as decisões tomadas, não apenas descrever o resultado.

---

## Modelo para Cópia

```markdown
# Nome do Artefato

## Descrição

## Objetivo

## Metodologia

## Conteúdo

Se tiver tabela, colocar a fonte da tabela.

Ex: SOBRENOME, Nome (2026).

## Referências

- Ordem alfabética

## Nível de Contribuição dos Integrantes

| Nome | % de Contribuição |
|------|-------------------|
|      |                   |
|      |                   |

<p align="center">Tabela X: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0    |      |           |           |         |

<p align="center">Tabela X+1: Histórico de versão.</p>
```

---

## Artefatos de Padrões de Projeto

Na Entrega 03, o artefato de cada padrão de projeto mantém todas as seções do modelo acima. A seção **Conteúdo** é organizada nas subseções exigidas pelo modelo da disciplina, que não devem ser omitidas:

| Subseção de Conteúdo | O que deve conter |
|----------------------|-------------------|
| Padrão Escolhido | Nome, categoria e intenção do padrão, com a justificativa da escolha embasada na literatura. |
| Contexto (Problema) | O contexto do jogo e o problema a ser resolvido com o padrão. |
| Modelagem (Notação UML) | A modelagem do padrão, estática ou dinâmica, na notação UML, com os participantes identificados. |
| Implementação (Código & Vídeo com Padrão em Execução) | A explicação de cada trecho de código dos participantes, o link para o código no repositório e o vídeo com o código em execução. |
| Rastreabilidade | Os elos com atas e com artefatos desta e das entregas anteriores. |

<p align="center">Tabela 2: Subseções de conteúdo dos artefatos de padrões de projeto. Fonte: modelo da disciplina, adaptado pelos autores, 2026.</p>

A seção **Metodologia** corresponde à metodologia do foco e deve contar como ocorreu o trabalho em equipe. Os pontos de vista de cada integrante sobre as lições aprendidas e o uso de IA Generativa ficam na página de IA Generativa da subequipe.

### Modelo de Conteúdo para Cópia

```markdown
## Conteúdo

### Padrão Escolhido

### Contexto (Problema)

### Modelagem (Notação UML)

### Implementação (Código & Vídeo com Padrão em Execução)

#### Código

#### Vídeo

### Rastreabilidade
```

### Modelo de Vídeo Incorporado

Para incorporar o vídeo do padrão em execução, substitua `ID_DO_VIDEO` pelo identificador do vídeo no YouTube e ajuste a legenda. Mantenha também o link direto, para o caso de o vídeo não carregar.

```html
<div style="max-width:720px;margin:0 auto;">
  <div style="position:relative;width:100%;padding-top:56.25%;">
    <iframe style="position:absolute;top:0;left:0;width:100%;height:100%;border:0;" src="https://www.youtube.com/embed/ID_DO_VIDEO" title="Padrão em execução" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>
  </div>
</div>

<p align="center">Vídeo X: Padrão em execução. Fonte: Autores, 2026.</p>
```

---

## Referências

> SERRANO, Milene; SERRANO, Maurício. **Arquitetura e Desenho de Software**. Universidade de Brasília, 2026.

## Nível de Contribuição dos Integrantes

| Nome | % de Contribuição |
|------|-------------------|    
| Marcelo de Araújo Lopes | 100% |

<p align="center">Tabela 3: Contribuição dos integrantes.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | 22/08/2026 | Criação do artefato padrão do grupo | Marcelo de Araújo Lopes | |

<p align="center">Tabela 4: Histórico de versão.</p>