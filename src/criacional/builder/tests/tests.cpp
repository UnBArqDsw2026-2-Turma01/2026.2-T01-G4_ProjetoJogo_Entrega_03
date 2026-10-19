#include <bits/stdc++.h>
#include <limits>

#include "../src/character_builder.h"
#include "../src/terminal_menu.h"

using namespace std;

namespace {

int failures = 0;
int checks = 0;

void Check(bool condition, const string& description) {
    ++checks;
    if (condition) {
        cout << "[OK]    " << description << "\n";
    } else {
        ++failures;
        cout << "[FALHA] " << description << "\n";
    }
}

void TestBuildWithAllAttributes() {
    CharacterBuilder builder;
    builder.SetName("Arthas");
    builder.SetType("Guerreiro");
    builder.SetHealthPoints(120);
    builder.SetAttackActions({AttackAction("Golpe de espada", 25), AttackAction("Investida", 40)});
    Character character = builder.Build();

    Check(character.GetName() == string("Arthas"), "Build com todos os atributos define o nome");
    Check(character.GetType() == string("Guerreiro"), "Build com todos os atributos define o tipo");
    Check(character.GetHealthPoints() == 120.0f, "Build com todos os atributos define os pontos de vida");
    Check(character.GetAttackActions().size() == 2, "Build com todos os atributos define as acoes de ataque");
    Check(character.GetAttackActions()[1].GetDamage() == 40.0f, "Acao de ataque guarda o dano");
}

void TestBuildWithSomeAttributes() {
    CharacterBuilder builder;
    builder.SetName("Merlin");
    Character character = builder.Build();

    Check(character.GetName() == string("Merlin"), "Build parcial mantem o atributo incluido");
    Check(!character.GetType().has_value(), "Build parcial deixa o tipo de fora");
    Check(!character.GetHealthPoints().has_value(), "Build parcial deixa os pontos de vida de fora");
    Check(character.GetAttackActions().empty(), "Build parcial deixa as acoes de ataque de fora");
}

void TestBuilderResetsAfterBuild() {
    CharacterBuilder builder;
    builder.SetName("Primeiro");
    builder.SetHealthPoints(50);
    builder.Build();
    Character second = builder.Build();

    Check(!second.GetName().has_value(), "Builder comeca vazio apos o Build");
    Check(!second.GetHealthPoints().has_value(), "Builder descarta os pontos de vida apos o Build");
}

void TestInvalidHealthPoints() {
    CharacterBuilder builder;
    bool thrown = false;
    try {
        builder.SetHealthPoints(0);
    } catch (const invalid_argument&) {
        thrown = true;
    }
    Check(thrown, "SetHealthPoints rejeita valor menor ou igual a zero");
}

void TestCharacterActions() {
    CharacterBuilder builder;
    builder.SetName("Lyra");
    builder.SetHealthPoints(80);
    builder.SetAttackActions({AttackAction("Bola de fogo", 30)});
    Character character = builder.Build();

    ostringstream out;
    character.attack(out);
    character.defend(out);
    Check(out.str().find("Bola de fogo causa 30 de dano") != string::npos, "attack usa as acoes de ataque");
    Check(out.str().find("Lyra se defende com 80 pontos de vida") != string::npos, "defend usa os pontos de vida");

    ostringstream empty_out;
    CharacterBuilder().Build().attack(empty_out);
    Check(empty_out.str().find("nao possui acoes de ataque") != string::npos, "attack sem acoes avisa o jogador");
}

void TestNonFiniteNumbers() {
    CharacterBuilder builder;
    builder.SetHealthPoints(80);
    for (float invalid : {numeric_limits<float>::infinity(), numeric_limits<float>::quiet_NaN()}) {
        bool thrown = false;
        try { builder.SetHealthPoints(invalid); }
        catch (const invalid_argument&) { thrown = true; }
        Check(thrown, "SetHealthPoints rejeita numero nao finito");
    }
    Check(builder.Build().GetHealthPoints() == 80.0f, "Recusa preserva os pontos de vida validos");
    istringstream in("0\n0\n1\ninf\nnan\n70\n1\n1\nGolpe\ninf\n15\n");
    ostringstream out;
    TerminalMenu menu(in, out);
    Character character = menu.CreateCharacter(builder);
    Check(character.GetHealthPoints() == 70.0f, "Menu recusa inf e nan antes dos pontos de vida validos");
    Check(character.GetAttackActions()[0].GetDamage() == 15.0f, "Menu recusa dano infinito");
}

void TestMenuSelectsAttributes() {
    istringstream in("1\nKael\n0\n1\n95\n1\n2\nCorte\n15\nChute\n10\n");
    ostringstream out;
    TerminalMenu menu(in, out);
    CharacterBuilder builder;
    Character character = menu.CreateCharacter(builder);

    Check(character.GetName() == string("Kael"), "Menu inclui o nome quando a opcao e 1");
    Check(!character.GetType().has_value(), "Menu deixa o tipo de fora quando a opcao e 0");
    Check(character.GetHealthPoints() == 95.0f, "Menu inclui os pontos de vida");
    Check(character.GetAttackActions().size() == 2, "Menu inclui a quantidade de acoes pedida");
    Check(character.GetAttackActions()[0].GetName() == "Corte", "Menu guarda o nome da acao");
}

void TestMenuRejectsInvalidInput() {
    istringstream in("2\nsim\n0\n0\n1\n-5\nabc\n70\n0\n");
    ostringstream out;
    TerminalMenu menu(in, out);
    CharacterBuilder builder;
    Character character = menu.CreateCharacter(builder);

    Check(out.str().find("Opcao invalida, digite 0 ou 1.") != string::npos, "Menu rejeita opcao diferente de 0 e 1");
    Check(out.str().find("Digite um numero maior que zero.") != string::npos, "Menu rejeita pontos de vida invalidos");
    Check(!character.GetName().has_value(), "Menu segue apos a entrada invalida");
    Check(character.GetHealthPoints() == 70.0f, "Menu aceita o valor valido depois do erro");
}

void TestMenuRunLoop() {
    istringstream in("1\nAna\n0\n0\n0\n1\n0\n1\nMago\n0\n0\n0\n");
    ostringstream out;
    TerminalMenu menu(in, out);
    CharacterBuilder builder;
    menu.Run(builder);

    Check(out.str().find("Nome: Ana") != string::npos, "Run cria o primeiro personagem");
    Check(out.str().find("Nome: (nao incluido)\nTipo: Mago") != string::npos, "Run cria o segundo personagem sem nome e com tipo");
    Check(out.str().find("Fim.") != string::npos, "Run encerra com o fim da entrada");
}

}

int main() {
    TestBuildWithAllAttributes();
    TestBuildWithSomeAttributes();
    TestBuilderResetsAfterBuild();
    TestInvalidHealthPoints();
    TestCharacterActions();
    TestNonFiniteNumbers();
    TestMenuSelectsAttributes();
    TestMenuRejectsInvalidInput();
    TestMenuRunLoop();

    cout << "\n" << (checks - failures) << "/" << checks << " verificacoes passaram\n";
    return failures == 0 ? 0 : 1;
}
