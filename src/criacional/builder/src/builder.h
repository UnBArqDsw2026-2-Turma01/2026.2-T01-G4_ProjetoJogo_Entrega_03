#pragma once

#include <bits/stdc++.h>

#include "attack_action.h"
#include "character.h"

using namespace std;

class Builder {
public:
    virtual ~Builder() = default;

    virtual void SetName(const string& name) = 0;
    virtual void SetType(const string& type) = 0;
    virtual void SetHealthPoints(float health_points) = 0;
    virtual void SetAttackActions(const vector<AttackAction>& attack_actions) = 0;
    virtual Character Build() = 0;
};
