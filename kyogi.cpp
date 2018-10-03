/*
    とりあえずどうやって動くかっていうアルゴリズムはほっといて盤面生成から動作確認まで作る！
*/

/*
    相手チームのいる場所ってプログラム的に要らなくない？
    単純にマスの数数えて自分たちに有利になるようにマスを取ろうとすればよくない？
    将棋とか2ターンあとを読むだけでコンピュータの圧勝ですし2ターンあとまで読むってことでいいかな？おっけ
*/

#include <list>
#include <utility>
#include <limits>
#include <string>
#include <iostream>
#include <cstdlib>

//盤面のサイズ(生成時の値をこちらに代入)
static const std::size_t width = 8, height = 8;

//ターン数(こっちも代入)
static const std::size_t turn;

//プレイヤー座標
int state_player[width][height];
for(int j = 0; j < height; j++){
    for(int i = 0; i < width; i++){
        state_player[i][j] = 0;
    }
}



struct hand{
    unsigned int x = std::unmeric_limits<unsigned int>::max(), y = std::numeric_limits<unsigned int>::max();

    bool pass() const{
        return x == std::numeric_limits<unsigned int>::max() || y ==std::numeric_limits<unsigned int>::max();
    }
};

//プレイヤーの位置座標
struct player_state{
    enum class state : char{
        red1 = 1,
        red2 = 2,
        blue1 = 3,
        blue2 = 4
    };

}
//盤面全体
struct state3_stage{

    //パネルの色
    enum class state : char{
        null = 0,
        red,
        blue = 2
    };

    static const std::size_t tabel_size = width * height * 2 / (sizeof(unsigned int ) * 8) + (width * height * 2 % (sizeof(unsigned int) * 8) > 0 ? 1 : 0);

    // 盤面の状態をビットテーブルで保存する.
    unsigned int bit_table[tabel_size];

    //現在のターン数
    std::size_t turn_count;

    //次の盤面を樹構造で保持する
    std::list<std::pair<hand, state3_stage>> subtree;

    //現在の盤面の価値
    int avail = 0;

    //盤面のプレイヤーの位置
    state_plyr plyr_pos;

    //盤面のプレイヤーのパネル(?言い方がわからない)
    state plyr;

    //この盤面の適した手
    hand opt_hand;
    
    //state3_stageのポインタ
    state3_stage *cache;

    state3_stage shallow_copy() const{
        state3_stage r;
        for(int i=0;i<tabel_size;i++){
            r.bit_table[i] = bit_table[i];
        }
        r.turn_count = turn_count;
        r.avail = avail;
        r.apply_plyr = plyr_pos
        r.plyr = plyr;
        r.opt_hand = opt_hand;
        return r;
    }

    //点数のカウント





    //行動
    void apply_plyr(){
        for(int a = 0;a<tabel_size;a++){
            bit_table[a] = cache->bit_table[a];
        }
        turn_count = cache->turn_count;
        {
            auto tmp = std::move(cache->subtree);
            subtree = tmp;
        }
        avail = cache->avail;
        plyr = cache->plyr;
        opt_hand = cache->opt_hand;
        cache = nullptr;
    }

    //戦略木を構築する(とりあえず1ターン先のことを考えて打つようにする)(とりあえず今はmin-maxでやってみる)
    //level = 1
    void compute(int level){
        if(level <= 0 || turn_count == turno){
            short_evaluation(plyr);
            return;
        }
        if(subtree.empty()){
            subtree = listup_nexthand(plyr)
        }
        int min_max = std::numeric_limits<int>::max();
        for()
    }

    //短絡評価(パネルの得点)
    void short_evaluation(state side){
        avail = evaluation(reverse(side)) - evaluation(side)); 
    }

    int evaluation(state side) const{
        int avail = 0;
        //1マスずつ見ていく
        for(unsigned int j = 0; j < height; j++){
            for(unsigned int i = 0;i < width; i++){
                //(i,j)の位置にパネルを置けば点数がどう動くかを計算上の点数のカウントを先に造らないと..ぁぁぁあああ
            }
        }
    }

    //プレイヤーの位置を移動させる
    std::list<std::pair<hand, state3_stage>> listup_nexthand(state panel) const{
        std::list<std::pair<hand, state3_stage>> list;
        for(unsigned int j = 0;J < height; j++){
            for(unsigned int i = 0; i < width; i++){
                unsigned int t = (*this)(i,j);
                //redならt = 1 blueならt = 2 nullなら0
                //ここをプレイヤーが存在している位置に変更する
                if(t != static_cast<int>(state::null)){
                    continue;
                }
                //t座標に
            }
        }
    }

    //敵色のパネル
    static state reverse(state s){
        state t;
        if(state::red1){
            t = state::blue;
        }
        else if(state::blue){
            t = state::red;
        }
        return t;
    }

/*いらなくね？
    //敵の座標
    static state_plyr reverse_plyr(state_plyr s){
        state_plyr t;
        if(state_plyr::red){
            t = state_plyr::blue;
        }
        else if(state_plyr::blue){
            t = state_plyr::red;
        } 
        return t;
    }
    */

    //指定した位置にパネルがおける(退かせる)かどうか
    //置ける場合は置いた(退かせた)後の盤面を返す
    std::pair<bool, state3_stage> test(int x, int y, state s, state_plyr p) const{
        state r = reverse(s);
        state_plyr rp = reverse_plyr(p);
        std:: size_t count = 0; //ここのcountは必要ないので後々消す
        state3_stage ret_stage = shallow_copy();

        //何もない||相手がいる||自分の色||相手の色


        //今いる場所の1つ下に相手プレイヤーが存在しない
        if((*this)(x,y-1) != static_cast<int>(rp)){
            //1つ下が何もない
            if((*this)(x,y-1) == state::null){
                
            }
        }
        //パネルがおかれてない
        else if((*this)(x,y-1) == state::null){

        }


    }
    }

    //プレイヤー座標の移動
    void move_player(int p, int x, int y, int h, int v){
        //赤
        if(p == 1){
        state_player[x][y] = 0;
        state_player[x + h][y + v] = 1; 
        }
        //青
        else{
        state_player[x][y] = 0;
        state_player[x + h][y + v] = 2;
        }
    }

    void game(){
        //赤は"1"青は"2"という形で扱っていく
        std::cout << "red:1, blue:2" << std::endl;
        int p_color = input("12") + 1;
        state3_stage player_color,enmy_color;
        /*  赤青
            青赤　　みたいな感じで決めときたい
        */
        if(p_color == 1){
            player_color = state3_stage::state::red;
        }
        else{
            player_color = state3_stage::state::blue;
        }
        enmy_color = state3_stage::reverse(player_color);

        //盤面を's'という変数に置く
        state3_stage s;
        //それぞれ初めの4つのパネルを置き、プレイヤーを設置する。
        //ここパイソンのQRコードから読み込んだ数列でパネルの座標を決めてく
        //s(x座標,y座標,static_cast<int>(state3_stage::state::red));　
        //move_player(1か2,x座標,y座標,0,0) 　みたいな感じで
        s.turn_count = 1;
        s.plyr = enmy_color();

        std::cout << "考え中..." << std::endl;
        int level = 1; //試作として1手先を読む"１"
        s.compute(level);
        std::cout << "player1: " << //行動（移動、除去、停滞）,座標(x,y)
                　<< "\n"
                  << "player2: " << //行動（移動、除去、停滞）,座標(x,y)
                  << std::endl;
        //プレイヤー間の移動、除去後のマスはパイソンのUIで塗るとしたら楽そう
        //確認(こちらが移動できなかったときは相手も移動できていない。かつパネルの変動もない（停滞は除く、マスの色塗りで対応できる）ので)
        int check = 0;
        std::cout << "移動できなかったプレイヤーは？"
                  << "\n"
                  << "いない:0,player1:1,player2:2,両方:3"
                  << std::endl;
        check = input("0123");
        switch(check){
            case : 0 
            break;
            case : 1
            break;
            case : 2
            break;
            case : 3
            break;
        }
        //パネルの色をUIで塗る


    }




}

int main(){

    game();
    //コンパイラが閉じるのを防ぐため
    getchar();
    getchar();

    return 0;
}
