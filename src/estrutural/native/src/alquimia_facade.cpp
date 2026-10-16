#include "alquimia_facade.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace alquimia;

static const char *CAMINHO_DADOS = "res://data/alquimia.json";
static const int QUANTIDADE_INICIAL = 5;

void AlquimiaFacade::_ready() {
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	const Dictionary dados = JSON::parse_string(FileAccess::get_file_as_string(CAMINHO_DADOS));
	ERR_FAIL_COND_MSG(dados.is_empty(), String::utf8("Não foi possível ler ") + String(CAMINHO_DADOS));

	catalogo.carregar(dados.get("ingredientes", Array()));
	tabela.carregar(dados.get("receitas", Array()));
	criador.carregar(dados.get("receitas", Array()));
	persistencia.carregar(livro);
	coletarIngredientes(QUANTIDADE_INICIAL);
}

// --- Operações de alto nível ---

Ref<ResultadoMistura> AlquimiaFacade::combinar(const String &idA, const String &idB) {
	if (!catalogo.buscar(idA) || !catalogo.buscar(idB)) {
		return ResultadoMistura::falhou(ResultadoMistura::ITEM_NAO_MISTURAVEL, "Só ingredientes entram na bancada. Poções não se misturam.");
	}
	const Receita *receita = tabela.buscar(idA, idB);
	if (!receita) {
		return ResultadoMistura::falhou(ResultadoMistura::RECEITA_DESCONHECIDA, "Esta dupla não tem receita cadastrada.");
	}
	return executarMistura(*receita);
}

Ref<ResultadoMistura> AlquimiaFacade::refazerReceita(const String &idReceita) {
	if (!livro.conheceReceita(idReceita)) {
		return ResultadoMistura::falhou(ResultadoMistura::RECEITA_DESCONHECIDA, "Essa receita ainda não foi descoberta.");
	}
	const Receita *receita = tabela.buscarPorId(idReceita);
	if (!receita) {
		return ResultadoMistura::falhou(ResultadoMistura::RECEITA_DESCONHECIDA, "Receita inexistente.");
	}
	return executarMistura(*receita);
}

bool AlquimiaFacade::podeRefazer(const String &idReceita) const {
	const Receita *receita = tabela.buscarPorId(idReceita);
	if (!receita || !livro.conheceReceita(idReceita)) {
		return false;
	}
	if (receita->idIngredienteA == receita->idIngredienteB) {
		return inventario.possui(receita->idIngredienteA, 2);
	}
	return inventario.possui(receita->idIngredienteA, 1) && inventario.possui(receita->idIngredienteB, 1);
}

PackedStringArray AlquimiaFacade::listarReceitasDescobertas() const {
	return livro.listarReceitas();
}

Array AlquimiaFacade::consultarComposicao(const String &idIngrediente) const {
	const Ingrediente *ingrediente = catalogo.buscar(idIngrediente);
	if (!ingrediente || !livro.composicaoRevelada(idIngrediente)) {
		return Array(); // ainda oculta
	}
	return elementosDe(*ingrediente);
}

// O coração da fachada: a sequência que a UI não precisa conhecer.
Ref<ResultadoMistura> AlquimiaFacade::executarMistura(const Receita &receita) {
	const String &a = receita.idIngredienteA;
	const String &b = receita.idIngredienteB;

	// 1. Inventário: há ingredientes suficientes? (A+A gasta dois)
	const bool temIngredientes = (a == b)
			? inventario.possui(a, 2)
			: inventario.possui(a, 1) && inventario.possui(b, 1);
	if (!temIngredientes) {
		return ResultadoMistura::falhou(ResultadoMistura::INGREDIENTE_INSUFICIENTE, "Faltam ingredientes na mochila.");
	}

	// 2. Inventário: consome os ingredientes.
	inventario.remover(a, 1);
	inventario.remover(b, 1);

	// 3. CriadorDePocao + Inventário: cria a poção e guarda na mochila.
	const Pocao pocao = criador.criar(receita);
	inventario.adicionar(pocao, 1);

	// 4. LivroDoAventureiro: registra a receita e revela composições.
	const bool receitaNova = livro.registrarReceita(receita.id);
	Array reveladas;
	PackedStringArray ingredientesUsados;
	ingredientesUsados.push_back(a);
	if (b != a) {
		ingredientesUsados.push_back(b);
	}
	for (int i = 0; i < ingredientesUsados.size(); i++) {
		const String id = ingredientesUsados[i];
		if (livro.revelarComposicao(id)) {
			const Ingrediente *ingrediente = catalogo.buscar(id);
			Dictionary d;
			d["id"] = id;
			d["nome"] = ingrediente->getNome();
			d["elementos"] = elementosDe(*ingrediente);
			reveladas.push_back(d);
		}
	}

	// 5. PersistenciaDoLivro: só grava quando algo novo foi aprendido.
	if (receitaNova || !reveladas.is_empty()) {
		persistencia.salvar(livro);
	}

	return ResultadoMistura::sucedeu(receita.id, descreverPocao(pocao), receitaNova, reveladas);
}

// --- Consultas para a tela ---

Array AlquimiaFacade::listarIngredientes() const {
	Array lista;
	for (const KeyValue<String, Ingrediente> &par : catalogo.todos()) {
		Dictionary d;
		d["id"] = par.key;
		d["nome"] = par.value.getNome();
		d["composicao_revelada"] = livro.composicaoRevelada(par.key);
		lista.push_back(d);
	}
	return lista;
}

Array AlquimiaFacade::listarInventario() const {
	Array lista;
	for (const KeyValue<String, int> &par : inventario.itens()) {
		Dictionary d;
		d["id"] = par.key;
		d["quantidade"] = par.value;
		if (const Ingrediente *ingrediente = catalogo.buscar(par.key)) {
			d["tipo"] = "ingrediente";
			d["nome"] = ingrediente->getNome();
		} else if (const Pocao *pocao = criador.modelo(par.key)) {
			d["tipo"] = "pocao";
			d["nome"] = pocao->getNome();
			d["cor"] = pocao->getCor();
			d["efeito"] = pocao->getEfeito();
		}
		lista.push_back(d);
	}
	return lista;
}

Dictionary AlquimiaFacade::descreverReceita(const String &idReceita) const {
	const Receita *receita = tabela.buscarPorId(idReceita);
	if (!receita) {
		return Dictionary();
	}
	Dictionary d;
	d["id"] = receita->id;
	d["ingrediente_a"] = receita->idIngredienteA;
	d["ingrediente_b"] = receita->idIngredienteB;
	d["nome_a"] = catalogo.buscar(receita->idIngredienteA)->getNome();
	d["nome_b"] = catalogo.buscar(receita->idIngredienteB)->getNome();
	if (const Pocao *pocao = criador.modelo(receita->idPocao)) {
		d["pocao"] = descreverPocao(*pocao);
	}
	d["pode_refazer"] = podeRefazer(idReceita);
	return d;
}

// --- Coleta no mundo ---

bool AlquimiaFacade::coletarIngrediente(const String &idIngrediente, int quantidade) {
	const Ingrediente *ingrediente = catalogo.buscar(idIngrediente);
	if (!ingrediente) {
		return false;
	}
	inventario.adicionar(*ingrediente, quantidade);
	return true;
}

// --- Apoio à demonstração ---

void AlquimiaFacade::coletarIngredientes(int quantidade) {
	for (const KeyValue<String, Ingrediente> &par : catalogo.todos()) {
		inventario.adicionar(par.value, quantidade);
	}
}

void AlquimiaFacade::esquecerLivro() {
	livro.esquecer();
	persistencia.salvar(livro);
}

// --- Auxiliares ---

Dictionary AlquimiaFacade::descreverPocao(const Pocao &pocao) const {
	Dictionary d;
	d["id"] = pocao.getId();
	d["nome"] = pocao.getNome();
	d["efeito"] = pocao.getEfeito();
	d["cor"] = pocao.getCor();
	return d;
}

Array AlquimiaFacade::elementosDe(const Ingrediente &ingrediente) const {
	Array elementos;
	for (const ElementoQuimico &e : ingrediente.getComposicao()) {
		Dictionary d;
		d["simbolo"] = e.simbolo;
		d["nome"] = e.nome;
		d["numero"] = e.numeroAtomico;
		elementos.push_back(d);
	}
	return elementos;
}

void AlquimiaFacade::_bind_methods() {
	ClassDB::bind_method(D_METHOD("combinar", "id_a", "id_b"), &AlquimiaFacade::combinar);
	ClassDB::bind_method(D_METHOD("refazer_receita", "id_receita"), &AlquimiaFacade::refazerReceita);
	ClassDB::bind_method(D_METHOD("pode_refazer", "id_receita"), &AlquimiaFacade::podeRefazer);
	ClassDB::bind_method(D_METHOD("listar_receitas_descobertas"), &AlquimiaFacade::listarReceitasDescobertas);
	ClassDB::bind_method(D_METHOD("consultar_composicao", "id_ingrediente"), &AlquimiaFacade::consultarComposicao);
	ClassDB::bind_method(D_METHOD("listar_ingredientes"), &AlquimiaFacade::listarIngredientes);
	ClassDB::bind_method(D_METHOD("listar_inventario"), &AlquimiaFacade::listarInventario);
	ClassDB::bind_method(D_METHOD("descrever_receita", "id_receita"), &AlquimiaFacade::descreverReceita);
	ClassDB::bind_method(D_METHOD("coletar_ingrediente", "id_ingrediente", "quantidade"), &AlquimiaFacade::coletarIngrediente);
	ClassDB::bind_method(D_METHOD("coletar_ingredientes", "quantidade"), &AlquimiaFacade::coletarIngredientes);
	ClassDB::bind_method(D_METHOD("esquecer_livro"), &AlquimiaFacade::esquecerLivro);
}
