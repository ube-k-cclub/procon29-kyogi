/*
    更新版


    ○仕様等
    ・変数としておくとき赤は"1"青は"2"という形で扱っていく
    ・盤面生成後、手動で自陣、敵陣のパネルを分けるその”オッケーボタン”的な奴を押してその時点から探索を開始する
    ・上のやつをみなすためにUIン今後実装して欲しいこと
      :(上のやつ)セットアップ終了の合図
      :自陣の色は何色か
      ：

    ○UI生成時に取得してほしいもん
    ・ターン数
    ・それぞれのマスの得点1次配列で上の段の左上から右に01234...端に来たら一段下がってまた左からってのを代入
    ・初めからあるパネルの(x,y)座標QRコードのそのままのやつをそれぞれ４つを
    std::pair<short int, short int> red1 = std::make_pair(2,1);
    て感じで宣言してあるのでそいつらに入れてほしい
    上の宣言は左上の赤1です

    10x10の盤面
     ０１２３４５６７８９　　　左上から赤１　青１
    0　　　　　　　　　　　　　　　　　青２　赤２
    1  　赤　　　　青
    2
    3
    4
    5
    6
    7
    8　　青　　　　赤
    9


    ○自分用
    ban_table[]に置かれているパネルの色兼無色を入れていく
    プレイヤーの位置は初めに宣言したred1,red2,blue1,blue2を再利用してそいつらをプレイヤー座標として使う
    毎ターンごとに

    次始めるとこ
    9/22:400




    ○一連の流れの再確認
    1．盤面生成、自陣の色選択、初期パネル設置、初期位置にプレイヤー設置
    2．α―β法(時間的に厳しかったらMin-Max法になっちゃうかも)にて次に設置、除去、停滞するパネルを選択
    3．対戦相手の行動により想定していた動きができなかった場合の処理(その後２，３ターン連続で同じ動きするときとか対策考えないといけないです)
    4．PythonでのUI操作でプレイヤーの位置変更、パネルの操作を行い、操作終了ボタン的な何かを押して、1ターンが終わる。
    以下規定ターンまで2~4のループ

    std::cout <<  << std::endl;
*/

#include <list>
#include <utility>
#include <limits>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <iterator>


//UIの生成時にデータを入れてほしい者たち

//盤面のサイズ(生成時の値をこちらに代入)
//========================================================UIから
const std::size_t width = 10, height = 10;

//パネルの位置を保存しておく
unsigned int ban_table[width * height] = {0};

//コピー用
unsigned int copy_ban_table[width * height] = {0};


//プレイヤー座標
unsigned int state_player[width * height] = {0};

//マスの得点
//=========================================================UIから
int panel_point[width * height] = {0};


//プレイヤーの周りの座標({null、敵パネル、自パネル}, 座標)
std::pair<int , int> around[9];
std::pair<int , int> around2[9];
 
//プレイヤー座標兼初期座標
//=========================================================UIから
std::pair<short int, short int> red1 = std::make_pair(2,1);
std::pair<short int, short int> red2 = std::make_pair(7,8);
std::pair<short int, short int> blue1 = std::make_pair(2,8);
std::pair<short int, short int> blue2 = std::make_pair(7,1);

//ターン数(こっちも代入)　今は仮
//=========================================================UIから
static const std::size_t turn = 100;

//探索をするときの打った手を保存するための変数
struct hand{
    int i = 0, //null、敵パネル、自パネル
     v = 0;    //座標
};

//入力された座標の色を返す
int search_color(int num){
    switch(ban_table[num]){
        case 0 : return 0;
        case 1 : return 1;
        case 2 : return 2;
        default : return 0;
    }
}

struct state3_stage{
    //パネルの色
    enum class state : char{
        null = 0,
        red,
        blue = 2
    };

    enum class panel_color : char{ 
        red = 1,
        blue = 2
    };

    enum class player_num : char{
        null,
        red1,
        red2,
        blue1,
        blue2
    };

    //現在の盤面の価値
    int avail;

    // 次の盤面を木構造で保持する.
    std::list<std::pair<hand, state3_stage>> subtree;

    //子の盤面でのプレイヤーの適した手
    hand opt_hand;

    //敵と自陣の色判別
    state3_stage::state player_color,enmy_color;

    state3_stage *cache;    //ポインタを用いた構造体宣言

    //通常のパネルと除去するときのパネル
    unsigned bit_table[width * height] = {0};

    unsigned copy_ban_table1[width * height] = {0};

    unsigned copy_ban_table2[width * height] = {0};


    //それぞれにプレイヤーの座標が入る
    int red1 = 0;
    int red2 = 0;
    int blue1 = 0;
    int blue2 = 0;

    state3_stage shallow_copy() const{
        state3_stage r;
        for(int i = 0; i < width * height; i++){
            r.bit_table[i] = ban_table[i];
        }
        r.avail = avail;
        r.opt_hand = opt_hand;
        return r;
    }

    static state reverse(state s){
        state t;
        if(s == state::red){
          t = state::blue;
        }
        else if (s == state::blue){
            t = state::red;
        }
        return t;
    }

    //ここは動作が確定した後
    void apply_player1(){
        {
            auto tmp = std::move(cache->subtree);
            subtree = tmp;
        }
        avail = cache->avail;
        opt_hand = cache->opt_hand;
        cache = nullptr;
    }

    void stage_copy(){
        std::cout << "コピーします" << std::endl;
        for(int i = 0; i < width * height; i++){
            copy_ban_table[i] = ban_table[i];
        }
    }

    //まずプレイヤー１を探索して、行動を選択、その次にプレイヤー２で探索、行動選択
    void compute1(int level,int player_color){

        areapoint_sum(state3_stage::state::red);

/*
        player_num player1;
        state player;
        if(player_color == 1){
            player1 =  player_num::red1;
            player = state::red;
        }
        else if(player_color == 2){
            player1 = player_num::blue1;
            player = state::blue;
        }
        else{
            std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
        }        
        if(level <= 0){
            //ここで1つの探索する木の一番下の点数が出る
            short_evaluation(player1);
            return;
        }
       if(subtree.empty()){
           subtree = listup_nexthand(player1);
       }
        for(auto &i : subtree){
            //subtreeに入っている分computeを行う
            i.second.compute1(level - 1, player_color);
       }
       int min_max = 0;
       for(auto &i : subtree){
            std::cout << bit_table[2] << "aaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
           std::cout << i.second.avail << std::endl;
           if(min_max > i.second.avail){
               min_max = i.second.avail;
               opt_hand = i.first;
               std::cout << "opt_hand.x:" << i.first.i << "opt_hand.y:" << i.first.v << std::endl;
                cache = &i.second;
                avail = i.second.evaluation(player); 
           }
       }
        
        std::cout << "来てます" << std::endl;

        player_num player1;
        state player;
        if(player_color == 1){
            player1 =  player_num::red1;
            player = state::red;
        }
        else if(player_color == 2){
            player1 = player_num::blue1;
            player = state::blue;
        }
        else{
            std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
        }
        if(level <= 0){
            //ここで1つの探索する木の一番下の点数が出る
            short_evaluation(player1);
            return;
        }
       if(subtree.empty()){
           subtree = listup_nexthand(player1);
       }
       for(auto &i : subtree){
            //subtreeに入っている分computeを行う

           i.second.compute1(level - 1, player_color);
       }
       int min_max = 0;
       for(auto &i : subtree){
            std::cout << bit_table[2] << "aaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
           std::cout << i.second.avail << std::endl;
           if(min_max > i.second.avail){
               min_max = i.second.avail;
               opt_hand = i.first;
               std::cout << "opt_hand.x:" << i.first.i << "opt_hand.y:" << i.first.v << std::endl;
                cache = &i.second;
                avail = i.second.evaluation(player); 
           }
       }
       */
    };

    void compute2(int level, int player_color){
        player_num player2;
        if(player_color == 1){
            player2 = player_num::red2;
        }
        else if(player_color == 2){
            player2 = player_num::blue2;
        }
        else{
            std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
        }
    }

    //短絡評価
    void short_evaluation(player_num player){
        state player_color;
        if(player == player_num::red1 || player == player_num::red2){
            player_color = state::red;
        }
        else{
            player_color = state::blue;
        }
        avail = evaluation(player_color);
    }

    //プレイヤーの合計得点
    int evaluation(state player_color){
        int avail = 0;
        //単純なパネルによる得点の合計(タイルポイント)
        for(int j = 0; j < height; j++){
            for(int i = 0; i < width; i++){
                if(copy_ban_table[j * height + i] == static_cast<int>(player_color)){
                    //ここでマスの重さを入れなとイケナイ太陽今は適当に普通のマスと外周、角だけ入れとくあくまで今のここはサンプル
                    static const float norm_weight = 1;
                    static const float edge_weight = norm_weight / 2;
                    static const float kado_weight = norm_weight / 4;
                    if((i == 0 || i == (width - 1)) && (j == 0 || j == height - 1)){
                        avail += //kado_weight * 
                        panel_point[j * height + i];
                    }else if(i == 0 || i == width - 1 || j == 0 || j == height - 1){
                        avail += //edge_weight * 
                        panel_point[j * height + i];
                    }else{
                        avail += //norm_weight * 
                        panel_point[j * height + i];       
                   }
                }
            }
        }

        
        //囲んだことによる得点(領域ポイント)
        //いい方法を思いつきました

        return avail;
    }


    void stage_copy1(){
        std::cout << "コピーします" << std::endl;
        for(int i = 0; i < width * height; i++){
            copy_ban_table1[i] = ban_table[i];
        }
    }
    void stage_copy2(){
        std::cout << "コピーします" << std::endl;
        for(int i = 0; i < width * height; i++){
            copy_ban_table2[i] = copy_ban_table1[i];
        }
    }

    int evaluation_1(state player_color){
        int avail = 0;
        //単純なパネルによる得点の合計(タイルポイント)
        for(int j = 0; j < height; j++){
            for(int i = 0; i < width; i++){
                if(copy_ban_table1[j * height + i] == static_cast<int>(player_color)){
                    //ここでマスの重さを入れなとイケナイ太陽今は適当に普通のマスと外周、角だけ入れとくあくまで今のここはサンプル
                    static const float norm_weight = 1;
                    static const float edge_weight = norm_weight / 2;
                    static const float kado_weight = norm_weight / 4;
                    if((i == 0 || i == (width - 1)) && (j == 0 || j == height - 1)){
                        avail += //kado_weight * 
                        panel_point[j * height + i];
                    }else if(i == 0 || i == width - 1 || j == 0 || j == height - 1){
                        avail += //edge_weight * 
                        panel_point[j * height + i];
                    }else{
                        avail += //norm_weight * 
                        panel_point[j * height + i];       
                   }
                }
            }
        }
        //囲んだことによる得点(領域ポイント)
        //いい方法を思いつきました

        return avail;
    }

    int evaluation_2(state player_color){
        int avail = 0;
        //単純なパネルによる得点の合計(タイルポイント)
        for(int j = 0; j < height; j++){
            for(int i = 0; i < width; i++){
                if(copy_ban_table2[j * height + i] == static_cast<int>(player_color)){
                    std::cout << j * height + i << std::endl;
                    //ここでマスの重さを入れなとイケナイ太陽今は適当に普通のマスと外周、角だけ入れとくあくまで今のここはサンプル
                    static const float norm_weight = 1;
                    static const float edge_weight = norm_weight / 2;
                    static const float kado_weight = norm_weight / 4;
                    if((i == 0 || i == (width - 1)) && (j == 0 || j == height - 1)){
                        avail += //kado_weight * 
                        panel_point[j * height + i];
                    }else if(i == 0 || i == width - 1 || j == 0 || j == height - 1){
                        avail += //edge_weight * 
                        panel_point[j * height + i];
                    }else{
                        avail += //norm_weight * 
                        panel_point[j * height + i];       
                   }
                }
                //領域ポイントの計算もここでやっちゃう
            }
        }
        //囲んだことによる得点(領域ポイント)
        //いい方法を思いつきました
        /*
            10x10の盤面
     ０１２３４５６７８９　　　左上から赤１　青１
    0　　　赤　　　　　　　　　　　　　青２　赤２
    1  　赤　赤　　青
    2　　赤　赤　青　　　　　(7,2)は上の列から見ればリストに入るが、横列から見れば領域ポイント
    3　　　赤青　　青       は入らないことがわかるからリストから外す
    4　　　青　青
    5　　　青　赤
    6　　青　　　赤
    7　　青
    8　　青　　　　赤
    9
        */

       
        return avail;
    }

    int areapoint_sum(state player_color){ 
        int sum = 0, time = 0;
        std::list<int> area;  //計算が遅くなるようであればvectorに変更
        int count = 0;
        //横の列を見る
        for(int j = 0; j < height; j++){
            for(int i = 0; i < width; i++){
                if(ban_table[j * height + i] == static_cast<int>(player_color)){
                    count++;
                    if(time == 0){
                        time = 1;
                    }
                    else if (time == 2){
                        time = 3;
                    }
                }
                else{
                    if(time == 1) time = 2;
                    //一度も自パネルを通ってないから領域ポイントから除外していい
                    if(time == 0){
                        area.push_front(j * height + i);
                    }
                }
            }
            if(count < 2 || time != 3){
                for(int k = 0; k < width; k++){
                    area.push_front(j * height + k);
                }
            }
            else{
                for(int k = 0; k < width; k++){
                    if(ban_table[j * height + k] == static_cast<int>(player_color)){
                        area.push_front(j * height + k);
                    }
                    if(k == 0 || k == width - 1){
                        area.push_front(j * height + k);
                    }
                }
            }
            time = 0;
            count = 0;
        }
        //縦の列を見る
        for(int j = 0; j < height; j++){
            for(int i = 0; i < width; i++){
                if(ban_table[i * width + j] == static_cast<int>(player_color)){
                    count++;
                    if(time == 0){
                        time = 1;
                    }
                    else if (time == 2){
                        time = 3;
                    }
                }
                else{
                    if(time == 1) time = 2;
                    if(time == 0){
                        area.push_front(i * width + j);
                    }
                }
            }
            if(count < 2 || time != 3){
                for(int k = 0; k < width; k++){
                    area.push_front(k * width + j);
                }
            }
            else{
                for(int k = 0; k < width; k++){
                    
                    if(ban_table[k * height + j] == static_cast<int>(player_color)){
                        area.push_front(k * width + j);
                    }
                    if(k == 0 || k == width - 1){
                        area.push_front(k * width + j);
                    }
                }
            }
            time = 0;
            count = 0;
        }
        //重複を消す
        area.sort();
        area.unique();
        //領域ポイントになる可能性のあるパネルの計算
        int i = 0;
        std::list<int> in_area;
        std::list<int>::iterator itr;
        std::list<int>::iterator itr2;
        for(itr = area.begin(); itr != area.end(); itr++, i++){
            while(*itr != i){
                in_area.push_front(i);
                i++;
            }
        }
        in_area.unique();
        std::list<int> in_area_copy;
        int k = 0;
        int ex = 0;
        while(in_area.size() != in_area_copy.size()){
            in_area_copy = in_area;
            for(itr = in_area.begin(); itr != in_area.end(); itr++){
                for(int j = 0; j < 4; j++){
                    for(itr2 = in_area.begin(); itr2 != in_area.end(); itr2++,k++){
                        if(check(j, *itr) == *std::next(in_area.begin(), k)){
                            ex++;
                        }
                    }
                    if(ban_table[check(j, *itr)] == static_cast<int>(player_color)){
                        ex++;
                    }
                    k = 0;
                }
                //!上下左右に自パネルか領域パネルになる可能性のあるパネルがある
                if(ex != 4){
                    in_area_copy.remove(*itr);
                }
                ex = 0;
            }        
            if(in_area_copy.size() == 0){
                std::cout << "領域ポイントはありません" << std::endl;
                in_area = in_area_copy;
                break;
            }
            auto tmp = in_area;
            in_area = in_area_copy;
            in_area_copy = tmp;
        }
        //領域ポイントの合計点
        for(itr = in_area.begin(); itr != in_area.end(); itr++){
            if(panel_point[*itr] < 0){
                sum += panel_point[*itr] * (-1);
            }
            else{
                sum += panel_point[*itr];
            }
        }
        
        std::cout << "sum == " << sum << std::endl;

        return sum;
    }

    
    int check(int num, int coord){
        switch(num){
            case 0 : return coord - width; break;
            case 1 : return coord + 1; break;
            case 2 : return coord + width; break;
            case 3 : return coord - 1; break;
        }
    }

    //取りうる次の手をリストアップする　引数はstate_player
    std::list<std::pair<hand, state3_stage>> listup_nexthand(player_num player){
        int pattern = 9;  //最大9方向に動ける（その場含む）
        int num = 0, tmp = 0, tmp2 = 0;
        state s;
        if(player == player_num::red1 || player == player_num::red2){
            num = 1;
            s = state::red;
        }
        else{
            num = 2;
            s = state::blue;
        }
        std::cout << std::endl;
        move_range(static_cast<int>(player));// 行けるところを探す
        for(int i = 0; i < pattern; i++){
            around2[i] = around[i];
        }
        stage_copy1();
        tmp = red1; // ==========tmpには移動する前の状態が入る
        for(int i = 0; i < pattern; i++){
            stage_copy2();
            red1 = tmp;
            if(around2[i].second > 0){
            copy_ban_table1[around2[i].second] = num; //行ける場所にアクション   
            move_player(i);
            move_range(static_cast<int>(player));
            for(int j = 0; j < pattern; j++){ 
                if(around[j].second > 0){              
                copy_ban_table2[around[j].second] = num;
                avail = evaluation_2(s);
                if(j != 0) copy_ban_table2[around[j].second] = 0;
                std::cout << "点数は22222222ーーーー" << avail << std::endl;
                }
            }
            avail = evaluation_1(s);
            std::cout << "点数はーーーー" << avail << std::endl;
            
            if(i != 0) copy_ban_table1[around2[i].second] = 0;
            }
            red1 = tmp;
        }









        std::list<std::pair<hand, state3_stage>> list;

        return list;
    }

    void move_player(int num){
        int j = red1 / width; 
        int i = red1 - j * width;
        switch(num){
            case 0 :red1 = j * width + i;break;
            case 1 :red1 = j * width + i - width;break;
            case 2 : red1 = j * width + i - width + 1; break;
            case 3 : red1 = j * width + i + 1; break;
            case 4 : red1 = j * width + i + width + 1; break;
            case 5 : red1 = j * width + i + width; break;
            case 6 : red1 = j * width + i + width - 1; break;
            case 7 : red1 = j * width + i - 1; break;
            case 8 : red1 = j * width + i - width - 1; break;
        }
    }

        
    //(とりあえず)自プレイヤーの周りの座標を調べる
    void move_range(int coord){
        std::cout << "来てますよね？" << std::endl;
        int i = 0, j = 0;
        int accel = 1, del = 2;
        state3_stage player_stage = shallow_copy();
        //自陣は赤
        if(player_color == state::red){
            switch(coord){
            case 1 :
            j = red1 / width; 
            i = red1 - j * width;
            around[0].second = j * width + i;
            around[0].first = search_color(j * width + i); 
            
            //これで自分の今いる位置にもう一度移動する
            player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
        //    std::cout << j << "," << i << std::endl;
            //coordの上の座標
            if(j != 0){
                around[1].second = j * width + i - width;
                around[1].first = search_color(j * width + i - width); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[1].second = -1;
                around[1].first = search_color(-1); 

            }
            //coordの右上の座標
            if((j != 0) && (i != width - 1)){
                around[2].second = j * width + i - width + 1;
                around[2].first = search_color(j * width + i - width + 1); 
                if(around[2].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[2].second = -1;
                around[2].first = search_color(-1); 
            }
            //coordの右の座標
            if(i != (width - 1)){
                around[3].second = j * width + i + 1;
                around[3].first = search_color(j * width + i + 1);
                if(around[3].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[3].second = -1;
                around[3].first = search_color(-1); 

            }
            //coordの右下
            if((i != (width - 1)) && (j != (height - 1))){
                around[4].second = j * width + i + width + 1;
                around[4].first = search_color(j * width + i + width + 1); 
                if(around[4].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[4].second = -1;
                around[4].first = search_color(-1); 
            }
            //coordの下
            if(j != (height - 1)){
                around[5].second = j * width + i + width;
                around[5].first = search_color(j * width + i + width); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[5].second = -1;
                around[5].first = search_color(-1); 
            }
            //coordの左下
            if((j != (height - 1)) && (i != 0)){
                around[6].second = j * width + i + width - 1;
                around[6].first = search_color(j * width + i + width - 1); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[6].second = -1;
                around[6].first = search_color(-1); 
            }
            //coordの左
            if(i != 0){
                around[7].second = j * width + i - 1;
                around[7].first = search_color(j * width + i + width - 1); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                   
               }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[7].second = -1;
                around[7].first = search_color(-1); 
            }
            //coordの左上
            if((i != 0) && (j != 0)){
                around[8].second = j * width + i - width - 1;
                around[8].first = search_color(j * width + i - width - 1); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), del);
                    
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red1), accel);
                }
            }
            else{
                around[8].second = -1;
                around[8].first = search_color(-1); 
            }
            break;

            //プレイヤー２
            case 2 :
            j = red2 / width; 
            i = red2 - j * width;
            around[0].second = j * width + i;
            around[0].first = search_color(j * width + i); 
            player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);

            std::cout << j << "," << i << std::endl;
            //coordの上の座標
            if(j != 0){
                around[1].second = j * width + i - width;
                around[1].first = search_color(j * width + i - width); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[1].second = -1;
                around[1].first = search_color(-1); 

            }
            //coordの右上の座標
            if((j != 0) && (i != width - 1)){
                around[2].second = j * width + i - width + 1;
                around[2].first = search_color(j * width + i - width + 1); 
                if(around[2].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[2].second = -1;
                around[2].first = search_color(-1); 
            }
            //coordの右の座標
            if(i != (width - 1)){
                around[3].second = j * width + i + 1;
                around[3].first = search_color(j * width + i + 1); 
                if(around[3].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[3].second = -1;
                around[3].first = search_color(-1); 

            }
            //coordの右下
            if((i != (width - 1)) && (j != (height - 1))){
                around[4].second = j * width + i + width + 1;
                around[4].first = search_color(j * width + i + width + 1); 
                if(around[4].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[4].second = -1;
                around[4].first = search_color(-1); 
            }
            //coordの下
            if(j != (height - 1)){
                around[5].second = j * width + i + width;
                around[5].first = search_color(j * width + i + width); 
                if(around[5].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[5].second = -1;
                around[5].first = search_color(-1); 
            }
            //coordの左下
            if((j != (height - 1)) && (i != 0)){
                around[6].second = j * width + i + width - 1;
                around[6].first = search_color(j * width + i + width - 1); 
                if(around[6].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[6].second = -1;
                around[6].first = search_color(-1); 
            }
            //coordの左
            if(i != 0){
                around[7].second = j * width + i - 1;
                around[7].first = search_color(j * width + i + width - 1); 
                if(around[7].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[7].second = -1;
                around[7].first = search_color(-1); 
            }
            //coordの左上
            if((i != 0) && (j != 0)){
                around[8].second = j * width + i - width - 1;
                around[8].first = search_color(j * width + i - width - 1); 
                if(around[8].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::red2), accel);
                }
            }
            else{
                around[8].second = -1;
                around[8].first = search_color(-1); 
            }
            break;
            }
        }
        //自陣は青
        else if(player_color == state::blue){
            switch(static_cast<int>(coord)){
            case 1 :
            j = blue1 / width; 
            i = blue1 - j * width;
            around[0].second = j * width + i;
            around[0].first = search_color(j * width + i); 
            player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);

            std::cout << j << "," << i << std::endl;
            //coordの上の座標
            if(j != 0){
                around[1].second = j * width + i - width;
                around[1].first = search_color(j * width + i - width); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[1].second = -1;
                around[1].first = search_color(-1); 

            }
            //coordの右上の座標
            if((j != 0) && (i != width - 1)){
                around[2].second = j * width + i - width + 1;
                around[2].first = search_color(j * width + i - width + 1); 
                if(around[2].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[2].second = -1;
                around[2].first = search_color(-1); 
            }
            //coordの右の座標
            if(i != (width - 1)){
                around[3].second = j * width + i + 1;
                around[3].first = search_color(j * width + i + 1); 
                if(around[3].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[3].second = -1;
                around[3].first = search_color(-1); 

            }
            //coordの右下
            if((i != (width - 1)) && (j != (height - 1))){
                around[4].second = j * width + i + width + 1;
                around[4].first = search_color(j * width + i + width + 1); 
                if(around[4].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[4].second = -1;
                around[4].first = search_color(-1); 
            }
            //coordの下
            if(j != (height - 1)){
                around[5].second = j * width + i + width;
                around[5].first = search_color(j * width + i + width); 
                if(around[5].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[5].second = -1;
                around[5].first = search_color(-1); 
            }
            //coordの左下
            if((j != (height - 1)) && (i != 0)){
                around[6].second = j * width + i + width - 1;
                around[6].first = search_color(j * width + i + width - 1); 
                if(around[6].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[6].second = -1;
                around[6].first = search_color(-1); 
            }
            //coordの左
            if(i != 0){
                around[7].second = j * width + i - 1;
                around[7].first = search_color(j * width + i + width - 1); 
                if(around[7].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[7].second = -1;
                around[7].first = search_color(-1); 
            }
            //coordの左上
            if((i != 0) && (j != 0)){
                around[8].second = j * width + i - width - 1;
                around[8].first = search_color(j * width + i - width - 1); 
                if(around[8].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue1), accel);
                }
            }
            else{
                around[8].second = -1;
                around[8].first = search_color(-1); 
            }
            break;

            //プレイヤー２
            case 2 :
            j = blue2 / width; 
            i = blue2 - j * width;
            around[0].second = j * width + i;
            around[0].first = search_color(j * width + i); 
            player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);

            std::cout << j << "," << i << std::endl;
            //coordの上の座標
            if(j != 0){
                around[1].second = j * width + i - width;
                around[1].first = search_color(j * width + i - width); 
                if(around[1].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[1].second = -1;
                around[1].first = search_color(-1); 

            }
            //coordの右上の座標
            if((j != 0) && (i != width - 1)){
                around[2].second = j * width + i - width + 1;
                around[2].first = search_color(j * width + i - width + 1); 
                if(around[2].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[2].second = -1;
                around[2].first = search_color(-1); 
            }
            //coordの右の座標
            if(i != (width - 1)){
                around[3].second = j * width + i + 1;
                around[3].first = search_color(j * width + i + 1); 
                if(around[3].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[3].second = -1;
                around[3].first = search_color(-1); 

            }
            //coordの右下
            if((i != (width - 1)) && (j != (height - 1))){
                around[4].second = j * width + i + width + 1;
                around[4].first = search_color(j * width + i + width + 1); 
                if(around[4].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[4].second = -1;
                around[4].first = search_color(-1); 
            }
            //coordの下
            if(j != (height - 1)){
                around[5].second = j * width + i + width;
                around[5].first = search_color(j * width + i + width); 
                if(around[5].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[5].second = -1;
                around[5].first = search_color(-1); 
            }
            //coordの左下
            if((j != (height - 1)) && (i != 0)){
                around[6].second = j * width + i + width - 1;
                around[6].first = search_color(j * width + i + width - 1); 
                if(around[6].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[6].second = -1;
                around[6].first = search_color(-1); 
            }
            //coordの左
            if(i != 0){
                around[7].second = j * width + i - 1;
                around[7].first = search_color(j * width + i + width - 1); 
                if(around[7].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[7].second = -1;
                around[7].first = search_color(-1); 
            }
            //coordの左上
            if((i != 0) && (j != 0)){
                around[8].second = j * width + i - width - 1;
                around[8].first = search_color(j * width + i - width - 1); 
                if(around[8].first == static_cast<int>(enmy_color)){
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), del);
                }
                else{
                    player_stage(i, j, static_cast<short int>(state3_stage::player_num::blue2), accel);
                }
            }
            else{
                around[8].second = -1;
                around[8].first = search_color(-1); 
            }
            break;
            }
        }
        for(int i = 0; i < 9; i++){
            std::cout << around[i].second << ",";
        }
    }

    //プレイヤーの座標変更
    void operator ()(short x, short y, short int v, int action){
    int i = y * width + x;
    unsigned int color;
    //移動、その場
    if(action == 1 || action == 0){
        if(v == 1){
        color = static_cast<int>(state::red);
        state_player[red1] = 0;
        state_player[i] = static_cast<short int>(v);
        ban_table[i] = color;
        red1 = i;        
        }
        if((v > 1) & (v < 3) == 1){
        color = static_cast<int>(state::red);
        state_player[red2] = 0;
        state_player[i] = static_cast<short int>(v);
        ban_table[i] = color;
        red2 = i;        
        }
        if((v > 2) & (v < 4) == 1){
        color = static_cast<int>(state::blue);
        state_player[blue1] = 0;
        state_player[i] = static_cast<short int>(v);
        ban_table[i] = color;
        blue1 = i;
        }
        if((v > 3) == 1){
        color = static_cast<int>(state::blue);
        state_player[blue2] = 0;
        state_player[i] = static_cast<short int>(v);
        ban_table[i] = color;
        blue2 = i;       
        }
    }
    //除去
    else if(action == 2){
        color = static_cast<int>(state::null);
        ban_table[i] = color;
    }
    else if(action == 3){
        state_player[i] = static_cast<short int>(v);
        ban_table[i] = color;
    }
    }
};

std::size_t input(const std::string &str){
    std::string a;
    std::size_t n;
    while(true){
        std::cout << ">> ";
        std::cin >> a;
        n = str.find(a[0]);
        if(n == std::string::npos){
            continue;
        }
        break;
    }
    return n;
}

//テスト用表示
void disp(){
    std::cout << "パネル盤面" << std::endl;
    for(int i = 0;i < 10; i++){
        for(int j = 0; j < 10; j++){
            std::cout << ban_table[i * width + j];
        }
    std::cout << "\n";
    }

    std::cout << "プレイヤー盤面" << std::endl;
    for(int i = 0;i < 10; i++){
        for(int j = 0; j < 10; j++){
            std::cout << state_player[i * width + j];
        }
    std::cout << "\n";
    }

    std::cout << "プレイヤー盤面" << std::endl;
    for(int i = 0;i < 10; i++){
        for(int j = 0; j < 10; j++){
            std::cout << panel_point[i * width + j] << ",";
        }
    std::cout << "\n";
    }
}

void game(){
    //何手先まで読むかの設定数値この状態だと1ターン先まで考える
    int level = 1;
    int move = 1, del = 2;
    state3_stage s;
    //赤は"1"青は"2"という形で扱っていく
    std::cout << "red:1, blue:2" << std::endl;
    int p_color = input("12") + 1;
    
    if(p_color == static_cast<int>(state3_stage::panel_color::red)){
    s.player_color = state3_stage::state::red;
    }
    else{
        s.player_color = state3_stage::state::blue;
    }
    s.enmy_color = state3_stage::reverse(s.player_color);

    //初めの4つのパネル設置&&初期位置決定
    s(red1.first,red1.second,static_cast<short int>(state3_stage::player_num::red1),move);
    s(red2.first,red2.second,static_cast<short int>(state3_stage::player_num::red2),move);
    s(blue1.first,blue1.second,static_cast<short int>(state3_stage::player_num::blue1),move);
    s(blue2.first,blue2.second,static_cast<short int>(state3_stage::player_num::blue2),move);

    //探索開始
    s.compute1(level,p_color);
    std::cout << "(" << "s.opt_hand.i:" << s.opt_hand.i << ",s.opt_hand.v:" << s.opt_hand.v << ")" << std::endl;   
}

int main(void){
    //テスト用
    std::srand( time(NULL) );
    for(int i = 0; i < width * height;i++){
        panel_point[i] = rand() % 20;
    }
	panel_point[0] = -15;
    /*
    ban_table[0] = 1;
    ban_table[11] = 1;
    ban_table[13] = 1;
    ban_table[14] = 1;
    ban_table[20] = 1;
    ban_table[23] = 1;
    ban_table[25] = 1;
    ban_table[33] = 1;
    ban_table[35] = 1;
    ban_table[40] = 1;
    ban_table[41] = 1;
    ban_table[43] = 1;
    ban_table[45] = 1;
    ban_table[50] = 1;
    ban_table[53] = 1;
    ban_table[56] = 1;
    ban_table[60] = 1;
    ban_table[61] = 1;
ban_table[63] = 1;
ban_table[64] = 1;
ban_table[65] = 1;
*/
    ban_table[12] = 1;
    ban_table[13] = 1;
    ban_table[14] = 1;
    ban_table[15] = 1;
    ban_table[16] = 1;
    ban_table[21] = 1;
    ban_table[27] = 1;
    ban_table[31] = 1;
    ban_table[37] = 1;
    ban_table[41] = 1;
    ban_table[47] = 1;
    ban_table[51] = 1;
    ban_table[57] = 1;
    ban_table[62] = 1;
    ban_table[63] = 1;
    ban_table[64] = 1;
    ban_table[65] = 1;
    ban_table[66] = 1;
    ban_table[67] = 1;



    

    disp();
    game();
    
    

    getchar();
    getchar();

    return 0;
}