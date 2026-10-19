# Singleton na partida do jogo

**Responsável: Cibelly Lourenço Ferreira.** Aplicação do GoF criacional Singleton em C++17. A classe `Partida` guarda o progresso da sessão e só pode existir uma vez; exploração, bancada de alquimia, combate e menu de pausa a obtêm por `Partida::instancia()`.

## Executar

Com GCC/Clang com suporte a C++17 e Make, na raiz do repositório:

```bash
make -C src/criacional/singleton run    # menu interativo
make -C src/criacional/singleton demo   # roteiro fixo de 8 passos
make -C src/criacional/singleton test   # 8 testes + garantia de compilação
```

Sem o `make`:

```bash
cd src/criacional/singleton
g++ -std=c++17 src/partida.cpp src/clientes.cpp src/main.cpp -o singleton
./singleton
```

No menu, cada opção chama um cliente diferente. Um texto depois do número troca o valor padrão:

| Entrada | Cliente chamado |
|---------|-----------------|
| `1 Caverna` | `Exploracao::entrarEm` |
| `2 Poção Flamejante` | `BancadaDeAlquimia::criarPocao` |
| `3 Gólem de Enxofre` | `Combate::derrotar` |
| `4` | `MenuDePausa::salvar` |
| `5` | `MenuDePausa::salvarESair` |
| `6 Alquimista` | novo jogo (`Partida::iniciar`) |
| `0` | fechar o programa |

Depois de cada chamada, o programa mostra o endereço devolvido por `Partida::instancia()` (sempre o mesmo) e o progresso atual.

## Organização

| Arquivo | Responsabilidade |
|---------|------------------|
| `src/partida.hpp` e `.cpp` | O Singleton `Partida`, o `Salvamento` (cópia do progresso) e a enumeração `EstadoDaPartida`. |
| `src/clientes.hpp` e `.cpp` | Os quatro clientes. Nenhum recebe a partida por parâmetro. |
| `src/main.cpp` | Menu interativo, roteiro em texto (`--demo`) e em JSON (`--roteiro`). |
| `tests/testes.cpp` | 8 testes e 6 `static_assert`. |
| `tests/nao_compila.cpp` | Código que tenta criar e copiar uma `Partida`; `make garantia` só passa se ele **não** compilar. |

## Decisões

- **Variável estática local** em `instancia()` (*Meyers Singleton*): criada na primeira chamada, destruída no fim do programa, sem `new`/`delete`. A inicialização é segura entre threads desde o C++11.
- **Construtor e destrutor privados; cópia e atribuição com `= delete`.** Ao declarar a cópia, o compilador também deixa de gerar o movimento.
- **`iniciar()` é o "novo jogo".** Zera o progresso, mas mantém o mesmo objeto. Os testes usam essa operação para começar de um estado conhecido, sem precisar de um método especial para testes.
- **`Salvamento` é guardado por valor.** Um salvamento antigo não muda quando o jogo continua.
- **A partida só registra resultados.** Regras de combate e de alquimia continuam fora dela, para o Singleton não virar um depósito de responsabilidades.

## Visualização no relatório

`make publicar` executa `./bin/singleton --roteiro` e grava a saída em [`docs/Base/Relatórios/SubEquipe_01/singleton/roteiro.js`](../../../docs/Base/Relatórios/SubEquipe_01/singleton/roteiro.js). A página [`index.html`](../../../docs/Base/Relatórios/SubEquipe_01/singleton/index.html) só desenha esses passos: nenhuma regra da partida é refeita em JavaScript. Funciona offline, abrindo o arquivo no navegador.

Cenários (`fundo_floresta.png`, `fundo_caverna.png`), sprites dos inimigos e ícones foram copiados sem edição de [`src/estrutural/assets/`](../../estrutural/assets/), produzidos pela SubEquipe 02. A fonte Pixelify Sans segue a tipografia do site, com a licença OFL em `assets/`.

## Documentação

[Relatório com UML, vídeo, análise crítica e rastreabilidade](../../../docs/Base/Relatórios/SubEquipe_01/Singleton.md).

| Versão | Data | Descrição | Autor |
|--------|------|-----------|-------|
| 1.0 | 19/10/2026 | Implementação do Singleton, clientes, testes, garantia de compilação e roteiro para a visualização. | Cibelly Lourenço Ferreira |
