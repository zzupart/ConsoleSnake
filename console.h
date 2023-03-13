#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string>

int getch(){
    int ch;
    ch = getchar();
    if(ch == 27){
    ch = getchar();
    ch = getchar();
    }

    return ch;
}

void GoToXY(int x, int y){ 
    std::cout << "\033[" << x << ";" << y << "f";
}

#endif
