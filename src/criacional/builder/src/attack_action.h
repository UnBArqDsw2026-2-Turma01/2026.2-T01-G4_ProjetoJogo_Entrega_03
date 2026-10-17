#pragma once

#include <bits/stdc++.h>

using namespace std;

class AttackAction {
public:
    AttackAction(string name, float damage);

    const string& GetName() const;
    float GetDamage() const;

    void attack(ostream& out = cout) const;

private:
    float damage;
    string name;
};
