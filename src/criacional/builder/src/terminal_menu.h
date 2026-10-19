#pragma once

#include <bits/stdc++.h>

#include "builder.h"
#include "character.h"

using namespace std;

class TerminalMenu {
public:
    TerminalMenu(istream& in, ostream& out);

    Character CreateCharacter(Builder& builder);
    void Run(Builder& builder);

private:
    bool AskInclude(const string& attribute);
    bool AskYesNo(const string& question);
    string AskText(const string& prompt);
    float AskPositiveNumber(const string& prompt);
    int AskPositiveInteger(const string& prompt);
    string ReadLine();

    istream& in;
    ostream& out;
};
