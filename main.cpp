#include <iostream>
#include <vector>
#include <tuple>
#include <unistd.h>

using namespace std;

#define FIELD_SIZE 8
#define NONE 0
#define BLACK 1
#define WHITE 2

class GameController{
private:
    /*
     ***fieldの形式***
      y
     x 0 1 2 3 4 5 6 7
      0
      1
      2
      3
      4
      5
      6
      7
    */
    vector<vector<int>> field;

    int turn;

    void discSetter(int disc_type, int x, int y){
        this->field[y][x] = disc_type;
    }

public:
    void initGame(){
        this->field.assign(FIELD_SIZE, vector<int>(FIELD_SIZE, NONE));

        this->field[(FIELD_SIZE-2)/2][(FIELD_SIZE-2)/2] = WHITE;
        this->field[(FIELD_SIZE-2)/2+1][(FIELD_SIZE-2)/2] = BLACK;
        this->field[(FIELD_SIZE-2)/2][(FIELD_SIZE-2)/2+1] = BLACK;
        this->field[(FIELD_SIZE-2)/2+1][(FIELD_SIZE-2)/2+1] = WHITE;

        this->turn = 1;
        return;
    }

    void printField(){
        for(int y = 0; y < FIELD_SIZE; y++){
            for(int x = 0; x < FIELD_SIZE; x++){
                string square_char;
                if(this->field[y][x] == NONE){
                    square_char = " ";
                } else if(this->field[y][x] == BLACK){
                    square_char = "#";
                } else if(this->field[y][x] == WHITE){
                    square_char = ".";
                }
                cout << "[" + square_char + "]";
            }
            cout << endl;
        }
        return;
    }

    int getTurn(){
        return this->turn;
    }

    void printTurn(){
        cout << "TURN: " + to_string(this->turn) << endl;
        return;
    }

    void addTurn(){
        this->turn++;
        return;
    }

    bool checkGameEnd(){
        for(int y = 0; y < FIELD_SIZE; y++){
            for(int x = 0; x < FIELD_SIZE; x++){
                if(this->field[y][x] == NONE) return false;
            }
        }
        return true;
    }

    tuple<int/*black*/, int/*white*/> getNumOfDiscs(){
        int num_of_black = 0;
        int num_of_white = 0;
        for(int y = 0; y < FIELD_SIZE; y++){
            for(int x = 0; x < FIELD_SIZE; x++){
                if(this->field[y][x] == BLACK){
                    num_of_black++;
                } else if(this->field[y][x] == WHITE){
                    num_of_white++;
                }
            }
        }

        return make_tuple(num_of_black, num_of_white);
    }

    vector<tuple<int/*disc_type*/, int/*x*/, int/*y*/>> changeableDiscSerch(int disc_type, int x, int y){
        vector<tuple<int/*disc_type*/, int/*x*/, int/*y*/>> changeable_discs;

        int directions[8][2] = {
        //  {x, y}
            {0, 1},
            {1, 0},
            {0, -1},
            {-1, 0},
            {1, 1},
            {1, -1},
            {-1, 1},
            {-1, -1}
        };

        for(int i_d = 0; i_d < 8; i_d++){
            vector<tuple<int/*disc_type*/, int/*x*/, int/*y*/>> found_discs;
            int next_x = x;
            int next_y = y;

            while(1){
                next_x += directions[i_d][0];
                next_y += directions[i_d][1];

                if(!(next_x >= 0 && next_x < FIELD_SIZE)) break;
                if(!(next_y >= 0 && next_y < FIELD_SIZE)) break;

                if(this->field[next_y][next_x] == NONE){
                    // 挟めるものがなければ探索をやめる
                    break;
                } else if(this->field[next_y][next_x] == disc_type){
                    // 挟んだ石を配列に入れる
                    for(int i_f = 0; i_f < found_discs.size(); i_f++){
                        changeable_discs.push_back(found_discs[i_f]);
                    }
                    break;
                } else {
                    // 挟める石があれば配列に入れる
                    found_discs.push_back(make_tuple(disc_type, next_x, next_y));
                }
            }
        }

        return changeable_discs;
    }

    string putDisc(int disc_type, int x, int y){
        // エラー処理
        if(!(disc_type >= NONE && disc_type <= WHITE)) return "invalid disc type";
        if(!(x >= 0 && x < FIELD_SIZE)) return "invalid x";
        if(!(y >= 0 && y < FIELD_SIZE)) return "invalid y";
        if(this->field[y][x] != NONE) return "you cannot put here";

        vector<tuple<int/*disc_type*/, int/*x*/, int/*y*/>> changeable_discs = changeableDiscSerch(disc_type, x, y);

        if(changeable_discs.size() > 0){
            this->discSetter(disc_type, x, y);
            // ひっくり返せる石をひっくり返す
            for(int i = 0; i < changeable_discs.size(); i++){
                this->discSetter(get<0>(changeable_discs[i]), get<1>(changeable_discs[i]), get<2>(changeable_discs[i]));
            }
            return "ok";
        } else {
            return "you cannot put here";
        }
    }
};


int main()
{
    GameController game_controller;

    game_controller.initGame();

    while(1){
        // ゲーム情報の表示
        system("clear");
        game_controller.printTurn();
        cout << endl;

        int turn = game_controller.getTurn();
        // 奇数なら白、偶数なら黒
        int turn_disc_color = (turn % 2 == 0) ? BLACK : WHITE;
        cout << (turn_disc_color == BLACK ? "BLACK" : "WHITE") << "'s TURN" << endl;
        cout << endl;

        cout << "BLACK: " << to_string(get<0>(game_controller.getNumOfDiscs())) << ", " << "WHITE: " << to_string(get<1>(game_controller.getNumOfDiscs())) << endl;
        cout << endl;
        game_controller.printField();

        // 石を置く座標の入力処理
        while(1){
            string command;
            cout << "input command (input: input x and y, pass: pass this turn)" << endl;
            cin >> command;
            if(command == "input"){
                int input_x, input_y;
                cout << "input x and y" << endl << ">>>";
                cin >> input_x >> input_y;

                string put_result = game_controller.putDisc(turn_disc_color, input_x, input_y);

                if(put_result == "ok"){
                    break;
                } else {
                    cout << put_result << endl;
                }
            } else if(command == "pass"){
                break;
            }
        }

        if(game_controller.checkGameEnd()) break;

        game_controller.addTurn();
   }

    // 結果表示
    int num_of_black = get<0>(game_controller.getNumOfDiscs());
    int num_of_white = get<1>(game_controller.getNumOfDiscs());
    cout << "BLACK: " << to_string(num_of_black) << ", " << "WHITE: " << to_string(num_of_white) << endl;

    if(num_of_black > num_of_white){
        cout << "BLACK WIN!!" << endl;
    } else if(num_of_white > num_of_black){
        cout << "WHITE WIN!!" << endl;
    } else {
        cout << "DRAW!!" << endl;
    }

    return 0;
}
