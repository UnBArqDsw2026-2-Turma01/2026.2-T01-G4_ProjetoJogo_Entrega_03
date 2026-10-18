#pragma once

#include "ponte_efeitos.h"

#include <godot_cpp/templates/hash_map.hpp>

namespace alquimia {

using godot::Array;
using godot::HashMap;
using godot::String;

class EfeitosDasPocoes {
public:
	void carregar(const Array &receitas);
	const std::vector<G4Efeitos::EspecificacaoEfeito> *buscar(const String &idPocao) const;
	String vfx(const String &idPocao) const;

	bool precisaDeInimigo(const String &idPocao) const;
	bool precisaEscolherAlvo(const String &idPocao) const;

private:
	HashMap<String, std::vector<G4Efeitos::EspecificacaoEfeito>> efeitos;
	HashMap<String, String> efeitosVisuais;
};

} // namespace alquimia
