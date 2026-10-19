const { chromium } = require(process.env.PLAYWRIGHT_MODULE || 'playwright');
const assert = require('node:assert/strict');
const { resolve } = require('node:path');
const { pathToFileURL } = require('node:url');
const { mkdir } = require('node:fs/promises');

(async () => {
  const gravar = process.argv.includes('--gravar');
  const assets = resolve(__dirname, '../../../../docs/Base/Relatórios/SubEquipe_01/assets/prototype');
  const url = process.env.PROTOTYPE_URL || pathToFileURL(resolve(__dirname, '../../../../docs/Base/Relatórios/SubEquipe_01/prototype/index.html')).href;
  const browser = await chromium.launch();
  try {
    const context = await browser.newContext({ viewport: { width: 1280, height: 900 },
      ...(gravar ? { recordVideo: { dir: '/tmp/prototype-video-v2', size: { width: 1280, height: 900 } } } : { reducedMotion: 'reduce' }) });
    const page = await context.newPage();
    const erros = [];
    page.on('pageerror', error => erros.push(error.message));
    await page.goto(url);
    await page.waitForSelector('html[data-pronto="true"]');
    await page.evaluate(() => document.fonts.ready);
    const pausa = ms => gravar ? page.waitForTimeout(ms) : Promise.resolve();
    const clicar = async (seletor, ms = 3500) => {
      const alvo = page.locator(seletor);
      await alvo.scrollIntoViewIfNeeded();
      const box = await alvo.boundingBox();
      await page.mouse.move(box.x + box.width / 2, box.y + box.height / 2, { steps: gravar ? 16 : 1 });
      if (gravar) await page.waitForTimeout(350);
      await alvo.click();
      await pausa(ms);
    };
    const vida = i => page.locator(`[data-linha="${i}"] td`).nth(2).textContent();
    const ataque = i => page.locator(`[data-linha="${i}"] td`).nth(3).textContent();
    await pausa(3000);
    await clicar('#criar');
    await clicar('#criar');
    await clicar('#copia-0', 1500);
    await clicar('#dano');
    assert.equal(await vida(0), '17 / 24');
    assert.equal(await vida(1), '24 / 24');
    await clicar('#fortalecer');
    assert.equal(await ataque(0), 'Investida: 7');
    assert.equal(await ataque(1), 'Investida: 5');
    await clicar('#criar');
    assert.equal(await vida(2), '24 / 24');
    assert.equal(await page.locator('#modelo-vida').textContent(), '24 / 24');
    await mkdir(assets, { recursive: true });
    await page.screenshot({ path: `${assets}/demonstracao.png`, fullPage: true });
    await clicar('#copia-0', 1500);
    await clicar('#clonar');
    assert.equal(await vida(3), '17 / 24');
    assert.equal(await ataque(3), 'Investida: 7');
    await clicar('#dano');
    assert.equal(await vida(3), '10 / 24');
    assert.equal(await vida(0), '17 / 24');
    assert.ok(await page.locator('#criar').isDisabled());
    assert.ok(await page.locator('#clonar').isDisabled());
    await clicar('summary', 5500);
    const video = page.video();
    await context.close();
    if (gravar) console.log(`Vídeo: ${await video.path()}`);

    const checks = await browser.newContext({ viewport: { width: 390, height: 844 }, reducedMotion: 'reduce' });
    const p = await checks.newPage();
    p.on('pageerror', error => erros.push(error.message));
    await p.goto(url);
    await p.waitForSelector('html[data-pronto="true"]');
    await p.selectOption('#especie', '1'); await p.locator('#criar').click();
    assert.match(await p.locator('#estados').textContent(), /Gólem de Enxofre/);
    await p.selectOption('#especie', '2'); await p.locator('#criar').click();
    assert.match(await p.locator('#estados').textContent(), /Morcego da Caverna/);
    for (let i = 0; i < 3; ++i) await p.locator('#dano').click();
    assert.ok(await p.locator('#dano').isDisabled());
    await p.locator('#clonar').click();
    assert.match(await p.locator('[data-linha="2"]').textContent(), /0 \/ 18/);
    for (const width of [360, 390, 768, 1280]) {
      await p.setViewportSize({ width, height: 900 });
      assert.ok(await p.evaluate(() => document.documentElement.scrollWidth <= innerWidth), `Rolagem horizontal em ${width}`);
    }
    await p.setViewportSize({ width: 390, height: 844 });
    await p.screenshot({ path: '/tmp/prototype-v2-mobile.png', fullPage: true });
    await p.locator('#reiniciar').click();
    assert.equal(await p.locator('[data-copia]').count(), 0);
    await p.locator('#criar').focus(); await p.keyboard.press('Enter');
    assert.equal(await p.locator('[data-copia]').count(), 1);
    const imagens = await p.evaluate(async () => {
      await Promise.all([...document.images].map(i => i.decode()));
      return [...document.images].every(i => i.naturalWidth > 0);
    });
    assert.ok(imagens); assert.deepEqual(erros, []);
    await checks.close();
    console.log('Interface aprovada: clonagem, estado alterado, três modelos, derrota, limite, reinício, teclado e quatro larguras.');
  } finally { await browser.close(); }
})().catch(erro => { console.error(erro); process.exitCode = 1; });
