const { test } = require('node:test');
const assert = require('node:assert/strict');
const { Inimigo, InimigoElemental, CatalogoDeInimigos } = require('../modelo.js');
const { criarCatalogo } = require('../catalogo.js');

function configuracao() {
  return { nome: 'Cogumelo', sprite: 'cogumelo', fraqueza: 'fogo', vidaMaxima: 24,
    ataques: [{ nome: 'Investida', dano: 5 }, { nome: 'Esporos', dano: 3 }] };
}

test('clonar mantém o tipo, todos os dados e os comportamentos, com outra identidade', () => {
  const original = new InimigoElemental(configuracao());
  const copia = original.clonar();
  assert.notStrictEqual(copia, original);
  assert.ok(copia instanceof InimigoElemental);
  assert.deepEqual(copia.dados(), original.dados());
  assert.equal(typeof copia.receberDano, 'function');
});

test('dano em uma cópia não muda o original nem a outra cópia', () => {
  const original = new InimigoElemental(configuracao());
  const a = original.clonar();
  const b = original.clonar();
  a.receberDano(7);
  assert.equal(a.dados().vida, 17);
  assert.equal(b.dados().vida, 24);
  assert.equal(original.dados().vida, 24);
});

test('cada objeto da lista de ataques é independente, inclusive o segundo ataque', () => {
  const original = new InimigoElemental(configuracao());
  const a = original.clonar();
  const b = original.clonar();
  a.fortalecerAtaque(0, 2);
  a.fortalecerAtaque(1, 4);
  assert.deepEqual(a.dados().ataques.map(a => a.dano), [7, 7]);
  assert.deepEqual(b.dados().ataques.map(a => a.dano), [5, 3]);
  assert.deepEqual(original.dados().ataques.map(a => a.dano), [5, 3]);
});

test('clonar uma instância alterada preserva o estado atual e continua independente', () => {
  const a = new InimigoElemental(configuracao());
  a.receberDano(7);
  a.fortalecerAtaque(0, 2);
  const b = a.clonar();
  assert.deepEqual(b.dados(), a.dados());
  b.receberDano(7);
  b.fortalecerAtaque(0, 2);
  assert.equal(a.dados().vida, 17);
  assert.equal(a.dados().ataques[0].dano, 7);
  assert.equal(b.dados().vida, 10);
  assert.equal(b.dados().ataques[0].dano, 9);
});

test('entrada do construtor e dados de leitura não expõem referências internas', () => {
  const config = configuracao();
  const inimigo = new InimigoElemental(config);
  config.ataques[0].dano = 999;
  config.ataques.push({ nome: 'Outro', dano: 999 });
  const leitura = inimigo.dados();
  leitura.vida = 0;
  leitura.ataques[1].dano = 999;
  assert.deepEqual(inimigo.dados(), { ...configuracao(), vida: 24 });
});

test('catálogo isola o modelo recebido e gera novas instâncias intactas', () => {
  const original = new InimigoElemental(configuracao());
  const catalogo = new CatalogoDeInimigos();
  catalogo.registrar('cogumelo', original);
  original.receberDano(20);
  original.fortalecerAtaque(0, 20);
  const a = catalogo.criar('cogumelo');
  a.receberDano(24);
  a.fortalecerAtaque(1, 20);
  const b = catalogo.criar('cogumelo');
  assert.deepEqual(b.dados(), { ...configuracao(), vida: 24 });
  assert.notStrictEqual(a, b);
});

test('catálogo delega a clonar sem depender da classe concreta InimigoElemental', () => {
  class OutroInimigo extends Inimigo {
    constructor(valor) { super(); this.valor = valor; }
    clonar() { return new OutroInimigo(this.valor); }
  }
  const catalogo = new CatalogoDeInimigos();
  catalogo.registrar('outro', new OutroInimigo(42));
  const a = catalogo.criar('outro');
  a.valor = 0;
  assert.ok(a instanceof OutroInimigo);
  assert.equal(catalogo.criar('outro').valor, 42);
});

test('dano para em zero; clonar um derrotado não ressuscita a instância', () => {
  const inimigo = new InimigoElemental(configuracao());
  inimigo.receberDano(100);
  assert.equal(inimigo.dados().vida, 0);
  assert.equal(inimigo.clonar().dados().vida, 0);
});

test('entradas inválidas não modificam o inimigo', () => {
  const inimigo = new InimigoElemental(configuracao());
  const antes = inimigo.dados();
  for (const valor of [-1, NaN, Infinity, 1.5, '7']) {
    assert.throws(() => inimigo.receberDano(valor), RangeError);
    assert.throws(() => inimigo.fortalecerAtaque(0, valor), RangeError);
  }
  assert.throws(() => inimigo.fortalecerAtaque(9, 2), RangeError);
  assert.throws(() => inimigo.fortalecerAtaque(0, Number.MAX_SAFE_INTEGER), RangeError);
  assert.deepEqual(inimigo.dados(), antes);
});

test('construtor recusa estados inválidos', () => {
  for (const alteracao of [{ vidaMaxima: 0 }, { vida: 25 }, { vida: -1 },
    { nome: '' }, { ataques: [] }, { ataques: [{ nome: 'A', dano: -1 }] }]) {
    assert.throws(() => new InimigoElemental({ ...configuracao(), ...alteracao }));
  }
});

test('catálogo recusa chave ausente, duplicada e protótipo sem contrato', () => {
  const catalogo = criarCatalogo();
  assert.throws(() => catalogo.criar('inexistente'), /desconhecido/);
  assert.throws(() => catalogo.registrar('cogumelo', catalogo.criar('cogumelo')), /registrado/);
  assert.throws(() => catalogo.registrar('', catalogo.criar('cogumelo')), /vazia/);
  assert.throws(() => catalogo.registrar('falso', {}), /inválido/);
  assert.throws(() => new Inimigo().clonar(), /implementar/);
});

test('três modelos da demonstração preservam suas configurações entre criações', () => {
  const catalogo = criarCatalogo();
  for (const [chave, vida] of [['cogumelo', 24], ['golem', 36], ['morcego', 18]]) {
    const a = catalogo.criar(chave);
    a.receberDano(7);
    assert.equal(catalogo.criar(chave).dados().vida, vida);
  }
});
