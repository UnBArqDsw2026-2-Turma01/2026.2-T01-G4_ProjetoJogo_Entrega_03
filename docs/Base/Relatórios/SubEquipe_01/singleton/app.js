// Apresenta o roteiro gerado pelo executável C++ (roteiro.js). Nenhuma regra da
// Partida é refeita aqui: cada passo mostra exatamente os dados impressos pelo
// programa, inclusive o endereço devolvido por Partida::instancia().
(function () {
  "use strict";

  var passos = window.ROTEIRO || [];
  var legendas = {
    1: "O jogo inicia a partida. A primeira chamada a instancia() cria o objeto; daqui em diante, ninguém mais cria outro.",
    2: "A exploração não recebe a partida por parâmetro: pede a Partida::instancia() e muda a área para Caverna. Mesmo endereço.",
    3: "A bancada de alquimia não conhece a exploração, mas registra a Poção Flamejante na mesma partida, que já está na Caverna.",
    4: "O combate registra a vitória sobre o Cogumelo Andante. Área e poção continuam lá: o progresso é um só.",
    5: "O menu de pausa salva (RF03). O salvamento nº 1 reúne a área, a poção e a vitória vindas de três clientes diferentes.",
    6: "Nova vitória, agora sobre o Gólem de Enxofre. O salvamento nº 1 não muda: ele guarda uma cópia do progresso daquele momento.",
    7: "Salvar e sair: o salvamento nº 2 registra as duas vitórias e a partida passa a encerrada.",
    8: "Com a partida encerrada, a exploração é recusada. O objeto e o endereço são os mesmos; só o estado protege a ação."
  };

  var indice = 0;
  var temporizador = null;
  var $ = function (id) { return document.getElementById(id); };
  var cena = document.querySelector(".cena");
  var fios = document.querySelector(".fios");
  var clientes = Array.prototype.slice.call(document.querySelectorAll(".cliente"));
  var partida = $("partida");

  function definir(id, valor) {
    var el = $(id);
    var texto = String(valor);
    if (el.textContent !== texto) {
      el.textContent = texto;
      el.classList.remove("mudou");
      void el.offsetWidth;
      el.classList.add("mudou");
    }
  }

  function desenharFios(ativo) {
    if (getComputedStyle(fios).display === "none") return;
    var base = cena.getBoundingClientRect();
    var alvo = partida.getBoundingClientRect();
    var html = "";
    clientes.forEach(function (cliente) {
      var r = cliente.getBoundingClientRect();
      var esquerda = r.right < alvo.left;
      var x1 = (esquerda ? r.right : r.left) - base.left;
      var y1 = r.top + r.height / 2 - base.top;
      var x2 = (esquerda ? alvo.left : alvo.right) - base.left;
      var y2 = Math.min(Math.max(y1, alvo.top - base.top + 40), alvo.bottom - base.top - 40);
      var classe = cliente.dataset.cliente === ativo ? ' class="ativo"' : "";
      html += '<line x1="' + x1 + '" y1="' + y1 + '" x2="' + x2 + '" y2="' + y2 + '"' + classe + "/>";
    });
    fios.innerHTML = html;
  }

  function mostrar() {
    var feitos = passos.slice(0, indice);
    var atual = feitos[feitos.length - 1];

    $("passo").textContent = "Passo " + indice + " de " + passos.length;
    $("legenda").textContent = atual ? legendas[atual.passo] || "" :
      "Use ▶ Reproduzir ou as setas do teclado para acompanhar a execução.";
    $("chamadas").textContent = feitos.length;
    $("distintos").textContent = new Set(feitos.map(function (p) { return p.endereco; })).size;

    $("endereco").textContent = atual ? atual.endereco : "—";
    definir("jogador", atual ? atual.jogador : "—");
    definir("area", atual ? atual.area : "—");
    definir("pocoes", atual ? atual.pocoes : 0);
    definir("vitorias", atual ? atual.vitorias : 0);
    definir("estado", atual ? atual.estado : "não iniciada");
    $("fundo").style.backgroundImage =
      'url("assets/' + (atual && atual.area === "Caverna" ? "caverna" : "floresta") + '.png")';

    var lista = $("salvamentos");
    var salvos = atual ? atual.salvamentos : [];
    var anteriores = lista.querySelectorAll("li:not(.vazio)").length;
    lista.innerHTML = salvos.length ? "" : '<li class="vazio">nenhum</li>';
    salvos.forEach(function (s, i) {
      var li = document.createElement("li");
      li.textContent = s.area + " · " + s.pocoes + (s.pocoes === 1 ? " poção" : " poções") + " · " +
        s.vitorias + (s.vitorias === 1 ? " vitória" : " vitórias") + " — " + s.origem;
      if (i >= anteriores) li.className = "novo";
      lista.appendChild(li);
    });

    clientes.forEach(function (cliente) {
      var nome = cliente.dataset.cliente;
      var ultimo = feitos.filter(function (p) { return p.cliente === nome; }).pop();
      cliente.querySelector(".chamada").textContent = ultimo ? ultimo.chamada : "—";
      cliente.classList.toggle("ativo", !!atual && atual.cliente === nome);
      cliente.classList.toggle("recusado", !!atual && atual.cliente === nome && !!atual.erro);
    });
    var combate = feitos.filter(function (p) { return p.cliente === "Combate"; }).pop();
    $("sprite-combate").src = "assets/" + (combate && /Gólem/.test(combate.chamada) ? "golem" : "cogumelo") + ".png";

    var terminal = $("terminal");
    terminal.innerHTML = "";
    feitos.forEach(function (p, i) {
      var bloco = document.createElement("span");
      bloco.textContent = p.saida + "\n";
      if (p.erro) bloco.className = "linha-erro";
      else if (i === feitos.length - 1) bloco.className = "linha-nova";
      terminal.appendChild(bloco);
    });
    terminal.scrollTop = terminal.scrollHeight;

    if (atual) {
      partida.classList.remove("pulso");
      void partida.offsetWidth;
      partida.classList.add("pulso");
    }
    desenharFios(atual && atual.cliente);
    $("anterior").disabled = indice === 0;
    $("proximo").disabled = indice === passos.length;
  }

  function ir(novo) {
    indice = Math.max(0, Math.min(passos.length, novo));
    mostrar();
  }

  function parar() {
    clearInterval(temporizador);
    temporizador = null;
    $("reproduzir").textContent = "▶ Reproduzir";
  }

  function reproduzir() {
    if (temporizador) { parar(); return; }
    if (indice === passos.length) ir(0);
    $("reproduzir").textContent = "❚❚ Pausar";
    ir(indice + 1);
    temporizador = setInterval(function () {
      if (indice >= passos.length) { parar(); return; }
      ir(indice + 1);
    }, Number(new URLSearchParams(location.search).get("intervalo")) || 3200);
  }

  $("anterior").addEventListener("click", function () { parar(); ir(indice - 1); });
  $("proximo").addEventListener("click", function () { parar(); ir(indice + 1); });
  $("reiniciar").addEventListener("click", function () { parar(); ir(0); });
  $("reproduzir").addEventListener("click", reproduzir);
  document.addEventListener("keydown", function (e) {
    if (e.key === "ArrowRight") { parar(); ir(indice + 1); }
    if (e.key === "ArrowLeft") { parar(); ir(indice - 1); }
  });
  window.addEventListener("resize", function () { desenharFios(passos[indice - 1] && passos[indice - 1].cliente); });

  window.singletonDemo = { ir: ir, reproduzir: reproduzir, total: passos.length };
  mostrar();
})();
