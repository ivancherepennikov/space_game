#include <iostream>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

class Bullet{
    private:
        int x, y;
        bool isActive;
    public:
        Bullet() {
            x = -1;
            y = -1;
            isActive = false;
        };

        int getX() { return x; }
        int getY() { return y; }
        bool isActiveStatus() { return isActive; }

        void create_bullet(int x_now, int y_now){
            if(!isActive){
                x = x_now;
                y = y_now-1;
                isActive = true;
            }
        }

        void move() {
            if (isActive) {
                y--;  // Пуля двигается вверх по оси Y
                if (y <= 0) {  // Если пуля выходит за верхнюю границу, деактивируем её
                    isActive = false;
                }
            }
        }
};

class Spaceship {
private:
    int x;
    int y;
    const int width = 51;
    const int height = 21;

public:
    Spaceship() : x(23), y(18) {}  // Центрируем по горизонтали и ставим внизу
   

    void draw_field(Bullet& bullet) {
        system("clear");

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    std::cout << "*";
                } else if (i == y && j == x) {
                    std::cout << "^";
                } else if (bullet.getX() == j && bullet.getY() == i) {
                    std::cout << "!";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    void move(char command){
        if ((command == 'w' or command == 'W') and y-1 > 0){
            y--;
        }
        if ((command == 'a' or command == 'A') and x-1 > 0){
            x--;
        }
        if ((command == 's' or command == 'S') and y+1 < height-1){
            y++;
        }
        if ((command == 'd' or command == 'D') and x+1 < width-1){
            x++;
        }
    }

    int getX() { return x; }
    int getY() { return y; }
};



int main() {
    Spaceship spaceship;
    Bullet bullet;
    spaceship.draw_field(bullet); 
    while (1){
        char command;
        command = getchar();
        spaceship.move(command);

        if (command == ' '){
            bullet.create_bullet(spaceship.getX(), spaceship.getY());
        }

        bullet.move();

        spaceship.draw_field(bullet);
    }
    
        
    return 0;
}
