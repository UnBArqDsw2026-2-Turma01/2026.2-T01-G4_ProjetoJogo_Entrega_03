/* Valores de demonstração. A origem e as simplificações estão no README. */
(() => {
  'use strict';
  const { InimigoElemental, CatalogoDeInimigos } =
    typeof module !== 'undefined' && module.exports ? require('./modelo.js') : PrototypeJogo;

  function criarCatalogo() {
    const catalogo = new CatalogoDeInimigos();
    const modelos = [
      ['cogumelo', 'Cogumelo Andante', 24, 'fogo', 'Investida', 5],
      ['golem', 'Gólem de Enxofre', 36, 'eletricidade', 'Bafo Sulfuroso', 9],
      ['morcego', 'Morcego da Caverna', 18, 'luz', 'Mordida', 5]
    ];
    for (const [chave, nome, vidaMaxima, fraqueza, ataque, dano] of modelos) {
      catalogo.registrar(chave, new InimigoElemental({
        nome, sprite: chave, vidaMaxima, fraqueza, ataques: [{ nome: ataque, dano }]
      }));
    }
    return catalogo;
  }

  if (typeof module !== 'undefined' && module.exports) module.exports = { criarCatalogo };
  else globalThis.criarCatalogo = criarCatalogo;
})();
