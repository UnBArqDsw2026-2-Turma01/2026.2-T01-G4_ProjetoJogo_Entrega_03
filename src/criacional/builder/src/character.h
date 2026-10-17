#pragma once

#include <bits/stdc++.h>

#include "attack_action.h"

using namespace std;

class Character {
public:
    Character(optional<string> type,
              optional<string> name,
              optional<float> health_points,
              vector<AttackAction> attack_actions);

    const optional<string>& GetType() const;
    const optional<string>& GetName() const;
    const optional<float>& GetHealthPoints() const;
    const vector<AttackAction>& GetAttackActions() const;

    void attack(ostream& out = cout) const;
    void defend(ostream& out = cout) const;
    void Describe(ostream& out = cout) const;

private:
    string DisplayName() const;

    optional<string> type;
    optional<string> name;
    optional<float> health_points;
    vector<AttackAction> attack_actions;
};
