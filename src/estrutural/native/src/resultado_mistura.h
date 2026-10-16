#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

using namespace godot;

// O que a fachada devolve à UI depois de uma mistura.
// É RefCounted para que o GDScript possa ler o resultado.
class ResultadoMistura : public RefCounted {
	GDCLASS(ResultadoMistura, RefCounted)

public:
	enum MotivoFalha {
		NENHUMA,
		ITEM_NAO_MISTURAVEL,
		INGREDIENTE_INSUFICIENTE,
		RECEITA_DESCONHECIDA,
	};

	// Mensagens em C++ são UTF-8; String(const char*) leria como Latin-1.
	static Ref<ResultadoMistura> falhou(MotivoFalha p_falha, const char *p_mensagemUtf8);
	static Ref<ResultadoMistura> sucedeu(const String &p_idReceita, const Dictionary &p_pocao, bool p_receitaNova, const Array &p_composicoesReveladas);

	bool isSucesso() const { return falha == NENHUMA; }
	MotivoFalha getFalha() const { return falha; }
	String getMensagem() const { return mensagem; }
	String getIdReceita() const { return idReceita; }
	Dictionary getPocao() const { return pocao; }
	bool isReceitaNova() const { return receitaNova; }
	Array getComposicoesReveladas() const { return composicoesReveladas; }

protected:
	static void _bind_methods();

private:
	MotivoFalha falha = NENHUMA;
	String mensagem;
	String idReceita;
	Dictionary pocao; // { id, nome, efeito, cor }
	bool receitaNova = false;
	Array composicoesReveladas; // [{ id, nome, elementos: [{ simbolo, nome, numero }] }]
};

VARIANT_ENUM_CAST(ResultadoMistura::MotivoFalha);
