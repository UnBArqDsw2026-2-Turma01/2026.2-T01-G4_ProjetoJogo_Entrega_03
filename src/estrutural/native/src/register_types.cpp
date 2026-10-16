#include "register_types.h"

#include "alquimia_facade.h"
#include "jogo/bau.h"
#include "jogo/galinha.h"
#include "jogo/ingrediente_coletavel.h"
#include "jogo/jogador.h"
#include "jogo/mochila_ui.h"
#include "jogo/mundo.h"
#include "resultado_mistura.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_alquimia(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	// Do sistema de alquimia, só a fachada (e o resultado que ela devolve) são
	// visíveis ao Godot. O subsistema não é registrado.
	GDREGISTER_CLASS(ResultadoMistura);
	GDREGISTER_CLASS(AlquimiaFacade);

	// Nós do jogo (Client da fachada). RUNTIME: não executam dentro do editor.
	GDREGISTER_RUNTIME_CLASS(MochilaUI);
	GDREGISTER_RUNTIME_CLASS(Jogador);
	GDREGISTER_RUNTIME_CLASS(IngredienteColetavel);
	GDREGISTER_RUNTIME_CLASS(Galinha);
	GDREGISTER_RUNTIME_CLASS(Bau);
	GDREGISTER_RUNTIME_CLASS(Mundo);
}

void uninitialize_alquimia(ModuleInitializationLevel p_level) {
}

extern "C" {
GDExtensionBool GDE_EXPORT alquimia_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_alquimia);
	init_obj.register_terminator(uninitialize_alquimia);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
	return init_obj.init();
}
}
