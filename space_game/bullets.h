#pragma once
#include "enemies.h"
#include <vector>

class Bullet {
private:
    int x, y;
    bool isActive;

public:
    Bullet();
    int getX();
    int getY();
    bool isActiveStatus();
    void create_bullet(int x_now, int y_now);
    void move();
    void kill_enemy(std::vector<Enemy>& enemies, Boss& boss, bool& bossActive, int& global_counter);
};
