👾 Терминальная космическая стрелялка

Классически 2D шутер в терминале!

🚀 Описание

Игрок управляет космическим кораблем, который может перемещаться влево и вправо и стрелять вверх по врагам. С каждым уровнем количество врагов увеличивается. Появляется БОСС, которого нужно уничтожить несколькими выстрелами.

Проигрыш наступает, если враги достигают нижнего края карты.

🎮 Управление
| Кнопка  | Действие        |
| ------- | --------------- |
| `A`     | Движение влево  |
| `D`     | Движение вправо |
| `Space` | Выстрел         |

🔧 Сборка и запуск
g++ -std=c++17 main.cpp enemies.cpp bullets.cpp players.cpp game_utils.cpp -o spacegame
./spacegame

Примечание: Игра предназначена для запуска в Unix-подобной среде (Linux, macOS).
