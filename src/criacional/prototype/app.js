/* Cliente visual. Não reconstrói inimigos: pede cópias ao catálogo ou à instância. */
(() => {
  'use strict';
  const catalogo = criarCatalogo();
  const chaves = ['cogumelo', 'golem', 'morcego'];
  const originais = new Map(chaves.map(chave => [chave, catalogo.criar(chave).dados()]));
  let instancias = [];
  let proximoId = 1;
  const $ = id => document.getElementById(id);
  const limite = 6;

  function elemento(tag, classe, texto) {
    const node = document.createElement(tag);
    if (classe) node.className = classe;
    if (texto !== undefined) node.textContent = texto;
    return node;
  }

  function ficha(dados) {
    const raiz = elemento('div', 'creature');
    const retrato = elemento('div', 'portrait');
    const imagem = elemento('img');
    imagem.src = `assets/${dados.sprite}.png`;
    imagem.alt = ''; // O nome já aparece no título da ficha.
    retrato.append(imagem);
    raiz.append(retrato, elemento('h3', '', dados.nome), elemento('p', 'weakness', `Fraqueza: ${dados.fraqueza}`));
    const legenda = elemento('div', 'health-label', 'Vida');
    legenda.append(elemento('strong', 'vida', `${dados.vida} / ${dados.vidaMaxima}`));
    const barra = elemento('progress');
    barra.max = dados.vidaMaxima;
    barra.value = dados.vida;
    barra.setAttribute('aria-label', `Vida de ${dados.nome}`);
    raiz.append(legenda, barra);
    for (const ataque of dados.ataques) {
      const linha = elemento('div', 'attack', ataque.nome);
      linha.append(elemento('strong', 'dano', `${ataque.dano} de dano`));
      raiz.append(linha);
    }
    return raiz;
  }

  function registrar(mensagem) {
    $('mensagem').textContent = mensagem;
    $('historico').prepend(elemento('li', '', mensagem));
    if ($('historico').children.length > 20) $('historico').lastChild.remove();
  }

  function adicionar(inimigo, chave, origem) {
    if (instancias.length >= limite) return;
    const id = proximoId++;
    instancias.push({ id, inimigo, chave });
    const dados = inimigo.dados();
    registrar(`#${id} criado de ${origem}: ${dados.vida}/${dados.vidaMaxima} de vida e ataque ${dados.ataques[0].dano}. É um novo objeto.`);
    renderizar();
  }

  function renderizar() {
    const foco = document.activeElement.id;
    const chave = $('especie').value;
    $('modelo').replaceChildren(ficha(catalogo.criar(chave).dados()));
    $('instancias').replaceChildren();
    $('contador').textContent = `${instancias.length} / ${limite}`;
    $('vazio').hidden = instancias.length > 0;
    $('clonar').disabled = instancias.length >= limite;

    for (const instancia of instancias) {
      const { id, inimigo, chave } = instancia;
      const dados = inimigo.dados();
      const alterado = JSON.stringify(dados) !== JSON.stringify(originais.get(chave));
      const card = elemento('article', `instance${alterado ? ' changed' : ''}`);
      card.dataset.id = id;
      card.setAttribute('aria-label', `Inimigo ${id}`);
      const identificacao = elemento('div', 'instance-label', `CÓPIA #${id}`);
      identificacao.append(elemento('em', '', alterado ? 'ALTERADA' : 'IGUAL AO MODELO'));
      card.append(identificacao, ficha(dados));
      const botoes = elemento('div', 'actions');
      const acoes = [
        ['dano', '−7 de vida', () => {
          inimigo.receberDano(7);
          registrar(`#${id} recebeu 7 de dano: vida ${inimigo.dados().vida}/${dados.vidaMaxima}. O dano foi aplicado apenas a esta instância.`);
        }],
        ['ataque', '+2 no ataque', () => {
          inimigo.fortalecerAtaque(0, 2);
          registrar(`#${id}: ataque passou de ${dados.ataques[0].dano} para ${inimigo.dados().ataques[0].dano}. A lista de ataques também é independente.`);
        }],
        ['copiar', 'Clonar esta cópia', () => adicionar(inimigo.clonar(), chave, `#${id} (estado atual)`)]
      ];
      for (const [acao, texto, executar] of acoes) {
        const botao = elemento('button', acao === 'copiar' ? 'copy' : '', texto);
        botao.id = `${acao}-${id}`;
        botao.setAttribute('aria-label', `${texto} do inimigo ${id}`);
        botao.disabled = (acao === 'dano' && dados.vida === 0) || (acao === 'copiar' && instancias.length >= limite);
        botao.addEventListener('click', () => { executar(); renderizar(); });
        botoes.append(botao);
      }
      card.append(botoes);
      $('instancias').append(card);
    }

    const preservados = chaves.filter(chave => JSON.stringify(catalogo.criar(chave).dados()) === JSON.stringify(originais.get(chave))).length;
    $('integridade').textContent = `Catálogo preservado: ${preservados} de ${chaves.length} modelos. Conferência feita após cada operação.`;
    if (foco && $(foco) && !$(foco).disabled) $(foco).focus({ preventScroll: true });
  }

  $('clonar').addEventListener('click', () => {
    const chave = $('especie').value;
    adicionar(catalogo.criar(chave), chave, 'modelo do catálogo');
  });
  $('especie').addEventListener('change', renderizar);
  $('reiniciar').addEventListener('click', () => {
    instancias = [];
    proximoId = 1;
    $('especie').value = 'cogumelo';
    $('historico').replaceChildren();
    registrar('Demonstração reiniciada. O catálogo continua com seus três modelos originais.');
    renderizar();
  });
  renderizar();
})();
