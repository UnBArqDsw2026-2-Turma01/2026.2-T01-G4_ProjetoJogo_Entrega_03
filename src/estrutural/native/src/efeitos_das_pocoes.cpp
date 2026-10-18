#include "efeitos_das_pocoes.h"

#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

namespace alquimia {

void EfeitosDasPocoes::carregar(const Array &receitas) {
	efeitos.clear();
	efeitosVisuais.clear();
	for (int i = 0; i < receitas.size(); i++) {
		const Dictionary receita = receitas[i];
		const Dictionary pocao = receita.get("pocao", Dictionary());
		const String id = pocao.get("id", "");
		efeitos[id] = ponte::especificacoes(pocao.get("efeitos", Array()), id);
		efeitosVisuais[id] = pocao.get("vfx", "");
	}
}

const std::vector<G4Efeitos::EspecificacaoEfeito> *EfeitosDasPocoes::buscar(const String &idPocao) const {
	return efeitos.getptr(idPocao);
}

String EfeitosDasPocoes::vfx(const String &idPocao) const {
	const String *v = efeitosVisuais.getptr(idPocao);
	return v ? *v : String();
}

bool EfeitosDasPocoes::precisaDeInimigo(const String &idPocao) const {
	const auto *lista = buscar(idPocao);
	return lista && G4Efeitos::FabricaDeEfeitos::precisaDeInimigo(*lista);
}

bool EfeitosDasPocoes::precisaEscolherAlvo(const String &idPocao) const {
	const auto *lista = buscar(idPocao);
	return lista && G4Efeitos::FabricaDeEfeitos::precisaEscolherAlvo(*lista);
}

} // namespace alquimia
