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
#include "enemies.h"
#include "bullets.h"
#include "players.h"
#include "game_utils.h"

const int width = 41;
const int height = 41;

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
                std::cout << "🌲";
            } else if (i == spaceship.getY() && j == spaceship.getX()) {
                std::cout << "🛸";
            } else {
                bool drawn = false;

                for (Bullet& bullet : bullets) {
                    if (bullet.isActiveStatus() && bullet.getX() == j && bullet.getY() == i) {
                        std::cout << "💥";
                        drawn = true;
                        break;
                    }
                }

                if (!drawn) {
                    for (Enemy& enemy : enemies) {
                        if (enemy.state() && enemy.getX() == j && enemy.getY() == i) {
                            std::cout << "👾";
                            drawn = true;
                            break;
                        } 

                        else if (enemy.is_destroyed() && !enemy.is_animation_shown() && enemy.getX() <= j && enemy.getX() + 2 >= j && enemy.getY() <= i && enemy.getY() + 2 >= i) {
                            std::cout << "💣";
                            enemy.show_animation();  
                            drawn = true;
                            break;
                        }
                    }
                }

                if (!drawn && bossActive && boss.state() && boss.getX() == j && boss.getY() == i) {
                    std::cout << "👹";
                    drawn = true;
                }

                if (!drawn) std::cout << "  ";
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
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count() >= 150) {
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