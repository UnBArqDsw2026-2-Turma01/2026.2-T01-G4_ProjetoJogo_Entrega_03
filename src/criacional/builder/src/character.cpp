#include <bits/stdc++.h>

#include "character.h"

using namespace std;

namespace {
const char* kNotIncluded = "(nao incluido)";
}

Character::Character(optional<string> type,
                     optional<string> name,
                     optional<float> health_points,
                     vector<AttackAction> attack_actions)
    : type(type),
      name(name),
      health_points(health_points),
      attack_actions(attack_actions) {}

const optional<string>& Character::GetType() const {
    return type;
}

const optional<string>& Character::GetName() const {
    return name;
}

const optional<float>& Character::GetHealthPoints() const {
    return health_points;
}

const vector<AttackAction>& Character::GetAttackActions() const {
    return attack_actions;
}

string Character::DisplayName() const {
    return name.value_or("Personagem sem nome");
}

void Character::attack(ostream& out) const {
    if (attack_actions.empty()) {
        out << DisplayName() << " nao possui acoes de ataque\n";
        return;
    }
    out << DisplayName() << " ataca:\n";
    for (const AttackAction& action : attack_actions) {
        action.attack(out);
    }
}

void Character::defend(ostream& out) const {
    out << DisplayName() << " se defende";
    if (health_points) {
        out << " com " << *health_points << " pontos de vida";
    }
    out << "\n";
}

void Character::Describe(ostream& out) const {
    out << "Nome: " << name.value_or(kNotIncluded) << "\n";
    out << "Tipo: " << type.value_or(kNotIncluded) << "\n";
    out << "Pontos de vida: ";
    if (health_points) {
        out << *health_points << "\n";
    } else {
        out << kNotIncluded << "\n";
    }
    if (attack_actions.empty()) {
        out << "Acoes de ataque: " << kNotIncluded << "\n";
        return;
    }
    out << "Acoes de ataque:\n";
    for (const AttackAction& action : attack_actions) {
        out << "  - " << action.GetName() << " (" << action.GetDamage() << " de dano)\n";
    }
}
