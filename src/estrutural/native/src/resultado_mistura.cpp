#include "resultado_mistura.h"

#include <godot_cpp/core/class_db.hpp>

Ref<ResultadoMistura> ResultadoMistura::falhou(MotivoFalha p_falha, const char *p_mensagemUtf8) {
	Ref<ResultadoMistura> r;
	r.instantiate();
	r->falha = p_falha;
	r->mensagem = String::utf8(p_mensagemUtf8);
	return r;
}

Ref<ResultadoMistura> ResultadoMistura::sucedeu(const String &p_idReceita, const Dictionary &p_pocao, bool p_receitaNova, const Array &p_composicoesReveladas) {
	Ref<ResultadoMistura> r;
	r.instantiate();
	r->idReceita = p_idReceita;
	r->pocao = p_pocao;
	r->receitaNova = p_receitaNova;
	r->composicoesReveladas = p_composicoesReveladas;
	r->mensagem = String::utf8(p_receitaNova ? "Receita nova descoberta!" : "Poção criada.");
	return r;
}

void ResultadoMistura::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_sucesso"), &ResultadoMistura::isSucesso);
	ClassDB::bind_method(D_METHOD("get_falha"), &ResultadoMistura::getFalha);
	ClassDB::bind_method(D_METHOD("get_mensagem"), &ResultadoMistura::getMensagem);
	ClassDB::bind_method(D_METHOD("get_id_receita"), &ResultadoMistura::getIdReceita);
	ClassDB::bind_method(D_METHOD("get_pocao"), &ResultadoMistura::getPocao);
	ClassDB::bind_method(D_METHOD("is_receita_nova"), &ResultadoMistura::isReceitaNova);
	ClassDB::bind_method(D_METHOD("get_composicoes_reveladas"), &ResultadoMistura::getComposicoesReveladas);

	BIND_ENUM_CONSTANT(NENHUMA);
	BIND_ENUM_CONSTANT(ITEM_NAO_MISTURAVEL);
	BIND_ENUM_CONSTANT(INGREDIENTE_INSUFICIENTE);
	BIND_ENUM_CONSTANT(RECEITA_DESCONHECIDA);
}
