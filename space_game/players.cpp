#include "players.h"

Spaceship::Spaceship() : x(19), y(37) {}

void Spaceship::move(char command) {
    if ((command == 'a' || command == 'A') && x - 1 > 0) x--;
    if ((command == 'd' || command == 'D') && x + 1 < 40) x++; // width - 1
}

int Spaceship::getX() { return x; }
int Spaceship::getY() { return y; }
