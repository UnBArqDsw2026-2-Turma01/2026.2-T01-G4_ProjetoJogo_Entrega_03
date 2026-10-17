#pragma once

#include <bits/stdc++.h>

#include "builder.h"

using namespace std;

class CharacterBuilder : public Builder {
public:
    void SetName(const string& name) override;
    void SetType(const string& type) override;
    void SetHealthPoints(float health_points) override;
    void SetAttackActions(const vector<AttackAction>& attack_actions) override;
    Character Build() override;

private:
    void Reset();

    optional<string> type;
    optional<string> name;
    optional<float> health_points;
    vector<AttackAction> attack_actions;
};
