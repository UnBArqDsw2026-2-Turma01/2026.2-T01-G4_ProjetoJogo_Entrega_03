#include <bits/stdc++.h>

#include "attack_action.h"

using namespace std;

AttackAction::AttackAction(string name, float damage)
    : damage(damage), name(name) {}

const string& AttackAction::GetName() const {
    return name;
}

float AttackAction::GetDamage() const {
    return damage;
}

void AttackAction::attack(ostream& out) const {
    out << "  " << name << " causa " << damage << " de dano\n";
}
