#ifndef G4_EFEITOS_TIPOS_HPP
#define G4_EFEITOS_TIPOS_HPP

#include <string>

namespace G4Efeitos {

enum class Atributo {
    FORCA,
    DEFESA,
    DEFESA_MAGICA,
    VELOCIDADE,
    PRECISAO,
    SORTE,
    DANO_MAGICO,
    HP_MAXIMO,
    MP_MAXIMO,
};

enum class Elemento {
    FISICO,
    FOGO,
    FRIO,
    ELETRICO,
    VENENO,
};

enum class Reacao {
    NENHUMA,
    ESQUIVOU,
    REFLETIU,
};

struct Ataque {
    int dano = 0;
    Elemento elemento = Elemento::FISICO;
    bool magico = false;
};

class Duracao {
public:
    enum class Unidade {
        TURNOS,
        SAQUES,
        USO_UNICO,
        PERMANENTE,
    };

    static Duracao turnos(int n) { return Duracao(Unidade::TURNOS, n); }
    static Duracao saques(int n) { return Duracao(Unidade::SAQUES, n); }
    static Duracao usoUnico() { return Duracao(Unidade::USO_UNICO, 1); }
    static Duracao permanente() { return Duracao(Unidade::PERMANENTE, 1); }

    void consumir(Unidade u) {
        if (u == unidade && restante > 0) {
            restante--;
        }
    }
    void esgotar() { restante = 0; }
    bool esgotada() const { return restante <= 0; }

    Unidade getUnidade() const { return unidade; }
    int getRestante() const { return restante; }

private:
    Duracao(Unidade u, int n) : unidade(u), restante(n) {}

    Unidade unidade;
    int restante;
};

inline Atributo atributoDeTexto(const std::string &texto, bool &ok) {
    ok = true;
    if (texto == "forca") return Atributo::FORCA;
    if (texto == "defesa") return Atributo::DEFESA;
    if (texto == "defesa_magica") return Atributo::DEFESA_MAGICA;
    if (texto == "velocidade") return Atributo::VELOCIDADE;
    if (texto == "precisao") return Atributo::PRECISAO;
    if (texto == "sorte") return Atributo::SORTE;
    if (texto == "dano_magico") return Atributo::DANO_MAGICO;
    if (texto == "hp_maximo") return Atributo::HP_MAXIMO;
    if (texto == "mp_maximo") return Atributo::MP_MAXIMO;
    ok = false;
    return Atributo::FORCA;
}

inline Elemento elementoDeTexto(const std::string &texto, bool &ok) {
    ok = true;
    if (texto.empty() || texto == "fisico") return Elemento::FISICO;
    if (texto == "fogo") return Elemento::FOGO;
    if (texto == "frio") return Elemento::FRIO;
    if (texto == "eletrico") return Elemento::ELETRICO;
    if (texto == "veneno") return Elemento::VENENO;
    ok = false;
    return Elemento::FISICO;
}

inline const char *nomeDe(Atributo a) {
    switch (a) {
        case Atributo::FORCA: return "força";
        case Atributo::DEFESA: return "defesa";
        case Atributo::DEFESA_MAGICA: return "defesa mágica";
        case Atributo::VELOCIDADE: return "velocidade";
        case Atributo::PRECISAO: return "precisão";
        case Atributo::SORTE: return "sorte";
        case Atributo::DANO_MAGICO: return "dano mágico";
        case Atributo::HP_MAXIMO: return "HP máximo";
        case Atributo::MP_MAXIMO: return "MP máximo";
    }
    return "?";
}

inline const char *nomeDe(Elemento e) {
    switch (e) {
        case Elemento::FISICO: return "físico";
        case Elemento::FOGO: return "fogo";
        case Elemento::FRIO: return "frio";
        case Elemento::ELETRICO: return "elétrico";
        case Elemento::VENENO: return "veneno";
    }
    return "?";
}

inline std::string textoDeDano(int dano, Elemento e) {
    const std::string n = std::to_string(dano);
    switch (e) {
        case Elemento::FISICO: return n + " de dano";
        case Elemento::ELETRICO: return n + " de dano elétrico";
        default: return n + " de dano de " + nomeDe(e);
    }
}

} // namespace G4Efeitos

#endif // G4_EFEITOS_TIPOS_HPP
