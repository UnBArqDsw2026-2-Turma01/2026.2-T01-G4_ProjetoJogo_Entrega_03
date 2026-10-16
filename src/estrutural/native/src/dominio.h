#pragma once

// Classes de domínio da alquimia. Não fazem parte do padrão Facade:
// são os dados que o subsistema manipula.

#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/string.hpp>

namespace alquimia {

using godot::Color;
using godot::String;
using godot::Vector;

struct ElementoQuimico {
	String simbolo;
	String nome;
	int numeroAtomico = 0;
};

enum class CategoriaIngrediente {
	ERVA,
	COGUMELO,
	PARTE_DE_CRIATURA,
	MINERIO,
};

// Abstrata: só existe como Ingrediente ou Pocao (construtor protegido).
class Item {
public:
	virtual ~Item() = default;
	const String &getId() const { return id; }
	const String &getNome() const { return nome; }

protected:
	Item() = default;
	Item(const String &p_id, const String &p_nome) :
			id(p_id), nome(p_nome) {}

	String id;
	String nome;
};

class Ingrediente : public Item {
public:
	Ingrediente() = default;
	Ingrediente(const String &p_id, const String &p_nome, CategoriaIngrediente p_categoria, const Vector<ElementoQuimico> &p_composicao) :
			Item(p_id, p_nome), categoria(p_categoria), composicao(p_composicao) {}

	CategoriaIngrediente getCategoria() const { return categoria; }
	const Vector<ElementoQuimico> &getComposicao() const { return composicao; }

private:
	CategoriaIngrediente categoria = CategoriaIngrediente::ERVA;
	Vector<ElementoQuimico> composicao;
};

class Pocao : public Item {
public:
	Pocao() = default;
	Pocao(const String &p_id, const String &p_nome, const String &p_efeito, const Color &p_cor) :
			Item(p_id, p_nome), efeito(p_efeito), cor(p_cor) {}

	const String &getEfeito() const { return efeito; }
	const Color &getCor() const { return cor; }

private:
	String efeito;
	Color cor;
};

struct Receita {
	String id; // a chave da dupla, ex.: "cogumelo_roxo+erva_lunar"
	String idIngredienteA;
	String idIngredienteB;
	String idPocao;
};

} // namespace alquimia
