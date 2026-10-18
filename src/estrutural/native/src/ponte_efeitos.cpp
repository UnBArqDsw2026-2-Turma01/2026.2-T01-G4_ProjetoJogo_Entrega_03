#include "ponte_efeitos.h"

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <map>

using namespace godot;
using namespace G4Efeitos;

namespace ponte {

EspecificacaoEfeito especificacao(const Dictionary &efeito, String &erro) {
	std::map<std::string, std::string> campos;
	const Array chaves = efeito.keys();
	for (int i = 0; i < chaves.size(); i++) {
		const Variant valor = efeito[chaves[i]];
		String como_texto = valor.stringify();
		if (valor.get_type() == Variant::FLOAT && double(valor) == double(int64_t(valor))) {
			como_texto = String::num_int64(int64_t(valor));
		}
		campos[padrao(chaves[i])] = padrao(como_texto);
	}
	std::string mensagem;
	EspecificacaoEfeito e = EspecificacaoEfeito::deCampos(campos, mensagem);
	erro = texto(mensagem);
	return e;
}

std::vector<EspecificacaoEfeito> especificacoes(const Array &efeitos, const String &origem) {
	std::vector<EspecificacaoEfeito> lista;
	for (int i = 0; i < efeitos.size(); i++) {
		String erro;
		lista.push_back(especificacao(efeitos[i], erro));
		if (!erro.is_empty()) {
			UtilityFunctions::push_error(origem + String(": ") + erro);
		}
	}
	return lista;
}

std::unique_ptr<Combatente> combatente(const Dictionary &dados) {
	std::map<Atributo, int> base;
	const Dictionary atributos = dados.get("atributos", Dictionary());
	const Array chaves = atributos.keys();
	for (int i = 0; i < chaves.size(); i++) {
		bool ok = false;
		const Atributo a = atributoDeTexto(padrao(chaves[i]), ok);
		if (ok) {
			base[a] = int(atributos[chaves[i]]);
		} else {
			UtilityFunctions::push_error(String::utf8("Atributo desconhecido: ") + String(chaves[i]));
		}
	}
	std::map<Elemento, float> resistencias;
	const Dictionary dadosResistencia = dados.get("resistencias", Dictionary());
	const Array elementos = dadosResistencia.keys();
	for (int i = 0; i < elementos.size(); i++) {
		bool ok = false;
		const Elemento e = elementoDeTexto(padrao(elementos[i]), ok);
		if (ok) {
			resistencias[e] = float(dadosResistencia[elementos[i]]);
		}
	}
	const std::string nome = padrao(dados.get("nome", "?"));
	const float raio = float(dados.get("raio_de_luz", 0.0));
	return std::make_unique<Combatente>(std::make_unique<AtributosBase>(nome, base, resistencias, raio));
}

PackedStringArray efeitosAtivos(const Combatente &c) {
	PackedStringArray icones;
	for (const std::string &icone : c.personagem().efeitosAtivos()) {
		icones.push_back(texto(icone));
	}
	return icones;
}

} // namespace ponte
