#ifndef G4_EFEITOS_JSON_MINIMO_HPP
#define G4_EFEITOS_JSON_MINIMO_HPP

#include "fabrica_de_efeitos.hpp"

#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace G4Efeitos::Json {

struct Valor {
    enum class Tipo { NULO, BOOL, NUMERO, TEXTO, LISTA, OBJETO } tipo = Tipo::NULO;
    bool booleano = false;
    double numero = 0.0;
    std::string texto;
    std::vector<Valor> lista;
    std::map<std::string, Valor> objeto;

    const Valor &operator[](const std::string &chave) const {
        static const Valor nulo;
        auto it = objeto.find(chave);
        return it == objeto.end() ? nulo : it->second;
    }
    bool existe() const { return tipo != Tipo::NULO; }

    std::string comoTexto() const {
        if (tipo == Tipo::TEXTO) return texto;
        if (tipo == Tipo::NUMERO) {
            std::ostringstream s;
            s << numero;
            return s.str();
        }
        if (tipo == Tipo::BOOL) return booleano ? "true" : "false";
        return "";
    }
};

class Leitor {
public:
    explicit Leitor(const std::string &p_texto) : t(p_texto) {}

    bool ler(Valor &v) {
        valor(v);
        espacos();
        return ok && i == t.size();
    }

private:
    void espacos() {
        while (i < t.size() && (t[i] == ' ' || t[i] == '\n' || t[i] == '\t' || t[i] == '\r')) i++;
    }

    void valor(Valor &v) {
        espacos();
        if (i >= t.size()) { ok = false; return; }
        const char c = t[i];
        if (c == '{') {
            v.tipo = Valor::Tipo::OBJETO;
            i++;
            espacos();
            if (t[i] == '}') { i++; return; }
            while (ok) {
                espacos();
                std::string chave;
                texto(chave);
                espacos();
                if (t[i++] != ':') { ok = false; return; }
                valor(v.objeto[chave]);
                espacos();
                if (t[i] == ',') { i++; continue; }
                if (t[i] == '}') { i++; return; }
                ok = false;
            }
        } else if (c == '[') {
            v.tipo = Valor::Tipo::LISTA;
            i++;
            espacos();
            if (t[i] == ']') { i++; return; }
            while (ok) {
                v.lista.emplace_back();
                valor(v.lista.back());
                espacos();
                if (t[i] == ',') { i++; continue; }
                if (t[i] == ']') { i++; return; }
                ok = false;
            }
        } else if (c == '"') {
            v.tipo = Valor::Tipo::TEXTO;
            texto(v.texto);
        } else if (t.compare(i, 4, "true") == 0) {
            v.tipo = Valor::Tipo::BOOL; v.booleano = true; i += 4;
        } else if (t.compare(i, 5, "false") == 0) {
            v.tipo = Valor::Tipo::BOOL; i += 5;
        } else if (t.compare(i, 4, "null") == 0) {
            i += 4;
        } else {
            size_t usado = 0;
            v.tipo = Valor::Tipo::NUMERO;
            v.numero = std::stod(t.substr(i, 32), &usado);
            i += usado;
        }
    }

    void texto(std::string &saida) {
        if (t[i++] != '"') { ok = false; return; }
        while (i < t.size() && t[i] != '"') {
            if (t[i] == '\\') {
                i++;
                const char e = t[i++];
                if (e == 'n') saida += '\n';
                else if (e == 't') saida += '\t';
                else if (e == 'u') {
                    const unsigned cp = static_cast<unsigned>(std::stoul(t.substr(i, 4), nullptr, 16));
                    i += 4;
                    if (cp < 0x80) saida += static_cast<char>(cp);
                    else if (cp < 0x800) { saida += static_cast<char>(0xC0 | (cp >> 6)); saida += static_cast<char>(0x80 | (cp & 0x3F)); }
                    else { saida += static_cast<char>(0xE0 | (cp >> 12)); saida += static_cast<char>(0x80 | ((cp >> 6) & 0x3F)); saida += static_cast<char>(0x80 | (cp & 0x3F)); }
                } else saida += e;
            } else {
                saida += t[i++];
            }
        }
        i++;
    }

    const std::string &t;
    size_t i = 0;
    bool ok = true;
};

inline bool lerArquivo(const std::string &caminho, Valor &v) {
    std::ifstream arquivo(caminho);
    if (!arquivo) return false;
    std::stringstream s;
    s << arquivo.rdbuf();
    const std::string conteudo = s.str();
    return Leitor(conteudo).ler(v);
}

inline EspecificacaoEfeito especificacao(const Valor &efeito, std::string &erro) {
    std::map<std::string, std::string> campos;
    for (const auto &[chave, valor] : efeito.objeto) campos[chave] = valor.comoTexto();
    return EspecificacaoEfeito::deCampos(campos, erro);
}

} // namespace G4Efeitos::Json

#endif // G4_EFEITOS_JSON_MINIMO_HPP
