/* Apresentação: as operações e o estado dos inimigos pertencem ao módulo C++. */
(async () => {
  const $ = id => document.getElementById(id);
  const criar = (tag, texto, classe) => {
    const node = document.createElement(tag);
    if (texto !== undefined) node.textContent = texto;
    if (classe) node.className = classe;
    return node;
  };
  try {
    const modulo = await CriarPrototype();
    const comando = modulo.cwrap('comando', 'string', ['number', 'number']);
    const estado = modulo.cwrap('estado', 'string', []);
    let atual;
    let quadro = 1;
    const movimentoReduzido = matchMedia('(prefers-reduced-motion: reduce)').matches;

    function executar(acao, argumento = 0) {
      const anterior = atual.selecionada;
      const erro = comando(acao, argumento);
      renderizar();
      if (erro) $('mensagem').textContent = erro;
      else if (acao === 4 || acao === 5) {
        const alvo = document.querySelector(`[data-copia="${anterior}"]`);
        const efeito = criar('span', acao === 4 ? '−7' : '+2', 'efeito');
        alvo.append(efeito);
        setTimeout(() => efeito.remove(), 700);
      }
    }

    function linha(rotulo, dados, classe) {
      const tr = criar('tr', undefined, classe);
      tr.append(criar('td', rotulo), criar('td', dados.nome));
      const vida = criar('td', `${dados.vida} / ${dados.vidaMaxima}`, dados.vida < dados.vidaMaxima ? 'alterado' : '');
      tr.append(vida, criar('td', `${dados.ataques[0].nome}: ${dados.ataques[0].dano}`));
      return tr;
    }

    function renderizar() {
      const foco = document.activeElement.id;
      atual = JSON.parse(estado());
      $('especie').value = ['cogumelo', 'golem', 'morcego'].indexOf(atual.chave);
      $('modelo-sprite').src = `assets/${atual.modelo.sprite}-1.png`;
      $('modelo-vida').textContent = `${atual.modelo.vida} / ${atual.modelo.vidaMaxima}`;
      $('modelo-ataque').textContent = atual.modelo.ataques[0].dano;
      $('modelo-fraqueza').textContent = atual.modelo.fraqueza;
      $('quantidade').textContent = `${atual.copias.length} / 4`;
      $('palco').classList.toggle('caverna', atual.chave === 'morcego');
      $('vazio').hidden = atual.copias.length > 0;
      $('inimigos').replaceChildren();
      $('estados').replaceChildren(linha('Modelo do catálogo', atual.modelo, 'modelo-linha'));
      atual.copias.forEach((inimigo, i) => {
        const button = criar('button', undefined, `inimigo${i === atual.selecionada ? ' selecionado' : ''}${inimigo.vida === 0 ? ' derrotado' : ''}`);
        button.id = `copia-${i}`;
        button.dataset.copia = i;
        button.setAttribute('aria-label', `Selecionar cópia ${i + 1}, ${inimigo.nome}`);
        button.setAttribute('aria-pressed', i === atual.selecionada);
        const img = criar('img');
        img.dataset.sprite = inimigo.sprite;
        img.src = `assets/${inimigo.sprite}-${quadro}.png`;
        img.alt = '';
        const barra = criar('progress');
        barra.max = inimigo.vidaMaxima;
        barra.value = inimigo.vida;
        barra.setAttribute('aria-label', `Vida da cópia ${i + 1}`);
        button.append(criar('span', `Cópia ${i + 1}`), img, criar('span', `${inimigo.vida} / ${inimigo.vidaMaxima}`, 'hp'), barra);
        button.addEventListener('click', () => executar(3, i));
        $('inimigos').append(button);
        const tr = linha(`Cópia ${i + 1}`, inimigo, i === atual.selecionada ? 'ativa' : '');
        tr.dataset.linha = i;
        $('estados').append(tr);
      });
      const alvo = atual.copias[atual.selecionada];
      $('selecionado').textContent = alvo ? `Cópia ${atual.selecionada + 1} · ${alvo.nome}` : 'Selecione um inimigo na cena';
      $('criar').disabled = atual.copias.length === 4;
      $('dano').disabled = !alvo || alvo.vida === 0;
      $('fortalecer').disabled = !alvo;
      $('clonar').disabled = !alvo || atual.copias.length === 4;
      $('mensagem').textContent = atual.mensagem;
      if (foco && $(foco) && !$(foco).disabled) $(foco).focus({ preventScroll: true });
    }

    $('reiniciar').addEventListener('click', () => executar(7));
    $('especie').addEventListener('change', event => executar(1, Number(event.target.value)));
    $('criar').addEventListener('click', () => executar(2));
    $('dano').addEventListener('click', () => executar(4));
    $('fortalecer').addEventListener('click', () => executar(5));
    $('clonar').addEventListener('click', () => executar(6));
    $('reiniciar').disabled = false;
    $('especie').disabled = false;
    renderizar();
    if (!movimentoReduzido) setInterval(() => {
      if (document.hidden) return;
      quadro = quadro % 4 + 1;
      document.querySelectorAll('.inimigo:not(.derrotado) img').forEach(img => {
        img.src = `assets/${img.dataset.sprite}-${quadro}.png`;
      });
    }, 220);
    document.documentElement.dataset.pronto = 'true';
  } catch (erro) {
    $('mensagem').textContent = 'Não foi possível carregar a demonstração. Recarregue a página.';
    console.error(erro);
  }
})();
