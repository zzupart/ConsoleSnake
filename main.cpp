#include "graphics.h"
#include "core.h"
#include "console.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <string>

using namespace std::chrono;

int main(){
    system("tput civis");
    system("stty -icanon -echo");
    system("clear");

    std::this_thread::sleep_for(1s);
    GameEngine Game;
    Game.clear_screen();
    Game.print_title(28, 13);
    Game.print_screen();
    std::this_thread::sleep_for(2s);
    bool exitGame = false;
    while(!exitGame){
        int pressedButton = Game.menu();
        switch(pressedButton){
            case 0:
                Game.new_game();
                break;
            case 1:
                Game.show_records();
                break;
            case 2:
                exitGame = true;
                Game.game_exit();
                break;
        }
    }

    std::cout << "\033[66;88f";
    std::cout.flush(); 
    system("clear");
    system("tput cnorm");
    system("stty icanon echo");

    return 0;
}
