/* GoF Prototype. Este arquivo roda no navegador e nos testes do Node.js. */
(() => {
  'use strict';

  function inteiro(valor, minimo, campo) {
    if (!Number.isSafeInteger(valor) || valor < minimo) {
      throw new RangeError(`${campo} deve ser um inteiro maior ou igual a ${minimo}.`);
    }
  }

  class Inimigo {
    clonar() {
      throw new Error('Cada inimigo deve implementar clonar().');
    }
  }

  class InimigoElemental extends Inimigo {
    #estado;

    constructor({ nome, sprite, fraqueza, vidaMaxima, vida = vidaMaxima, ataques }) {
      super();
      inteiro(vidaMaxima, 1, 'Vida máxima');
      inteiro(vida, 0, 'Vida');
      if (vida > vidaMaxima) throw new RangeError('Vida excede o máximo.');
      if (![nome, sprite, fraqueza].every(v => typeof v === 'string' && v.trim())) {
        throw new TypeError('Nome, sprite e fraqueza são obrigatórios.');
      }
      if (!Array.isArray(ataques) || ataques.length === 0) {
        throw new TypeError('Informe pelo menos um ataque.');
      }
      for (const ataque of ataques) {
        if (typeof ataque.nome !== 'string' || !ataque.nome.trim()) {
          throw new TypeError('O ataque precisa de nome.');
        }
        inteiro(ataque.dano, 0, 'Dano do ataque');
      }
      this.#estado = {
        nome, sprite, fraqueza, vidaMaxima, vida,
        ataques: ataques.map(({ nome, dano }) => ({ nome, dano }))
      };
    }

    clonar() {
      // O construtor também copia cada objeto da lista de ataques.
      // Copiar só a lista manteria os ataques compartilhados entre inimigos.
      return new InimigoElemental(this.#estado);
    }

    dados() {
      // A interface recebe uma fotografia dos dados, nunca o estado interno.
      return { ...this.#estado, ataques: this.#estado.ataques.map(a => ({ ...a })) };
    }

    receberDano(quantidade) {
      inteiro(quantidade, 0, 'Dano');
      this.#estado.vida = Math.max(0, this.#estado.vida - quantidade);
    }

    fortalecerAtaque(indice, aumento) {
      inteiro(indice, 0, 'Índice');
      inteiro(aumento, 0, 'Aumento');
      const ataque = this.#estado.ataques[indice];
      if (!ataque) throw new RangeError('Ataque inexistente.');
      inteiro(ataque.dano + aumento, 0, 'Novo dano');
      ataque.dano += aumento;
    }
  }

  class CatalogoDeInimigos {
    #modelos = new Map();

    registrar(chave, prototipo) {
      if (typeof chave !== 'string' || !chave.trim()) throw new TypeError('Chave vazia.');
      if (!(prototipo instanceof Inimigo)) throw new TypeError('Protótipo inválido.');
      if (this.#modelos.has(chave)) throw new Error(`Modelo já registrado: ${chave}`);
      // O catálogo possui sua própria cópia, isolada do objeto recebido.
      this.#modelos.set(chave, prototipo.clonar());
    }

    criar(chave) {
      const prototipo = this.#modelos.get(chave);
      if (!prototipo) throw new Error(`Modelo desconhecido: ${chave}`);
      // Não conhece InimigoElemental nem escolhe construtores por tipo.
      return prototipo.clonar();
    }
  }

  const api = { Inimigo, InimigoElemental, CatalogoDeInimigos };
  if (typeof module !== 'undefined' && module.exports) module.exports = api;
  else globalThis.PrototypeJogo = Object.freeze(api);
})();
