#include <bits/stdc++.h>

#include "character_builder.h"

using namespace std;

void CharacterBuilder::SetName(const string& value) {
    name = value;
}

void CharacterBuilder::SetType(const string& value) {
    type = value;
}

void CharacterBuilder::SetHealthPoints(float value) {
    if (value <= 0) {
        throw invalid_argument("Os pontos de vida devem ser maiores que zero");
    }
    health_points = value;
}

void CharacterBuilder::SetAttackActions(const vector<AttackAction>& value) {
    attack_actions = value;
}

Character CharacterBuilder::Build() {
    Character character(type, name, health_points, attack_actions);
    Reset();
    return character;
}

void CharacterBuilder::Reset() {
    type.reset();
    name.reset();
    health_points.reset();
    attack_actions.clear();
}
