// Dependência opcional de desenvolvimento: playwright. A demonstração não precisa dela.
const { chromium } = require(process.env.PLAYWRIGHT_MODULE || 'playwright');
const assert = require('node:assert/strict');
const { resolve } = require('node:path');
const { pathToFileURL } = require('node:url');
const { mkdir } = require('node:fs/promises');

(async () => {
  const gravar = process.argv.includes('--gravar');
  const saida = resolve(__dirname, '../../../../docs/Base/Relatórios/SubEquipe_01/assets/prototype');
  await mkdir(saida, { recursive: true });
  const browser = await chromium.launch({ headless: true });
  try {
    const context = await browser.newContext({ viewport: { width: 1440, height: 960 },
      ...(gravar ? { recordVideo: { dir: '/tmp/prototype-video', size: { width: 1440, height: 960 } } } : {}) });
    const page = await context.newPage();
    const erros = [];
    page.on('pageerror', error => erros.push(error.message));
    const url = process.env.PROTOTYPE_URL || pathToFileURL(resolve(__dirname, '../../../../docs/Base/Relatórios/SubEquipe_01/prototype/index.html')).href;
    await page.goto(url);
    await page.locator('#clonar').waitFor();
    await page.evaluate(() => Promise.all([...document.images].map(i => i.decode())));
    const pausa = async () => { if (gravar) await page.waitForTimeout(4500); };
    const clicar = async seletor => { await page.locator(seletor).click(); await pausa(); };
    const vida = id => page.locator(`[data-id="${id}"] .vida`).textContent();
    const ataque = id => page.locator(`[data-id="${id}"] .dano`).textContent();
    await pausa();
    await clicar('#clonar');
    await clicar('#clonar');
    assert.equal(await vida(1), '24 / 24');
    assert.equal(await vida(2), '24 / 24');
    await clicar('#dano-1');
    assert.equal(await vida(1), '17 / 24');
    assert.equal(await vida(2), '24 / 24');
    await clicar('#ataque-1');
    assert.equal(await ataque(1), '7 de dano');
    assert.equal(await ataque(2), '5 de dano');
    await clicar('#clonar');
    assert.equal(await vida(3), '24 / 24');
    assert.equal(await ataque(3), '5 de dano');
    assert.equal(await page.locator('#modelo .vida').textContent(), '24 / 24');
    assert.equal(await page.locator('#modelo .dano').textContent(), '5 de dano');
    await page.screenshot({ path: `${saida}/demonstracao.png`, fullPage: true });
    await clicar('#copiar-1');
    assert.equal(await vida(4), '17 / 24');
    assert.equal(await ataque(4), '7 de dano');
    await clicar('#dano-4');
    assert.equal(await vida(4), '10 / 24');
    assert.equal(await vida(1), '17 / 24');
    await page.selectOption('#especie', 'golem');
    await clicar('#clonar');
    assert.equal(await vida(5), '36 / 36');
    await page.selectOption('#especie', 'morcego');
    await clicar('#clonar');
    assert.equal(await vida(6), '18 / 18');
    assert.ok(await page.locator('#clonar').isDisabled());
    assert.ok(await page.locator('#copiar-1').isDisabled());
    assert.match(await page.locator('#integridade').textContent(), /3 de 3/);
    assert.deepEqual(erros, []);
    const video = page.video();
    await context.close();
    if (gravar) console.log(`Vídeo bruto: ${await video.path()}`);
    const verificacao = await browser.newContext({ viewport: { width: 390, height: 844 } });
    const mobile = await verificacao.newPage();
    mobile.on('pageerror', error => erros.push(error.message));
    await mobile.goto(url);
    await mobile.locator('#clonar').click();
    for (let i = 0; i < 4; i++) await mobile.locator('#dano-1').click();
    assert.equal(await mobile.locator('[data-id="1"] .vida').textContent(), '0 / 24');
    assert.ok(await mobile.locator('#dano-1').isDisabled());
    await mobile.locator('#copiar-1').click();
    assert.equal(await mobile.locator('[data-id="2"] .vida').textContent(), '0 / 24');
    for (const width of [360, 390, 768, 1440]) {
      await mobile.setViewportSize({ width, height: 960 });
      assert.ok(await mobile.evaluate(() => document.documentElement.scrollWidth <= innerWidth), `Overflow em ${width}px`);
    }
    await mobile.setViewportSize({ width: 390, height: 844 });
    await mobile.screenshot({ path: '/tmp/prototype-mobile.png', fullPage: true });
    await mobile.locator('#reiniciar').click();
    assert.equal(await mobile.locator('.instance').count(), 0);
    assert.ok(await mobile.locator('#vazio').isVisible());
    await mobile.locator('#clonar').focus();
    await mobile.keyboard.press('Enter');
    assert.equal(await mobile.locator('.instance').count(), 1);
    assert.deepEqual(erros, []);
    await verificacao.close();
    console.log('Interface OK: clones independentes, cópia alterada, três modelos, limite, derrota, reinício, teclado e quatro larguras.');
  } finally {
    await browser.close();
  }
})().catch(error => { console.error(error); process.exitCode = 1; });
