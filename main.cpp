#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

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

    void deactivate() {
        isActive = false;
    }
};

class Spaceship {
private:
    int x;
    int y;
    const int width = 91;
    const int height = 41;

public:
    Spaceship() : x(40), y(37) {}

    void draw_field(std::vector<Bullet>& bullets) {
        system("clear");

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    std::cout << "*";
                } else if (i == y && j == x) {
                    std::cout << "^"; 
                } else {
                    bool bullet_drawn = false;
                    for (Bullet& bullet : bullets) {
                        if (bullet.isActiveStatus() && bullet.getX() == j && bullet.getY() == i) {
                            std::cout << "!";
                            bullet_drawn = true;
                            break;
                        }
                    }
                    if (!bullet_drawn) {
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
    spaceship.draw_field(bullets);

    while (1) {
        char command = getch(); 

        spaceship.move(command);

        if (command == ' ') {
            Bullet new_bullet;
            new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
            bullets.push_back(new_bullet);
        }

        for (auto& bullet : bullets) {
            bullet.move();
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
            return !bullet.isActiveStatus();
        }), bullets.end());

        spaceship.draw_field(bullets);
    }

    return 0;
}
