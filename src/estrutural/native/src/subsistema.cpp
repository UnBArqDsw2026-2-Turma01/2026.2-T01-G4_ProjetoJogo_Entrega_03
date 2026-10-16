#include "subsistema.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

namespace alquimia {

// --- Inventario ---

bool Inventario::possui(const String &idItem, int p_quantidade) const {
	return quantidade(idItem) >= p_quantidade;
}

void Inventario::remover(const String &idItem, int p_quantidade) {
	const int restante = quantidade(idItem) - p_quantidade;
	if (restante > 0) {
		quantidades[idItem] = restante;
	} else {
		quantidades.erase(idItem);
	}
}

void Inventario::adicionar(const Item &item, int p_quantidade) {
	quantidades[item.getId()] = quantidade(item.getId()) + p_quantidade;
}

int Inventario::quantidade(const String &idItem) const {
	const int *valor = quantidades.getptr(idItem);
	return valor ? *valor : 0;
}

// --- CatalogoDeIngredientes ---

static CategoriaIngrediente categoria_de_texto(const String &texto) {
	if (texto == "COGUMELO") {
		return CategoriaIngrediente::COGUMELO;
	}
	if (texto == "PARTE_DE_CRIATURA") {
		return CategoriaIngrediente::PARTE_DE_CRIATURA;
	}
	if (texto == "MINERIO") {
		return CategoriaIngrediente::MINERIO;
	}
	return CategoriaIngrediente::ERVA;
}

void CatalogoDeIngredientes::carregar(const Array &dados) {
	ingredientes.clear();
	for (int i = 0; i < dados.size(); i++) {
		const Dictionary d = dados[i];
		Vector<ElementoQuimico> composicao;
		const Array elementos = d.get("composicao", Array());
		for (int j = 0; j < elementos.size(); j++) {
			const Dictionary e = elementos[j];
			composicao.push_back({ e.get("simbolo", ""), e.get("nome", ""), int(e.get("numero", 0)) });
		}
		const String id = d.get("id", "");
		ingredientes[id] = Ingrediente(id, d.get("nome", id), categoria_de_texto(d.get("categoria", "")), composicao);
	}
}

const Ingrediente *CatalogoDeIngredientes::buscar(const String &idIngrediente) const {
	return ingredientes.getptr(idIngrediente);
}

// --- TabelaDeReceitas ---

String TabelaDeReceitas::chaveDaDupla(const String &idA, const String &idB) {
	// Ordena os ids para que A+B e B+A caiam na mesma receita.
	const String separador("+");
	return idA < idB ? idA + separador + idB : idB + separador + idA;
}

void TabelaDeReceitas::carregar(const Array &dados) {
	receitas.clear();
	for (int i = 0; i < dados.size(); i++) {
		const Dictionary d = dados[i];
		const Dictionary pocao = d.get("pocao", Dictionary());
		Receita receita;
		receita.idIngredienteA = d.get("a", "");
		receita.idIngredienteB = d.get("b", "");
		receita.id = chaveDaDupla(receita.idIngredienteA, receita.idIngredienteB);
		receita.idPocao = pocao.get("id", "");
		receitas[receita.id] = receita;
	}
}

const Receita *TabelaDeReceitas::buscar(const String &idA, const String &idB) const {
	return buscarPorId(chaveDaDupla(idA, idB));
}

const Receita *TabelaDeReceitas::buscarPorId(const String &idReceita) const {
	return receitas.getptr(idReceita);
}

// --- LivroDoAventureiro ---

bool LivroDoAventureiro::registrarReceita(const String &idReceita) {
	if (conheceReceita(idReceita)) {
		return false;
	}
	receitasDescobertas.push_back(idReceita);
	return true;
}

bool LivroDoAventureiro::conheceReceita(const String &idReceita) const {
	return receitasDescobertas.has(idReceita);
}

bool LivroDoAventureiro::revelarComposicao(const String &idIngrediente) {
	if (composicaoRevelada(idIngrediente)) {
		return false;
	}
	composicoesReveladas.push_back(idIngrediente);
	return true;
}

bool LivroDoAventureiro::composicaoRevelada(const String &idIngrediente) const {
	return composicoesReveladas.has(idIngrediente);
}

Dictionary LivroDoAventureiro::exportar() const {
	Dictionary dados;
	dados["receitas"] = receitasDescobertas;
	dados["composicoes"] = composicoesReveladas;
	return dados;
}

void LivroDoAventureiro::importar(const Dictionary &dados) {
	receitasDescobertas = PackedStringArray(dados.get("receitas", PackedStringArray()));
	composicoesReveladas = PackedStringArray(dados.get("composicoes", PackedStringArray()));
}

void LivroDoAventureiro::esquecer() {
	receitasDescobertas.clear();
	composicoesReveladas.clear();
}

// --- CriadorDePocao ---

void CriadorDePocao::carregar(const Array &dadosReceitas) {
	modelos.clear();
	for (int i = 0; i < dadosReceitas.size(); i++) {
		const Dictionary receita = dadosReceitas[i];
		const Dictionary p = receita.get("pocao", Dictionary());
		const String id = p.get("id", "");
		modelos[id] = Pocao(id, p.get("nome", id), p.get("efeito", ""), Color::html(p.get("cor", "#ffffff")));
	}
}

Pocao CriadorDePocao::criar(const Receita &receita) const {
	const Pocao *m = modelo(receita.idPocao);
	ERR_FAIL_NULL_V_MSG(m, Pocao(), String::utf8("Receita sem poção cadastrada: ") + receita.id);
	return *m;
}

const Pocao *CriadorDePocao::modelo(const String &idPocao) const {
	return modelos.getptr(idPocao);
}

// --- PersistenciaDoLivro ---

Error PersistenciaDoLivro::salvar(const LivroDoAventureiro &livro) const {
	Ref<FileAccess> arquivo = FileAccess::open(caminho, FileAccess::WRITE);
	if (arquivo.is_null()) {
		return FileAccess::get_open_error();
	}
	arquivo->store_string(JSON::stringify(livro.exportar(), "\t"));
	return OK;
}

Error PersistenciaDoLivro::carregar(LivroDoAventureiro &livro) const {
	if (!FileAccess::file_exists(caminho)) {
		return ERR_FILE_NOT_FOUND;
	}
	const Variant dados = JSON::parse_string(FileAccess::get_file_as_string(caminho));
	if (dados.get_type() != Variant::DICTIONARY) {
		return ERR_PARSE_ERROR;
	}
	livro.importar(dados);
	return OK;
}

} // namespace alquimia
