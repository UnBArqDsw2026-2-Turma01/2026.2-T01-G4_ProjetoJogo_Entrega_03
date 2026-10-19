// Este arquivo NÃO deve compilar. `make garantia` tenta compilá-lo e só passa
// quando o compilador recusa as duas linhas abaixo: é a prova de que nenhum
// cliente consegue criar uma segunda Partida.
#include "partida.hpp"

int main() {
    Partida outra;                         // construtor privado
    Partida copia = Partida::instancia();  // cópia proibida
}
