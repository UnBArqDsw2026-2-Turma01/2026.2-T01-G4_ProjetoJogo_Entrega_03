#pragma once

// Utilitários compartilhados pelos nós do jogo (Client do padrão Facade).

#include "alquimia_facade.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>

namespace jogo {

using namespace godot;

// Literais com acento: String(const char*) leria como Latin-1.
inline String u(const char *texto) {
	return String::utf8(texto);
}

inline Ref<Texture2D> textura(const String &caminho) {
	Ref<Texture2D> t = ResourceLoader::get_singleton()->load(caminho);
	return t;
}

inline Ref<Texture2D> icone(const String &id) {
	const String caminho = String("res://assets/icones/") + id + String(".png");
	if (id.is_empty() || !ResourceLoader::get_singleton()->exists(caminho)) {
		return Ref<Texture2D>();
	}
	return textura(caminho);
}

inline Dictionary ler_json(const String &caminho) {
	return JSON::parse_string(FileAccess::get_file_as_string(caminho));
}

// O Client só conhece a fachada, que roda como Autoload "Alquimia".
inline AlquimiaFacade *fachada(const Node *no) {
	return no->get_node<AlquimiaFacade>(NodePath("/root/Alquimia"));
}

inline Color cor_texto() { return Color::html("#3b2a2a"); }
inline Color cor_suave() { return Color::html("#7a6458"); }
inline Color cor_erro() { return Color::html("#a83232"); }
inline Color cor_destaque() { return Color::html("#2f6b3a"); }

} // namespace jogo
