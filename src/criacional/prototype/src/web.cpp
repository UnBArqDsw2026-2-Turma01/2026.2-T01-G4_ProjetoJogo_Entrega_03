#include "demonstracao.hpp"
#include <exception>

namespace {
Demonstracao demonstracao;
std::string resposta;
}

extern "C" {
// Erros voltam como texto. O cliente só desenha dados devolvidos pelo C++.
const char* comando(int acao, int argumento) {
    try {
        demonstracao.executar(static_cast<Comando>(acao), argumento);
        resposta.clear();
    } catch (const std::exception& erro) {
        resposta = erro.what();
    }
    return resposta.c_str();
}

const char* estado() {
    resposta = demonstracao.json();
    return resposta.c_str();
}
}
