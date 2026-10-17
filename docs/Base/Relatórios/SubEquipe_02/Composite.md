# SubEquipe 02: Padrão GoF Composite (Interface de Usuário em C++)

## Objetivo

Organizar elementos simples de tela (rótulos, botões e barras de progresso) e contêineres complexos (painéis e menus) em uma estrutura de árvore uniforme. O objetivo é permitir que rotinas de renderização e comandos de entrada naveguem por qualquer nível da interface de combate e menus sem depender de verificações manuais de tipo.

## Metodologia

A definição do padrão ocorreu na primeira reunião da SubEquipe 02, documentada na [Ata 01 do Subgrupo 02](/Atas/AtaSub02_01.md). A subequipe adotou C++17 como linguagem base para todos os padrões estruturais, assegurando que as bibliotecas pudessem ser compiladas tanto como executáveis de console quanto integradas à Godot Engine.

Para desenhar a estrutura de classes, usei os requisitos de interface levantados na Entrega 02: a HUD de Batalha descrita no Léxico L16 e as transições de turno do Diagrama de Estados do Combate.

---

## Conteúdo

### 1. Padrão Escolhido e Fundamentação Teórica

| Item | Descrição |
|------|-----------|
| Padrão | Composite |
| Categoria | GoF Estrutural |
| Autor Responsável | João Victor da Silva Batista de Farias ([@beyondmagic](https://github.com/beyondmagic)) |
| Linguagem | C++17 (com Makefile e paridade em GDScript) |
| Intenção do Padrão | Compor objetos em estruturas de árvore para representar hierarquias do tipo parte-todo, permitindo tratar objetos individuais e composições de objetos de maneira uniforme (GAMMA et al., 2000). |
| Justificativa da Escolha | No G4_ProjetoJogo, as interfaces de tela combinam múltiplos controles visuais agrupados em painéis que abrem, fecham e se reorganizam durante o combate. Tratar cada tipo com verificações manuais (`if type == ...`) criaria acoplamento direto entre as regras de turno e os controles visuais. O Composite isola essas variações e trata folhas e compostos sob a mesma interface. |

<p align="center">Tabela 1: Padrão GoF Composite na SubEquipe 02. Fonte: Autores, 2026.</p>

---

### 2. Contexto e Problema de Design

Na interface de combate do G4_ProjetoJogo, a tela precisa apresentar informações de naturezas diferentes ao mesmo tempo:

- O status do herói em tempo real (barras de vida, mana e prontidão de turno);
- O menu contextual de comandos (atacar, usar item, misturar elementos alquímicos ou fugir);
- Os dados e barras de vida dos inimigos;
- O histórico textual dos eventos da rodada (registro de ações);
- Janelas contextuais acionadas sob demanda, como a bancada rápida de campo para sintetizar poções no meio da luta.

Sem um padrão de composição, qualquer rotina que atualizasse a tela ou tratasse cliques precisaria conhecer a estrutura interna de cada janela. Se um novo painel fosse adicionado ou reorganizado, o código de controle precisaria ser reescrito com novas condições. Ao adotar o Composite, o HUD passa a tratar botões individuais e grupos inteiros de controles pela mesma interface abstrata.

---

### 3. Modelagem na Notação UML

O diagrama de classes abaixo detalha a estrutura do padrão Composite implementada em C++:

```mermaid
classDiagram
    class UIComponent {
        <<Component>>
        #String m_name
        #bool m_visible
        #Vector2 m_position
        #Vector2 m_size
        #UIComponent* m_parent
        +render(indentLevel: int) String*
        +handleInput(actionName: String) bool*
        +setVisible(visible: bool) void
        +isComposite() bool
        +add(component: shared_ptr~UIComponent~) void
        +remove(component: shared_ptr~UIComponent~) void
        +getChildren() vector~shared_ptr~UIComponent~~
    }

    class UIComposite {
        <<Composite>>
        #vector~shared_ptr~UIComponent~~ m_children
        +add(component: shared_ptr~UIComponent~) void
        +remove(component: shared_ptr~UIComponent~) void
        +getChildren() vector~shared_ptr~UIComponent~~
        +findByName(targetName: String) shared_ptr~UIComponent~
        +render(indentLevel: int) String
        +handleInput(actionName: String) bool
        +isComposite() bool
    }

    class UILabel {
        <<Leaf>>
        -String m_text
        +setText(newText: String) void
        +render(indentLevel: int) String
    }

    class UIButton {
        <<Leaf>>
        -String m_labelText
        -String m_actionId
        -bool m_isEnabled
        -int m_clickCount
        +render(indentLevel: int) String
        +handleInput(actionName: String) bool
    }

    class UIProgressBar {
        <<Leaf>>
        -String m_barType
        -float m_currentValue
        -float m_maxValue
        +setValues(cur: float, max: float) void
        +getPercentage() float
        +render(indentLevel: int) String
    }

    class UIPanel {
        <<ConcreteComposite>>
        -String m_title
        +render(indentLevel: int) String
    }

    class BattleHUD {
        <<DomainComposite>>
        -shared_ptr~UIPanel~ m_playerPanel
        -shared_ptr~UIPanel~ m_actionMenu
        -shared_ptr~UIPanel~ m_enemyPanel
        -shared_ptr~UIPanel~ m_combatLog
        +buildHUD(playerName: String, enemyName: String) void
        +setTurnActive(isPlayerTurn: bool) void
    }

    UIComponent <|-- UIComposite : herança
    UIComponent <|-- UILabel : herança
    UIComponent <|-- UIButton : herança
    UIComponent <|-- UIProgressBar : herança
    UIComposite <|-- UIPanel : herança
    UIComposite <|-- BattleHUD : herança
    UIComposite o-- UIComponent : agrega m_children (0..*)
```

<p align="center">Figura 1: Diagrama de classes do padrão GoF Composite na notação UML. Fonte: elaboração própria (Farias, João Victor, 2026).</p>

---

### 4. Implementação (Código & Vídeo com Padrão em Execução)

O código em C++ está em [`src/estrutural/composite/`](https://github.com/UnBArqDsw2026-2-Turma01/2026.2-T01-G4_ProjetoJogo_Entrega_03/tree/main/src/estrutural/composite/). Há também uma versão equivalente em GDScript nos arquivos `.gd` do mesmo diretório para execução direta no motor Godot.

#### Destaques de Código em C++

1. **Renderização recursiva no nó composto (`ui_composite.hpp`):**
Se o painel estiver visível, ele imprime seus dados de posição e repassa a chamada `render()` para cada controle filho da sua lista.

```cpp
std::string render(int indentLevel = 0) const override {
    if (!m_visible) return "";
    std::ostringstream ss;
    ss << getIndent(indentLevel) << "+-- [COMPOSTO: " << m_name << "] (pos: " 
       << m_position.toString() << ", size: " << m_size.toString() << ")\n";

    for (const auto& child : m_children) {
        ss << child->render(indentLevel + 1);
    }
    return ss.str();
}
```

2. **Despacho de comandos em profundidade (`ui_composite.hpp`):**
Ao receber uma ação de entrada (como clique de mouse ou atalho de teclado), o composto percorre seus filhos em ordem reversa (da frente para trás no plano visual) para que o elemento em primeiro plano intercepte o evento antes dos painéis de fundo.

```cpp
bool handleInput(const std::string& actionName) override {
    if (!m_visible) return false;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->handleInput(actionName)) {
            return true;
        }
    }
    return false;
}
```

3. **Consumo de evento na folha (`ui_button.hpp`):**
O botão compara o comando recebido com seu identificador de ação. Quando há correspondência, incrementa o contador de cliques e consome o evento retornando verdadeiro, o que interrompe a busca na árvore.

```cpp
bool handleInput(const std::string& actionName) override {
    if (!m_visible || !m_isEnabled) return false;
    if (actionName == m_actionId || actionName == m_name) {
        m_clickCount++;
        std::cout << "   >> [EVENTO UI] Botão '" << m_name << "' acionado! (Cliques: " << m_clickCount << ")\n";
        return true;
    }
    return false;
}
```

#### Compilação e Execução

Para compilar e rodar a demonstração em C++ via terminal:

```bash
make -C src/estrutural/composite run
```

Para rodar a versão equivalente em GDScript no Godot Engine em modo headless:

```bash
godot --headless --path src/estrutural -s composite/composite_demo.gd
```

#### Vídeo

Apresentação do padrão Composite: contextualização do problema na interface de combate do jogo, modelo de classes UML, explicação dos participantes em código e demonstração prática da árvore de UI e do despacho de eventos no terminal.

<div style="max-width:720px;margin:0 auto;">
  <div style="position:relative;width:100%;padding-top:56.25%;">
    <iframe style="position:absolute;top:0;left:0;width:100%;height:100%;border:0;" src="https://www.youtube.com/embed/ID_DO_VIDEO" title="Padrão Composite em execução" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>
  </div>
</div>

<p align="center">Vídeo 1: Padrão Composite aplicado à interface de combate, em execução. Fonte: João Victor Farias, 2026. <a href="https://youtu.be/ID_DO_VIDEO" target="_blank">Link direto para o vídeo</a>.</p>

---

### 5. Rastreabilidade com Artefatos Anteriores

| Artefato de Origem | Elo com o Padrão Composite |
|--------------------|----------------------------|
| [Léxico L16: HUD de Batalha](https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_02/#/Base/Léxicos) | O Léxico define o HUD como a interface visual que reúne HP, MP, barra de turnos e menu de ações, mapeado diretamente na classe `BattleHUD`. |
| [Modelagem Estática da Entrega 02](https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_02/#/Base/Relatórios/SubEquipe_02/ModelagemEstatica) | O pacote de apresentação gráfica e os módulos de gatilho foram refinados para a hierarquia polimórfica de `UIComponent`. |
| [Diagrama de Estados do Combate](https://unbarqdsw2026-2-turma01.github.io/2026.2-T01-G4_ProjetoJogo_Entrega_02/#/Base/DiagramasDeEstados) | A transição de turno em que o menu de ações do jogador fica ativo ou oculto é feita por `hud->setTurnActive(isPlayerTurn)`, ocultando a subárvore inteira de uma só vez. |
| [Ata 01 da SubEquipe 02](/Atas/AtaSub02_01.md) | Decisão do subgrupo que atribuiu o padrão Composite a João Victor da Silva Batista de Farias. |

<p align="center">Tabela 2: Rastreabilidade com artefatos anteriores. Fonte: Autores, 2026.</p>

---

## Referências

> GAMMA, Erich; HELM, Richard; JOHNSON, Ralph; VLISSIDES, John. **Padrões de Projeto**: soluções reutilizáveis de software orientado a objetos. Porto Alegre: Bookman, 2000.

> LARMAN, Craig. **Utilizando UML e Padrões**: uma introdução à análise e ao projeto orientados a objetos e ao desenvolvimento iterativo. 3. ed. Porto Alegre: Bookman, 2007.

> REFACTORING.GURU. **Composite**. Disponível em: https://refactoring.guru/pt-br/design-patterns/composite. Acesso em: 16 out. 2026.

> SERRANO, Milene; SERRANO, Maurício. **Arquitetura e Desenho de Software**. Universidade de Brasília, 2026.

> STROUSTRUP, Bjarne. **The C++ Programming Language**. 4. ed. Boston: Addison-Wesley, 2013.

## Nível de Contribuição dos Integrantes

| Nome | % de Contribuição |
|------|:-----------------:|
| João Victor da Silva Batista de Farias | 100% |

<p align="center">Tabela 3: Contribuição dos integrantes no padrão Composite.</p>

## Histórico de Versão

| Versão | Data | Descrição | Autor(es) | Revisor |
|:------:|------|:----------|:----------|:--------|
| 1.0 | 16/10/2026 | Elaboração inicial da documentação do GoF Composite em C++, modelagem UML, código-fonte e rastreabilidade | João Victor da Silva Batista de Farias | Marcelo de Araújo Lopes |
| 1.1 | 17/10/2026 | Revisão e humanização do texto, eliminação de clichês e padronização do bloco de vídeo | João Victor da Silva Batista de Farias | Marcelo de Araújo Lopes |

<p align="center">Tabela 4: Histórico de versão.</p>
