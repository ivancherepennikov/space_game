#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

const int width = 91;
const int height = 41;

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);  // Сохраняем текущие настройки терминала
    newt = oldt;
    newt.c_lflag &= ~ICANON;  // Отключаем буферизацию ввода
    newt.c_lflag &= ~ECHO;    // Отключаем вывод символов на экран
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Применяем новые настройки
    ch = getchar();  // Чтение символа
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Восстанавливаем старые настройки
    return ch;
}

class Enemy{
    private:
        int x, y;
        bool isLive;

    public:
        Enemy(int x_, int y_) : x(x_), y(y_), isLive(true) {}
        int getX() { return x; }
        int getY() { return y; }
        bool state() { return isLive; }

        void kill(){
            isLive = false;
        }

        void move_ememy(){
            if (isLive) {
                y++;
                if (y >= height - 1) {
                    system("clear");
                    std::cout << "Поражение! Враг достиг края";
                    exit(0); 
                }
            }
        }
        
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

    void kill_enemy(std::vector<Enemy>& enemies){
        for (auto& enemy: enemies){
            if (enemy.getX() == x && enemy.getY() == y && enemy.state()) {
                enemy.kill();
                isActive = false;  
                break;     
            }
        }
    }
    

    void deactivate() {
        isActive = false;
    }
};

class Spaceship { 
private:
    int x;
    int y;

public:
    Spaceship() : x(40), y(37) {}

    void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies) {
        system("clear");
    
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    std::cout << "*";
                } else if (i == y && j == x) {
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
                        }
                    }
    
                    if (!drawn) {
                        std::cout << " ";
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    

    void move(char command) {
        if ((command == 'a' || command == 'A') && x - 1 > 0) {
            x--;
        }
        if ((command == 'd' || command == 'D') && x + 1 < width - 1) {
            x++;
        }
    }

    int getX() { return x; }
    int getY() { return y; }
};

int main() {
    Spaceship spaceship;
    std::vector<Bullet> bullets;

    std::vector<Enemy> enemies = {
        Enemy(39, 8),
        Enemy(40, 3)
    };  
    int number_of_emeies = enemies.size();

    spaceship.draw_field(bullets, enemies);

    while (1) {
        if (enemies.empty()){
            std::cout << "victory";
            return 0;
        }

        char command = getch(); 

        spaceship.move(command);

        if (command == ' ') {
            Bullet new_bullet;
            new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
            bullets.push_back(new_bullet);
        }

        for (auto& bullet : bullets) {
            bullet.move();
            bullet.kill_enemy(enemies);
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
            return !bullet.isActiveStatus();
        }), bullets.end());

        for (auto& enemy : enemies) {
            enemy.move_ememy();
        }

        bool hasLiveEnemies = false;
        for (auto& enemy : enemies) {
            if (enemy.state()) {
                hasLiveEnemies = true;
                break;
            }
        }
        if (!hasLiveEnemies) {
            system("clear");
            std::cout << "VICTORY! Все враги уничтожены.\n";
            return 0;
        }

        spaceship.draw_field(bullets, enemies); 
    }

    return 0;
}

