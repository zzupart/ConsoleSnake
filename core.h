#ifndef CORE_H
#define CORE_H

#include "console.h"
#include "graphics.h"
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <mutex>

using namespace std::chrono;

const char* recordsSavingPath = "records.bin";
bool stopControls = false;
std::mutex controlsMutex;

class SnakePart{
    public:
        int x, y;
};

class Record{
    public:
        int score;
        std::string diff;
        std::string time;
        Record(){}
        Record(int a_score, int a_diff){
            auto now = std::chrono::system_clock::now();
            std::time_t end_time = std::chrono::system_clock::to_time_t(now);
            time = std::ctime(&end_time);
            time.erase(0, 4);
            time.erase(12, 3);
            score = a_score;
            switch(a_diff){
                case 0:
                    diff = "LOW";
                    break;
                case 1:
                    diff = "MEDIUM";
                    break;
                case 2:
                    diff = "HIGH";
                    break;
            }
        }
        bool operator<(const Record& a) const{
            return score > a.score;
        }
};

class GameEngine{
    public:
        char screen[HEIGHT][WIDTH];
 
        void clear_screen(){
            for(int i = 0; i < HEIGHT; i++){
                for(int j = 0; j < WIDTH; j++){
                    screen[i][j] = ' ';
                }
            }
        }
        void print_screen(){
            GoToXY(0, 0);
            for(int i = 0; i < HEIGHT; i++){
                for(int j = 0; j < WIDTH; j++){
                    GoToXY(i, j);
                    std::cout << screen[i][j];
                }
                //std::cout << std::endl;
            }
            std::cout.flush();
        }
        void print_title(int y, int x){
            for(int i = 0; i < 11; i++){
                for(int j = 0; j < 63; j++){
                    screen[y + i][x + j] = gameTitle[i][j];
                }
            }
        }
        int menu(){
            int activeMenuButton = 0;

            while(true){
                clear_screen();
                print_title(4, 13);
                for(int i = 0; i < 88; i++){
                    screen[20][i] = '=';
                    screen[62][i] = '=';
                }
                for(int i = 21; i < 62; i++){
                    screen[i][18] = '|';
                    screen[i][19] = '|';
                    screen[i][70] = '|';
                    screen[i][71] = '|';
                }
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 16; j++){
                        screen[25 + i][37 + j] = menuButton1[i][j];
                        screen[40 + i][37 + j] = menuButton2[i][j];
                        screen[55 + i][37 + j] = menuButton3[i][j];
                    }
                    for(int j = 0; j < 4; j++){
                        screen[activeMenuButton * 15 + 25 + i][30 + j] = menuPointer[i][j];
                    }
                }
                print_screen();

                int key = getch();
                switch(key){
                    case 65:
                        activeMenuButton--;
                        break;
                    case 66:
                        activeMenuButton++;
                }
                if(key == 72) break;
                if(activeMenuButton < 0) activeMenuButton = 2;
                if(activeMenuButton > 2) activeMenuButton = 0;
            }
            return activeMenuButton;
        }
    void game_exit(){
        clear_screen();
        print_title(28, 13);
      
        for(int i = 0; i < 10; i++){
            screen[65][38 + i] = credit[i];
        }
        print_screen();
        std::this_thread::sleep_for(1s);
    }
    void show_records(){
        std::ifstream recordsin;
        recordsin.open(recordsSavingPath, std::ios_base::binary);
        recordsin.unsetf(std::ios::skipws);
        clear_screen();
        print_screen();
        if(recordsin.peek() != EOF){
            Record f_rec;
            std::vector<Record> f_records;
            while(recordsin.read((char*)&f_rec, sizeof(f_rec)))
                f_records.push_back(f_rec);
            recordsin.close();
            bool exit = false;
            int activeMenuButton = 1;
            while(true){
                clear_screen();
                for(int i = 0; i < 7; i++) screen[1][41 + i] = recordsLabel[i];
                for(int i = 0; i < 5; i++) screen[5][14 + i] = scoreLabel[i];
                for(int i = 0; i < 10; i++) screen[5][27 + i] = difficultyLabel[i];
                for(int i = 0; i < 4; i++) screen[5][64 + i] = timeLabel[i];
                for(int i = 0; i < 3; i++) screen[5][3 + i] = "#nn"[i];
                for(int i = 0; i < WIDTH; i++){
                    screen[3][i] = '=';
                    screen[7][i] = '=';
                }
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 2; j++){
                        screen[4 + i][9 + j] = '|';
                        screen[4 + i][22 + j] = '|';
                        screen[4 + i][40 + j] = '|';
                    }
                }
                for(int i = 0; i < f_records.size(); i++){
                    for(int j = 0; j < ('#' + std::to_string(i + 1)).size(); j++)
                        screen[9 + 4 * i][3 + j] = ('#' + std::to_string(i + 1))[j];
                    int scoreSize = std::to_string(f_records[i].score).size();
                    for(int j = 0; j < scoreSize; j++){
                        screen[9 + 4 * i][16 - (scoreSize / 2) + j] = std::to_string(f_records[i].score)[j];
                    }
                    int diffSize = f_records[i].diff.size();
                    for(int j = 0; j < diffSize; j++){
                        screen[9 + 4 * i][32 - (diffSize / 2) + j] = f_records[i].diff[j];
                    }
                    int timeSize = f_records[i].time.size();
                    for(int j = 0; j < timeSize; j++){
                        screen[9 + 4 * i][66 - (timeSize / 2) + j] = f_records[i].time[j];
                    }
                    for(int j = 0; j < WIDTH; j++){
                        screen[11 + 4 * i][j] = '=';
                    }
                }
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 16; j++){
                        screen[54 + i][37 + j] = eraseButton[i][j];
                        screen[60 + i][37 + j] = menuButton3[i][j];
                    }
                    for(int j = 0; j < 4; j++){
                        screen[54 + i + activeMenuButton * 6][30 + j] = menuPointer[i][j];
                    }
                }
                print_screen();
                int key = getch();
                switch(key){
                    case 65:
                        activeMenuButton--;
                        break;
                    case 66:
                        activeMenuButton++;
                        break;
                }
                if(activeMenuButton < 0) activeMenuButton = 1;
                if(activeMenuButton > 1) activeMenuButton = 0;
                if(key == 72){
                    if(activeMenuButton == 0){
                        int ynButton = 0;
                        while(true){
                            clear_screen();
                            for(int i = 0; i < 3; i++){
                                for(int j = 0; j < 16; j++){
                                    screen[31 + i][36 + j] = noButton[i][j];
                                    screen[43 + i][36 + j] = yesButton[i][j];
                                }
                                for(int j = 0; j < 4; j++){
                                    screen[31 + i + ynButton * 12][30 + j] = menuPointer[i][j];
                                }
                            }
                            for(int i = 0; i < 39; i++){
                                screen[20][25 + i] = confirmEraseLabel[i];
                            }
                            print_screen();

                            int key2 = getch();
                            switch(key2){
                                case 65:
                                    ynButton--;
                                    break;  
                                case 66:
                                    ynButton++;
                                    break;
                            }
                            if(ynButton < 0) ynButton = 1;
                            if(ynButton > 1) ynButton = 0;
                            if(key2 == 72){
                                if(ynButton == 0) break;
                                else{
                                    remove(recordsSavingPath);
                                    exit = true;
                                    break;
                                }
                            }
                        }
                    }
                    else break;
                }
                if(exit == true) break;
            }
        }
        else{
            clear_screen();
            for(int i = 0; i < WIDTH; i++){
                screen[(HEIGHT / 2) - 3][i] = '=';
                screen[(HEIGHT / 2) + 3][i] = '=';
            }
            std::string noRecords = "NO RECORDS YET";
            for(int i = 0; i < noRecords.size(); i++){
                screen[HEIGHT / 2][(WIDTH / 2) - 7 + i] = noRecords[i];
            }
            print_screen();
            std::this_thread::sleep_for(3s);
        }
        recordsin.close();
    }
    void get_key(int &direction){
        while(true){
            if(stopControls) break;
            controlsMutex.lock();
            int key = getch();
            controlsMutex.unlock();
            switch(key){
                case 65:
                    if(direction != 3) direction = 1;
                    break;
                case 67:
                    if(direction != 4) direction = 2;
                    break;
                case 66:
                    if(direction != 1) direction = 3;
                    break;
                case 68:
                    if(direction != 2) direction = 4;
                    break;
            }
        }
    }
    void new_game(){
        int diff = 0;
        while(true){
            clear_screen();
            for(int i = 0; i < 21; i++){
                screen[9][34 + i] = chooseDiff[i];
            }
            for(int i = 0; i < 88; i++){
                screen[20][i] = '=';
                screen[62][i] = '=';
                }
            for(int i = 21; i < 62; i++){
                screen[i][18] = '|';
                screen[i][19] = '|';
                screen[i][70] = '|';
                screen[i][71] = '|';
            }
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 16; j++){
                    screen[25 + i][37 + j] = diffLow[i][j];
                    screen[40 + i][37 + j] = diffMedium[i][j];
                    screen[55 + i][37 + j] = diffHigh[i][j];
                }
                    for(int j = 0; j < 4; j++){
                    screen[diff * 15 + 25 + i][30 + j] = menuPointer[i][j];
                }
            }
            print_screen();

            int key = getch();
            switch(key){
                case 65:
                    diff--;
                    break;
                case 66:
                    diff++;
            }
            if(key == 72) break;
            if(diff < 0) diff = 2;
            if(diff > 2) diff = 0;
            print_screen();
        }
        while(true){
            int score = 0;
            int direction;
            int fieldHeight = 22;
            int fieldWidth = 44;
            char field[fieldHeight][fieldWidth];
            bool keyPressed = true;

            for(int i = 0; i < fieldHeight; i++){
                for(int j = 0; j < fieldWidth; j++){
                    if(i == 0 || i == fieldHeight - 1 || j == 0 || j == fieldWidth - 1) field[i][j] = '#';
                    else field[i][j] = ' ';
                }
            }

            SnakePart snakeHead;
            snakeHead.x = fieldWidth / 2;
            snakeHead.y = fieldHeight / 2;
            std::vector<SnakePart> snake = {snakeHead};
            field[snakeHead.y][snakeHead.x] = '@';
            std::vector<SnakePart>::iterator snakeTail = snake.end() - 1;

            clear_screen();
            for(int i = 0; i < fieldHeight; i++){
                for(int j = 0; j < fieldWidth; j++){
                    screen[(HEIGHT / 2) - (fieldHeight / 2) + i][(WIDTH / 2) - (fieldWidth / 2) + j] = field[i][j];
                }
            }
            print_screen();
 
            while(keyPressed){
                int key = getch();
                switch(key){
                    case 65:
                        direction = 1;
                        keyPressed = false;
                        break;
                    case 67:
                        direction = 2;
                        keyPressed = false;
                        break;
                    case 66:
                        direction = 3;
                        keyPressed = false;
                        break;
                    case 68:
                        direction = 4;
                        keyPressed = false;
                        break;
                    default:
                        keyPressed = true;
                        break;
                }
            }
            stopControls = false;
            std::thread controls(&GameEngine::get_key, this, std::ref(direction));
            bool fruitGenerated = false;
            int fruitX;
            int fruitY;
            while(true){
                switch(direction){
                    case 1:
                        snakeHead.y--;
                        break;
                    case 2:
                        snakeHead.x++;
                        break;
                    case 3:
                        snakeHead.y++; 
                        break;
                    case 4:
                        snakeHead.x--;
                        break;
                }
                char approach = field[snakeHead.y][snakeHead.x];
                if(approach != ' ' && approach != '$') break;
                if(approach == '$'){
                    score += 10 + diff * 5;
                    SnakePart snakeNewPart;
                    snakeNewPart.x = snakeTail->x;
                    snakeNewPart.y = snakeTail->y;
                    snake.push_back(snakeNewPart);
                    fruitGenerated = false;
                }

                for(int i = 0; i < fieldHeight; i++){
                    for(int j = 0; j < fieldWidth; j++){
                        if(i == 0 || i == fieldHeight - 1 || j == 0 || j == fieldWidth - 1) field[i][j] = '#';
                            else field[i][j] = ' ';
                    }
                }

                for(std::vector<SnakePart>::iterator part = snake.end() - 1; part != snake.begin() - 1; part--){
                    std::vector<SnakePart>::iterator next = part - 1;
                    //std::vector<SnakePart>::iterator prev = part + 1;
                    if(part == snake.end() - 1){
                        if(part != snake.begin()){
                            part->y = next->y;
                            part->x = next->x;
                            field[part->y][part->x] = '+';  
                        }
                    }
                    if(part == snake.begin()){
                        part->y = snakeHead.y;
                        part->x = snakeHead.x;
                        field[part->y][part->x] = '@';
                    }
                    if(part != snake.end() - 1 && part != snake.begin()){ 
                        part->y = next->y;
                        part->x = next->x;
                        field[part->y][part->x] = 'O';
                    }
                }
                if(!fruitGenerated){
                    while(true){
                        srand(time(NULL));
                        fruitX = rand() % fieldWidth;
                        fruitY = rand() % fieldHeight;
                        if(field[fruitY][fruitX] == ' '){
                            fruitGenerated = true;
                            break;
                        }
                    }
                }
                field[fruitY][fruitX] = '$';

                clear_screen();
                std::string scoreLabel = "SCORE: ";
                scoreLabel += std::to_string(score);
                for(int i = 0; i < scoreLabel.size(); i++){
                    screen[((HEIGHT / 2) - (fieldHeight / 2)) / 2][(WIDTH / 2) - scoreLabel.size() / 2 + i] = scoreLabel[i];
                }
                for(int i = ((HEIGHT / 2) - (fieldHeight / 2)) / 2 - 1; i <= ((HEIGHT / 2) - (fieldHeight / 2)) / 2 + 1; i++){
                    for(int j = 0; j < 2; j++){
                        screen[i][(WIDTH / 2) - scoreLabel.size() / 2 - 4 + j] = '|';
                        screen[i][(WIDTH / 2) + scoreLabel.size() / 2 + 3 + j] = '|';
                    }
                }
                for(int i = (WIDTH / 2) - (scoreLabel.size() / 2) - 4; i <= (WIDTH / 2) + (scoreLabel.size() / 2) + 4; i++){
                    screen[((HEIGHT / 2) - (fieldHeight / 2)) / 2 - 2][i] = '=';
                    screen[((HEIGHT / 2) - (fieldHeight / 2)) / 2 + 2][i] = '=';
                }

                for(int i = 0; i < fieldHeight; i++){
                    for(int j = 0; j < fieldWidth; j++){
                        screen[(HEIGHT / 2) - (fieldHeight / 2) + i][(WIDTH / 2) - (fieldWidth / 2) + j] = field[i][j];
                    }
                }
                print_screen();
                GoToXY((HEIGHT / 2) - (fieldHeight / 2), (WIDTH / 2) - 5);

                std::this_thread::sleep_for(std::chrono::milliseconds(200 - diff * 50));
            }
            
            bool isRecordBest = false;
            if(score > 0){
                Record rec(score, diff);
                std::ifstream recordsin;
                recordsin.open(recordsSavingPath, std::ios_base::binary);
                recordsin.unsetf(std::ios::skipws);
                if(recordsin.peek() != EOF){
                    Record f_rec;
                    std::vector<Record> f_records{};
                    while(recordsin.read((char*)&f_rec, sizeof(Record)))
                        f_records.push_back(f_rec);
                    if(rec.score > f_records.at(0).score) isRecordBest = true;
                    f_records.push_back(rec);
                    std::sort(f_records.begin(), f_records.end());
                    if(f_records.size() > 10) f_records.erase(f_records.end() - 1);
                    std::ofstream recordsout;
                    recordsout.open(recordsSavingPath, std::ios_base::binary);
                    for(auto i : f_records) recordsout.write((char*)&i, sizeof(Record));
                    recordsout.close();
                }
                else{
                    std::ofstream recordsout;
                    recordsout.open(recordsSavingPath, std::ios_base::binary);
                    recordsout.write((char*)&rec, sizeof(rec));
                    recordsout.close();
                }
                recordsin.close();
            }

            clear_screen();
            if(isRecordBest){
                std::string bestRecord = "NEW BEST RECORD!";
                for(int i = 0; i < bestRecord.size(); i++){ 
                    screen[HEIGHT / 2 - 1][(WIDTH / 2) - (bestRecord.size() / 2) + i] = bestRecord[i];
                }
                for(int i = 0; i < std::to_string(score).size(); i++){
                    screen[HEIGHT / 2 + 1][(WIDTH / 2) - (std::to_string(score).size() / 2) + i] = std::to_string(score)[i];
                }
                for(int i  = 0; i < WIDTH; i++){
                    screen[27][i] = '=';
                    screen[39][i] = '='; 
                }
            }
            else{
                std::string yourScore = "YOUR SCORE IS ";
                yourScore += std::to_string(score);
                for(int i = 0; i < yourScore.size(); i++){
                    screen[HEIGHT / 2][(WIDTH / 2) - (yourScore.size() / 2) + i] = yourScore[i];
                }
                for(int i  = 0; i < WIDTH; i++){
                    screen[28][i] = '=';
                    screen[38][i] = '=';
                }
            }
            stopControls = true;
            std::this_thread::sleep_for(1s);
            print_screen();
            std::this_thread::sleep_for(1s);
            controls.join();

            int activeButton = 0;
            while(true){
                clear_screen();
                for(int i = 0; i < WIDTH; i++){
                    screen[8][i] = '=';
                    if(i > 20 && i < 67){
                        screen[24][i] = '=';
                        screen[58][i] = '=';
                    }
                }
                for(int i = 9; i < HEIGHT; i++){
                    for(int j = 0; j < 2; j++){
                        screen[i][19 + j] = '|';
                        screen[i][67 + j] = '|';
                    }
                }
                for(int i = 0; i < 10; i++){
                    screen[16][39 + i] = gameOver[i];
                }
                for(int i = 0; i < 3; i++){
                    for(int j = 0; j < 16; j++){
                        screen[31 + i][36 + j] = gameOverButton1[i][j];
                        screen[47 + i][36 + j] = gameOverButton2[i][j];
                    }
                    for(int j = 0; j < 4; j++){
                        screen[activeButton * 16 + 31 + i][30 + j] = menuPointer[i][j];
                    }
                }
                print_screen();

                int key = getch();
                switch(key){
                    case 65:
                        activeButton--;
                        break;
                    case 66:
                        activeButton++;
                }
                if(key == 72) break;
                if(activeButton < 0) activeButton = 1;
                if(activeButton > 1) activeButton = 0;
            }
            if(activeButton == 1) break;
        }
    }
};

#endif
