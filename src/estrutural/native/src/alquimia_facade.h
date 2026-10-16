#pragma once

#include "resultado_mistura.h"
#include "subsistema.h"

#include <godot_cpp/classes/node.hpp>

using namespace godot;

// Facade (GoF): única porta de entrada da UI para o sistema de alquimia.
// É a única classe da extensão registrada no Godot (junto do ResultadoMistura
// que ela devolve) e roda como Autoload "Alquimia". Por trás dela, as classes
// do subsistema colaboram em uma ordem que a UI não precisa conhecer.
class AlquimiaFacade : public Node {
	GDCLASS(AlquimiaFacade, Node)

public:
	void _ready() override;

	// Operações de alto nível usadas pela mochila.
	Ref<ResultadoMistura> combinar(const String &idA, const String &idB);
	Ref<ResultadoMistura> refazerReceita(const String &idReceita);
	bool podeRefazer(const String &idReceita) const;
	PackedStringArray listarReceitasDescobertas() const;
	Array consultarComposicao(const String &idIngrediente) const;

	// Consultas para desenhar a tela.
	Array listarIngredientes() const;
	Array listarInventario() const;
	Dictionary descreverReceita(const String &idReceita) const;

	// Coleta no mundo.
	bool coletarIngrediente(const String &idIngrediente, int quantidade);

	// Apoio à demonstração.
	void coletarIngredientes(int quantidade);
	void esquecerLivro();

protected:
	static void _bind_methods();

private:
	Ref<ResultadoMistura> executarMistura(const alquimia::Receita &receita);
	Dictionary descreverPocao(const alquimia::Pocao &pocao) const;
	Array elementosDe(const alquimia::Ingrediente &ingrediente) const;

	alquimia::Inventario inventario;
	alquimia::CatalogoDeIngredientes catalogo;
	alquimia::TabelaDeReceitas tabela;
	alquimia::LivroDoAventureiro livro;
	alquimia::CriadorDePocao criador;
	alquimia::PersistenciaDoLivro persistencia;
};
