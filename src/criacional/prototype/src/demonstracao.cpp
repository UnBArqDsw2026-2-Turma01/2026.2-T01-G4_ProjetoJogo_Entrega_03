#include "demonstracao.hpp"

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
std::string textoJson(const std::string& texto) {
    std::ostringstream out;
    out << '"';
    for (unsigned char c : texto) {
        if (c == '"' || c == '\\') out << '\\' << c;
        else if (c < 0x20) out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
        else out << c;
    }
    out << '"';
    return out.str();
}

void escreverInimigo(std::ostream& out, const Inimigo& inimigo) {
    out << "{\"nome\":" << textoJson(inimigo.nome()) << ",\"sprite\":" << textoJson(inimigo.sprite())
        << ",\"fraqueza\":" << textoJson(inimigo.fraqueza()) << ",\"vida\":" << inimigo.vida()
        << ",\"vidaMaxima\":" << inimigo.vidaMaxima() << ",\"ataques\":[";
    bool primeiro = true;
    for (const auto& ataque : inimigo.ataques()) {
        if (!primeiro) out << ',';
        out << "{\"nome\":" << textoJson(ataque.nome) << ",\"dano\":" << ataque.dano << '}';
        primeiro = false;
    }
    out << "]}";
}
}

void Demonstracao::executar(Comando comando, int argumento) {
    static const std::array<std::string, 3> chaves = {"cogumelo", "golem", "morcego"};
    switch (comando) {
    case Comando::Modelo:
        chave_ = chaves.at(static_cast<std::size_t>(argumento));
        mensagem_ = "Modelo selecionado: " + catalogo_.modelo(chave_).nome() + ".";
        return;
    case Comando::Reiniciar:
        copias_.clear();
        selecionada_ = -1;
        chave_ = "cogumelo";
        mensagem_ = "Cena reiniciada. Os modelos do catálogo permanecem preservados.";
        return;
    case Comando::Criar:
        if (copias_.size() >= 4) throw std::length_error("A cena comporta quatro inimigos. Reinicie para criar outra composição.");
        copias_.push_back(catalogo_.criar(chave_));
        selecionada_ = static_cast<int>(copias_.size()) - 1;
        mensagem_ = "Cópia " + std::to_string(selecionada_ + 1) + " criada do modelo " + catalogo_.modelo(chave_).nome() + ".";
        return;
    case Comando::Selecionar:
        (void)copias_.at(static_cast<std::size_t>(argumento));
        selecionada_ = argumento;
        mensagem_ = "Cópia " + std::to_string(selecionada_ + 1) + " selecionada.";
        return;
    case Comando::Dano:
    case Comando::Fortalecer:
    case Comando::ClonarCopia:
        break;
    default:
        throw std::invalid_argument("Comando desconhecido.");
    }
    if (selecionada_ < 0) throw std::logic_error("Selecione um inimigo na cena.");
    auto& inimigo = *copias_.at(static_cast<std::size_t>(selecionada_));
    const std::string rotulo = "Cópia " + std::to_string(selecionada_ + 1);
    if (comando == Comando::Dano) {
        inimigo.receberDano(7);
        mensagem_ = rotulo + ": " + std::to_string(inimigo.vida()) + " de vida. As outras instâncias mantêm seus valores.";
    } else if (comando == Comando::Fortalecer) {
        inimigo.fortalecerAtaque(0, 2);
        mensagem_ = rotulo + ": ataque " + std::to_string(inimigo.ataques().at(0).dano) + ". A alteração é somente desta instância.";
    } else {
        if (copias_.size() >= 4) throw std::length_error("A cena comporta quatro inimigos. Reinicie para criar outra composição.");
        copias_.push_back(inimigo.clonar());
        selecionada_ = static_cast<int>(copias_.size()) - 1;
        mensagem_ = "Cópia " + std::to_string(selecionada_ + 1) + " clonada da " + rotulo + ", preservando seu estado atual.";
    }
}

std::string Demonstracao::json() const {
    std::ostringstream out;
    out << "{\"chave\":" << textoJson(chave_) << ",\"selecionada\":" << selecionada_
        << ",\"mensagem\":" << textoJson(mensagem_) << ",\"modelo\":";
    escreverInimigo(out, catalogo_.modelo(chave_));
    out << ",\"copias\":[";
    for (std::size_t i = 0; i < copias_.size(); ++i) {
        if (i) out << ',';
        escreverInimigo(out, *copias_[i]);
    }
    out << "]}";
    return out.str();
}

void Demonstracao::imprimir(std::ostream& saida) const {
    auto mostrar = [&](const Inimigo& inimigo) {
        saida << inimigo.nome() << " | vida " << inimigo.vida() << '/' << inimigo.vidaMaxima()
              << " | " << inimigo.ataques().front().nome << ": " << inimigo.ataques().front().dano << '\n';
    };
    saida << "\nModelo: ";
    mostrar(catalogo_.modelo(chave_));
    for (std::size_t i = 0; i < copias_.size(); ++i) {
        saida << (static_cast<int>(i) == selecionada_ ? "> " : "  ") << "Cópia " << i + 1 << ": ";
        mostrar(*copias_[i]);
    }
    saida << mensagem_ << "\n\n";
}
