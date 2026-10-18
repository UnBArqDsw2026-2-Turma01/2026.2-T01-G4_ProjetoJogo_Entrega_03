#pragma once

#include "efeitos_das_pocoes.h"
#include "resultado_mistura.h"
#include "subsistema.h"

#include <godot_cpp/classes/node.hpp>

#include <memory>

using namespace godot;

// O que acontece ao usar uma poção: a FabricaDeEfeitos (padrão Decorator)
// decora o alvo ou executa a ação instantânea. Usado pelo combate em C++.
struct UsoDePocao {
	bool sucesso = false;
	String mensagem; // motivo, quando falha
	String nome;
	String vfx;
	Color cor;
	bool revelarMapa = false;
	std::vector<G4Efeitos::FabricaDeEfeitos::Aplicacao> aplicacoes;
};

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

	// Poções em uso (padrão Decorator). O jogador é um Combatente: a cadeia
	// de efeitos ativos fica guardada aqui, atrás da fachada, como o inventário.
	UsoDePocao usarPocao(const String &idPocao, G4Efeitos::Combatente *alvo, const std::vector<G4Efeitos::Combatente *> &inimigos);
	G4Efeitos::Combatente &getJogador() { return *jogador; }
	bool precisaDeInimigo(const String &idPocao) const;
	bool precisaEscolherAlvo(const String &idPocao) const;

	// As mesmas operações para a interface em GDScript e para o mundo.
	Dictionary usarPocaoForaDeCombate(const String &idPocao);
	Dictionary descreverJogador() const;
	int bonusDeSorte() const;
	void registrarSaque();
	void apagarLuzes();
	void restaurarJogador();
	void descansar(); // fora do combate: +1 HP e +1 MP

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
	alquimia::EfeitosDasPocoes efeitos;
	G4Efeitos::FabricaDeEfeitos fabrica;
	std::unique_ptr<G4Efeitos::Combatente> jogador;
};
