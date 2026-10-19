// Compara a execução nativa com o mesmo C++ compilado para WebAssembly.
const assert = require('node:assert/strict');
const { execFileSync } = require('node:child_process');
const { resolve } = require('node:path');
const CriarPrototype = require('../bin/motor.js');
(async () => {
  const modulo = await CriarPrototype();
  const comando = modulo.cwrap('comando', 'string', ['number', 'number']);
  const estado = modulo.cwrap('estado', 'string', []);
  const nativo = execFileSync(resolve(__dirname, '../bin/prototype'), ['--roteiro'], { encoding: 'utf8' })
    .trim().split('\n').map(JSON.parse);
  const roteiro = [[2,0],[2,0],[3,0],[4,0],[5,0],[6,0],[4,0],[2,0]];
  roteiro.forEach(([acao, argumento], i) => {
    assert.equal(comando(acao, argumento), '');
    assert.deepEqual(JSON.parse(estado()), nativo[i], `Diferença no passo ${i + 1}`);
  });
  const antes = estado();
  assert.notEqual(comando(99, 0), '');
  assert.equal(estado(), antes);
  assert.notEqual(comando(2, 0), '');
  assert.equal(estado(), antes);
  console.log('8 estados idênticos em C++ nativo e WebAssembly; recusas preservam o estado.');
})().catch(erro => { console.error(erro); process.exitCode = 1; });
