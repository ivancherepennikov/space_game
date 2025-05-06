#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <random>
#include <algorithm>
#include <fcntl.h>
#include <sys/select.h>
#include <chrono>
#include <thread>


const int width = 41;
const int height = 41;

char getch_nonblock() {
    struct termios oldt, newt;
    char ch = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    return ch;
}

void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
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

        void set_position(int new_x, int new_y) {
            x = new_x;
            y = new_y;
        }        

        void move_ememy(int turn, int level) {
            if (turn % 3 == 0 && isLive) {
                int way = generate_random(1,3);
                if (way == 1){
                    x++;
                    y++;
                }
                if (way == 2){
                    y++;
                }
                if (way == 3){
                    x--;
                    y++;
                }
                if (y >= height - 1) {
                    clear_screen();
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

    void kill_enemy(std::vector<Enemy>& enemies, Boss& boss, bool& bossActive, int& global_counter) {
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
    int k = 0;

    while (k < number_of_enemies) {
        int x = generate_random(10, width - 10);
        int y = generate_random(1, 10);
        bool position_taken = false;

        for (int i = 0; i < enemies.size(); i++) {
            if (x == enemies[i].getX() && y == enemies[i].getY()) {
                position_taken = true;
                break;
            }
        }

        if (!position_taken) {
            enemies.emplace_back(x, y);
            k++;
        }
    }
}

void move_enemies(std::vector<Enemy>& enemies, int turn, int level) {
    if (turn % 3 != 0) return;

    std::vector<std::pair<int, int>> planned_positions;

    for (auto& enemy : enemies) {
        if (!enemy.state()) {
            planned_positions.push_back({enemy.getX(), enemy.getY()});
            continue;
        }

        int new_x = enemy.getX();
        int new_y = enemy.getY() + 1;
        int way = generate_random(1, 5);

        if (way == 1){ 
            new_x++;
        }
        else if (way == 5){
            new_x--;
        }

        new_x = std::max(1, std::min(width - 2, new_x));
        new_y = std::max(1, std::min(height - 2, new_y));

        bool conflict = false;
        for (const auto& pos : planned_positions) {
            if (pos.first == new_x && pos.second == new_y) {
                conflict = true;
                break;
            }
        }

        if (!conflict) {
            enemy.set_position(new_x, new_y);
            planned_positions.emplace_back(new_x, new_y);
        } else {
            planned_positions.emplace_back(enemy.getX(), enemy.getY());
        }

        if (enemy.getY() >= height - 2) {
            clear_screen();
            std::cout << "Поражение! Враг достиг края\nВы проиграли на " << level << " уровне\n";
            exit(0);
        }
    }
}

void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship, int level, Boss& boss, bool bossActive, int& global_counter) {
    clear_screen();
    std::cout << "Level: " << level << " | Boss HP: " << boss.GetHP() <<" | score: " << global_counter << std::endl;

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
    int global_counter = 0;
    Spaceship spaceship;
    std::vector<Bullet> bullets;
    bool bossActive = true;
    Boss boss(20, 5); 
    std::vector<Enemy> enemies;
    generate_enemies(enemies, level);

    draw_field(bullets, enemies, spaceship, level, boss, bossActive, global_counter);

    auto last_update = std::chrono::steady_clock::now();

    while (true) {
        char command = getch_nonblock();
        if (command == 'a' || command == 'A' || command == 'd' || command == 'D') {
            spaceship.move(command);
        }
        if (command == ' ') {
            Bullet new_bullet;
            new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
            bullets.push_back(new_bullet);
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count() >= 200) {
            for (auto& bullet : bullets) {
                bullet.kill_enemy(enemies, boss, bossActive, global_counter);
                bullet.move();
            }

            bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                        [](Bullet& bullet) { return !bullet.isActiveStatus(); }),
                        bullets.end());

            move_enemies(enemies, turn, level);

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
                clear_screen();
                std::cout << "VICTORY! Все враги уничтожены.\n";
                sleep(2);
                std::cout << "Следующий уровень? y/n\n";
                sleep(5);
                char cmd = getch_nonblock();
                if (cmd == 'y') {
                    level++;
                    enemies.clear();
                    generate_enemies(enemies, level);
                    bullets.clear();
                    turn = 0;
                    boss = Boss(20, 5); 
                    bossActive = true;
                    draw_field(bullets, enemies, spaceship, level, boss, bossActive, global_counter);
                    last_update = std::chrono::steady_clock::now();
                    continue;
                } else {
                    clear_screen();
                    exit(0);
                }
            }

            draw_field(bullets, enemies, spaceship, level, boss, bossActive, global_counter);
            turn++;
            last_update = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}