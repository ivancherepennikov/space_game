#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <random>
#include <algorithm>

const int width = 41;
const int height = 41;

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int generate_random(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

class Enemy {
    protected:
        int x, y;
        bool isLive;
        bool hasAnimationShown;
    
    public:
        Enemy(int x_, int y_) : x(x_), y(y_), isLive(true), hasAnimationShown(false) {}
    
        int getX() { return x; }
        int getY() { return y; }
        bool state() { return isLive; }
    
        virtual void kill() {
            isLive = false;
            hasAnimationShown = false; 
        }
    
        void move_ememy(int turn, int level) {
            if (turn % 3 == 0 && isLive) {
                y++;
                if (y >= height - 1) {
                    system("clear");
                    std::cout << "Поражение! Враг достиг края\nВы проиграли на " << level << " уровне\n";
                    exit(0);
                }
            }
        }
    
        void kill_enemy() {
            isLive = false;
            hasAnimationShown = false; 
        }
    
        bool is_destroyed() { return !isLive; } 
        bool is_animation_shown() { return hasAnimationShown; } 
        void show_animation() { hasAnimationShown = true; }
    };    

class Boss : public Enemy {
private:
    int health;

public:
    Boss(int x_, int y_) : Enemy(x_, y_), health(3) {}

    void take_damage() {
        health--;
        if (health <= 0) {
            kill();
        }
    }

    void kill() override {
        isLive = false;
    }

    int GetHP() { return health; }
};

class Bullet {
private:
    int x, y;
    bool isActive;

public:
    Bullet() : x(-1), y(-1), isActive(false) {}

    int getX() { return x; }
    int getY() { return y; }
    bool isActiveStatus() { return isActive; }

    void create_bullet(int x_now, int y_now) {
        if (!isActive) {
            x = x_now;
            y = y_now - 1;
            isActive = true;
        }
    }

    void move() {
        if (isActive) {
            y--;
            if (y <= 0) {
                isActive = false;
            }
        }
    }

    void kill_enemy(std::vector<Enemy>& enemies, Boss& boss, bool& bossActive) {
        for (auto& enemy : enemies) {
            if (enemy.getX() == x && enemy.getY() == y && enemy.state()) {
                enemy.kill_enemy(); 
                isActive = false;  
                return;
            }
        }
    
        if (bossActive && boss.state() && boss.getX() == x && boss.getY() == y) {
            boss.take_damage();
            if (!boss.state()) bossActive = false;
            isActive = false;
        } 
    }
};

class Spaceship {
private:
    int x;
    int y;

public:
    Spaceship() : x(19), y(37) {}

    void move(char command) {
        if ((command == 'a' || command == 'A') && x - 1 > 0) x--;
        if ((command == 'd' || command == 'D') && x + 1 < width - 1) x++;
    }

    int getX() { return x; }
    int getY() { return y; }
};

void generate_enemies(std::vector<Enemy>& enemies, int level) {
    int number_of_enemies = level * 5 - 1;
    for (int i = 0; i < number_of_enemies; i++) {
        int x = generate_random(10, width - 10);
        int y = generate_random(1, 10);
        enemies.emplace_back(x, y);
    }
}

void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship, int level, Boss& boss, bool bossActive) {
    system("clear");
    std::cout << "Level: " << level << " | Boss HP: " << boss.GetHP() << std::endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                std::cout << "*";
            } else if (i == spaceship.getY() && j == spaceship.getX()) {
                std::cout << "^";
            } else {
                bool drawn = false;

                for (Bullet& bullet : bullets) {
                    if (bullet.isActiveStatus() && bullet.getX() == j && bullet.getY() == i) {
                        std::cout << "!";
                        drawn = true;
                        break;
                    }
                }

                if (!drawn) {
                    for (Enemy& enemy : enemies) {
                        if (enemy.state() && enemy.getX() == j && enemy.getY() == i) {
                            std::cout << "@";
                            drawn = true;
                            break;
                        } 

                        else if (enemy.is_destroyed() && !enemy.is_animation_shown() && enemy.getX() <= j && enemy.getX() + 2 >= j && enemy.getY() <= i && enemy.getY() + 2 >= i) {
                            std::cout << "#";
                            enemy.show_animation();  
                            drawn = true;
                            break;
                        }
                    }
                }

                if (!drawn && bossActive && boss.state() && boss.getX() == j && boss.getY() == i) {
                    std::cout << "0";
                    drawn = true;
                }

                if (!drawn) std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    int level = 1;
    int turn = 0;
    Spaceship spaceship;
    std::vector<Bullet> bullets;
    bool bossActive = true;
    Boss boss(20, 5); 

    std::vector<Enemy> enemies;
    generate_enemies(enemies, level);

    draw_field(bullets, enemies, spaceship, level, boss, bossActive);

    while (true) {
        char command = getch();

        spaceship.move(command);

        if (command == ' ') {
            Bullet new_bullet;
            new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
            bullets.push_back(new_bullet);
        }

        for (auto& bullet : bullets) {
            bullet.kill_enemy(enemies, boss, bossActive);
            bullet.move();
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                     [](Bullet& bullet) { return !bullet.isActiveStatus(); }),
                      bullets.end());

        for (auto& enemy : enemies) {
            enemy.move_ememy(turn, level);
        }

        if (bossActive && boss.state()) {
            boss.move_ememy(turn, level);
        }

        bool hasLiveEnemies = false;
        for (auto& enemy : enemies) {
            if (enemy.state()) {
                hasLiveEnemies = true;
                break;
            }
        }
        if (bossActive && boss.state()) hasLiveEnemies = true;

        if (!hasLiveEnemies) {
            system("clear");
            std::cout << "VICTORY! Все враги уничтожены.\n";
            std::cout << "Следующий уровень? y/n\n?";
            sleep(1);
            char cmd = getch();
            if (cmd == 'y') {
                level++;
                enemies.clear();
                generate_enemies(enemies, level);
                bullets.clear();
                turn = 0;
                boss = Boss(20, 5); 
                bossActive = true;
                draw_field(bullets, enemies, spaceship, level, boss, bossActive);
                continue;
            } else {
                system("clear");
                exit(0);
            }
        }

        turn++;
        draw_field(bullets, enemies, spaceship, level, boss, bossActive);
    }

    return 0;
}
