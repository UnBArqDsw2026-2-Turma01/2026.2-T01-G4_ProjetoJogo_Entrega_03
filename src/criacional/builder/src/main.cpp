#include <bits/stdc++.h>

#include "character_builder.h"
#include "terminal_menu.h"

using namespace std;

int main() {
    CharacterBuilder builder;
    TerminalMenu menu(cin, cout);
    menu.Run(builder);
    return 0;
}
