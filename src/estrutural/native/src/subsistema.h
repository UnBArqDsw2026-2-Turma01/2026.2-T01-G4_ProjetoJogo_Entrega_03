#pragma once

// Classes do subsistema de alquimia. Nenhuma delas é registrada no Godot:
// o GDScript só enxerga a AlquimiaFacade. Elas também não sabem que a
// fachada existe, cada uma resolve uma parte do problema.

#include "dominio.h"

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

namespace alquimia {

using godot::Array;
using godot::Dictionary;
using godot::Error;
using godot::HashMap;
using godot::PackedStringArray;

// Quantidades de cada item que o jogador carrega na mochila.
class Inventario {
public:
	bool possui(const String &idItem, int quantidade) const;
	void remover(const String &idItem, int quantidade);
	void adicionar(const Item &item, int quantidade);
	int quantidade(const String &idItem) const;
	const HashMap<String, int> &itens() const { return quantidades; }

private:
	HashMap<String, int> quantidades; // mantém a ordem de inserção
};

// Os 8 ingredientes base e suas composições químicas.
class CatalogoDeIngredientes {
public:
	void carregar(const Array &dados);
	const Ingrediente *buscar(const String &idIngrediente) const;
	const HashMap<String, Ingrediente> &todos() const { return ingredientes; }

private:
	HashMap<String, Ingrediente> ingredientes;
};

// As 36 receitas: toda dupla de ingredientes, sem ordem e com repetição.
class TabelaDeReceitas {
public:
	void carregar(const Array &dados);
	const Receita *buscar(const String &idA, const String &idB) const;
	const Receita *buscarPorId(const String &idReceita) const;

	static String chaveDaDupla(const String &idA, const String &idB);

private:
	HashMap<String, Receita> receitas; // chave: chaveDaDupla(a, b)
};

// O que o jogador já aprendeu: receitas descobertas e composições reveladas.
class LivroDoAventureiro {
public:
	bool registrarReceita(const String &idReceita);
	bool conheceReceita(const String &idReceita) const;
	bool revelarComposicao(const String &idIngrediente);
	bool composicaoRevelada(const String &idIngrediente) const;
	PackedStringArray listarReceitas() const { return receitasDescobertas; }

	Dictionary exportar() const;
	void importar(const Dictionary &dados);
	void esquecer();

private:
	PackedStringArray receitasDescobertas; // ordem de descoberta
	PackedStringArray composicoesReveladas;
};

// Monta a poção resultante a partir dos modelos de cada receita.
class CriadorDePocao {
public:
	void carregar(const Array &dadosReceitas);
	Pocao criar(const Receita &receita) const;
	const Pocao *modelo(const String &idPocao) const;

private:
	HashMap<String, Pocao> modelos;
};

// Grava e lê o Livro do Aventureiro em user:// (no navegador, vira IndexedDB).
class PersistenciaDoLivro {
public:
	explicit PersistenciaDoLivro(const String &p_caminho = "user://livro_aventureiro.sav") :
			caminho(p_caminho) {}

	Error salvar(const LivroDoAventureiro &livro) const;
	Error carregar(LivroDoAventureiro &livro) const;

private:
	String caminho;
};

} // namespace alquimia
