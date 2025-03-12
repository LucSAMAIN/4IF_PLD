#pragma once
#include <string>
#include <vector>
#include <map>
#include "type.h"

using namespace std;

// Classe pour les définitions de fonctions 
class DefFonction {
public:
    DefFonction(string name, Type returnType) : name(name), returnType(returnType) {}
    string getName() const { return name; }
    Type getReturnType() const { return returnType; }

private:
    string name;
    Type returnType;
}; 