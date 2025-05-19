#include "bullets.h"

Bullet::Bullet() : x(-1), y(-1), isActive(false) {}

int Bullet::getX() { return x; }
int Bullet::getY() { return y; }
bool Bullet::isActiveStatus() { return isActive; }

void Bullet::create_bullet(int x_now, int y_now) {
    if (!isActive) {
        x = x_now;
        y = y_now - 1;
        isActive = true;
    }
}

void Bullet::move() {
    if (isActive) {
        y--;
        if (y <= 0) {
            isActive = false;
        }
    }
}

void Bullet::kill_enemy(std::vector<Enemy>& enemies, Boss& boss, bool& bossActive, int& global_counter) {
    for (auto& enemy : enemies) {
        if (enemy.getX() == x && enemy.getY() == y && enemy.state()) {
            enemy.kill_enemy(); 
            isActive = false;  
            global_counter++;
            return;
        }
    }

    if (bossActive && boss.state() && boss.getX() == x && boss.getY() == y) {
        boss.take_damage();
        global_counter++;
        if (!boss.state()) bossActive = false;
        isActive = false;
    } 
}
