#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <random>

const int width = 41;
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

int generate_random(int min, int max) {
    static std::random_device rd;                    
    static std::mt19937 gen(rd());                     
    std::uniform_int_distribution<> distrib(min, max); 

    return distrib(gen);
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

        void move_ememy(int turn, int level){
            if (turn % 3 == 0){
                if (isLive) {
                    y++;
                    if (y >= height - 1) {
                        system("clear");
                        std::cout << "Поражение! Враг достиг края\nВы проиграли на " << level << " уровне";
                        exit(0); 
                    }
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
    Spaceship() : x(19), y(37) {}

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

void generate_enemies(std::vector<Enemy>& enemies, int level){
    int number_of_enemies = level * 5;
    for (int i = 0; i < number_of_enemies; i++){
        int x = generate_random(10, width-10);
        int y = generate_random(1,10);
        Enemy enemy(x, y);
        enemies.push_back(enemy);
    }
}

void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship, int level) {
    system("clear");
    std::cout << "level: " << level << std::endl;

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

int main() {
    int level = 1;
    int turn = 0;
    Spaceship spaceship;
    std::vector<Bullet> bullets;

    std::vector<Enemy> enemies;
    generate_enemies(enemies, level);

    int number_of_emeies = enemies.size();

    draw_field(bullets, enemies, spaceship, level);

    while (1) {
        char command = getch(); 

        spaceship.move(command);

        if (command == ' ') {
            Bullet new_bullet;
            new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
            bullets.push_back(new_bullet);
        }

        for (auto& bullet : bullets) {
            bullet.kill_enemy(enemies); 
            bullet.move();               
        }        

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
            return !bullet.isActiveStatus();
        }), bullets.end());

        for (auto& enemy : enemies) {
            enemy.move_ememy(turn, level);
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
            std::cout << "Следующий уровень? y/n\n?";
            sleep(1);
            char command = getch();
            if (command == 'y'){
                system("clear");
                level++;
                enemies.clear();                 
                generate_enemies(enemies, level); 
                bullets.clear();            
                turn = 0;   
                draw_field(bullets, enemies, spaceship,level);                     
                continue;                        
            }
            else{
                system("clear");
                exit(0);
            }
        }
        

        turn++;

        draw_field(bullets, enemies, spaceship, level); 
    }

    return 0;
}

