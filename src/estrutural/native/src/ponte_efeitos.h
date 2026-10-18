#pragma once

#include <combatente.hpp>
#include <fabrica_de_efeitos.hpp>

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ponte {

using godot::Array;
using godot::Dictionary;
using godot::PackedStringArray;
using godot::String;

inline String texto(const std::string &s) {
	return String::utf8(s.c_str());
}

inline std::string padrao(const String &s) {
	return std::string(s.utf8().get_data());
}

G4Efeitos::EspecificacaoEfeito especificacao(const Dictionary &efeito, String &erro);
std::vector<G4Efeitos::EspecificacaoEfeito> especificacoes(const Array &efeitos, const String &origem);

std::unique_ptr<G4Efeitos::Combatente> combatente(const Dictionary &dados);

PackedStringArray efeitosAtivos(const G4Efeitos::Combatente &c);

} // namespace ponte
