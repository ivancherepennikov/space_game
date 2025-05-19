#include "enemies.h"
#include "game_utils.h"
#include <iostream>
#include <cstdlib>

Enemy::Enemy(int x_, int y_) : x(x_), y(y_), isLive(true), hasAnimationShown(false) {}

int Enemy::getX() { return x; }
int Enemy::getY() { return y; }
bool Enemy::state() { return isLive; }

void Enemy::kill() {
    isLive = false;
    hasAnimationShown = false;
}

void Enemy::set_position(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Enemy::move_ememy(int turn, int level) {
    if (turn % 2 == 0 && isLive) {
        int way = generate_random(1, 5);
        if (way == 1) {
            x++;
            y++;
        } else if (way == 5) {
            x--;
            y++;
        }
        if (y >= 40) {  
            clear_screen();
            std::cout << "Поражение! Враг достиг края\nВы проиграли на " << level << " уровне\n";
            exit(0);
        }
    }
}

void Enemy::kill_enemy() {
    isLive = false;
    hasAnimationShown = false;
}

bool Enemy::is_destroyed() { return !isLive; }
bool Enemy::is_animation_shown() { return hasAnimationShown; }
void Enemy::show_animation() { hasAnimationShown = true; }

Boss::Boss(int x_, int y_) : Enemy(x_, y_), health(3) {}

void Boss::take_damage() {
    health--;
    if (health <= 0) {
        kill();
    }
}

void Boss::kill() {
    isLive = false;
}

int Boss::GetHP() { return health; }
