#pragma once

class Spaceship {
private:
    int x;
    int y;

public:
    Spaceship();
    void move(char command);
    int getX();
    int getY();
};
