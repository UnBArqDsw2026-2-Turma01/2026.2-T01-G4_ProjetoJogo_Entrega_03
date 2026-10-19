#include <bits/stdc++.h>

#include "terminal_menu.h"

using namespace std;

TerminalMenu::TerminalMenu(istream& in, ostream& out) : in(in), out(out) {}

string TerminalMenu::ReadLine() {
    string line;
    if (!getline(in, line)) {
        throw runtime_error("Entrada encerrada");
    }
    return line;
}

bool TerminalMenu::AskInclude(const string& attribute) {
    return AskYesNo("Incluir " + attribute + "?");
}

bool TerminalMenu::AskYesNo(const string& question) {
    while (true) {
        out << question << " (1 = sim, 0 = nao): ";
        string answer = ReadLine();
        if (answer == "1") {
            return true;
        }
        if (answer == "0") {
            return false;
        }
        out << "Opcao invalida, digite 0 ou 1.\n";
    }
}

string TerminalMenu::AskText(const string& prompt) {
    while (true) {
        out << prompt;
        string answer = ReadLine();
        if (!answer.empty()) {
            return answer;
        }
        out << "O valor nao pode ser vazio.\n";
    }
}

float TerminalMenu::AskPositiveNumber(const string& prompt) {
    while (true) {
        out << prompt;
        string answer = ReadLine();
        try {
            size_t read = 0;
            float value = stof(answer, &read);
            if (read == answer.size() && value > 0) {
                return value;
            }
        } catch (const logic_error&) {
        }
        out << "Digite um numero maior que zero.\n";
    }
}

int TerminalMenu::AskPositiveInteger(const string& prompt) {
    while (true) {
        out << prompt;
        string answer = ReadLine();
        try {
            size_t read = 0;
            int value = stoi(answer, &read);
            if (read == answer.size() && value > 0) {
                return value;
            }
        } catch (const logic_error&) {
        }
        out << "Digite um numero inteiro maior que zero.\n";
    }
}

Character TerminalMenu::CreateCharacter(Builder& builder) {
    out << "\nAtributos disponiveis: nome, tipo, pontos de vida, acoes de ataque\n";

    if (AskInclude("nome")) {
        builder.SetName(AskText("  Nome: "));
    }
    if (AskInclude("tipo")) {
        builder.SetType(AskText("  Tipo: "));
    }
    if (AskInclude("pontos de vida")) {
        builder.SetHealthPoints(AskPositiveNumber("  Pontos de vida: "));
    }
    if (AskInclude("acoes de ataque")) {
        int count = AskPositiveInteger("  Quantidade de acoes: ");
        vector<AttackAction> actions;
        for (int i = 1; i <= count; ++i) {
            string name = AskText("  Nome da acao " + to_string(i) + ": ");
            float damage = AskPositiveNumber("  Dano da acao " + to_string(i) + ": ");
            actions.emplace_back(name, damage);
        }
        builder.SetAttackActions(actions);
    }

    return builder.Build();
}

void TerminalMenu::Run(Builder& builder) {
    out << "=== Criacao de personagens com o padrao Builder ===\n";
    try {
        do {
            Character character = CreateCharacter(builder);
            out << "\n--- Personagem criado ---\n";
            character.Describe(out);
            out << "\n";
            character.attack(out);
            character.defend(out);
            out << "\n";
        } while (AskYesNo("Criar outro personagem?"));
    } catch (const runtime_error&) {
        out << "\n";
    }
    out << "Fim.\n";
}
