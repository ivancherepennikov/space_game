#pragma once
#include <vector>

class Enemy {
protected:
    int x, y;
    bool isLive;
    bool hasAnimationShown;

public:
    Enemy(int x_, int y_);
    int getX();
    int getY();
    bool state();
    virtual void kill();
    void set_position(int new_x, int new_y);
    void move_ememy(int turn, int level);
    void kill_enemy();
    bool is_destroyed();
    bool is_animation_shown();
    void show_animation();
};

class Boss : public Enemy {
private:
    int health;

public:
    Boss(int x_, int y_);
    void take_damage();
    void kill() override;
    int GetHP();
};
