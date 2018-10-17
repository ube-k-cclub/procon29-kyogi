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
#include <string.h>
#include <iterator>
#include <math.h>                                                               



//UIの生成時にデータを入れてほしい者たち

//盤面のサイズ(生成時の値をこちらに代入)
//========================================================UIから
const std::size_t width = 10, height = 10;

//パネルの位置を保存しておく
unsigned int ban_table[width * height] = {0};

//プレイヤー座標
unsigned int state_player[width * height] = {0};


//マスの得点
//=========================================================UIから
unsigned int panel_point[width * height] = {0};


//プレイヤーの周りの座標({null、敵パネル、自パネル}, 座標)
std::pair<int , int> around[9];
std::pair<int , int> around2[9];

 
//プレイヤー座標兼初期座標
//=========================================================UIから
std::pair<int, int> red1 = std::make_pair(2,1);
std::pair<int, int> red2 = std::make_pair(7,8);
std::pair<int, int> blue1 = std::make_pair(2,8);
std::pair<int, int> blue2 = std::make_pair(7,1);

//ターン数(こっちも代入)　今は仮
//=========================================================UIから
static const std::size_t turn = 100;

//ギア(ギアシステム)
//=========================================================UIから
int gear = 1;

//入力された座標の色を返す
int search_color(int num)
{
	switch (ban_table[num])
	{
	case 0: return 0;
	case 1: return 1;
	case 2: return 2;
	default: return 0;
	}
}

struct state3_stage
{
	//パネルの色
	enum class state : char
	{
		null,
		red,
		blue
	};

	enum class player_num : char
	{
		null,
		red1,
		red2,
		blue1,
		blue2
	};

	//プレイヤーの座標
	int red1 = 12;
	int red2 = 82;
	int blue1 = 17;
	int blue2 = 87;

	//現在の盤面の得点
	int red_avail;
	int blue_avail;

	//現在の盤面の評価
	unsigned int panel_evalution[width * height] = { 0 };


	//自陣と敵色の判別
	state3_stage::state player_color, enemy_color;

	//色を逆転させる
	state reverse(state color)
	{
		state rev;
		if (color == state::red)
		{
			rev = state::blue;
		}
		else
		{
			rev = state::red;
		}

		return rev;
	}

	void compute1(int level, int color)
	{
		player_num player1;
		state player;
		int player_pos = 0;
		if (color == static_cast<int>(state::red))
		{
			player1 = player_num::red1;
			player = state::red;
			player_pos = red1;
		}
		else if (color == static_cast<int>(state::blue))
		{
			player1 = player_num::blue1;
			player = state::blue;
			player_pos = blue1;
		}
		else
		{
			std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
		}

		switch (gear)
		{
		case 1:
			 move(player, player_pos); //どう動くか
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
		}
	}

	void move(state player, int player_pos_)
	{
		static int count_move[4] = {1,1,1,1}; //このcount_moveの要素数は読む深さを現している。初めはすべて"1"

		move_range(count_move[0], count_move[1], count_move[2], count_move[3], player_pos_, player);
		count_move[3]++;
		if (count_move[3] == 9)
		{
			count_move[3] = 1;
			count_move[2]++;
		}
		if (count_move[2] == 9)
		{
			count_move[2] = 1;
			count_move[1]++;
		}
		if (count_move[1] == 9)
		{
			count_move[1] = 1;
			count_move[0]++;
		}
		if (count_move[0] == 9)
		{
		//count_move[0] = 1;count_move[1] = 1;count_move[2] = 1;count_move[3] = 1;
			return;
		}
		move(player, player_pos_);
	}

	void move_range(int layer1, int layer2, int layer3, int layer4, int player_pos, state color)
	{
		static int count = 0;
		static int line[4] = { 0 };
		int line_tmp[4] = { 0 };
		static int i = 0, j = 0;
		static bool finish = false;
		j = static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width)));
		i = static_cast<int>(floor(static_cast<float>(player_pos) - (static_cast<float>(j) * static_cast<float>(width))));
		if (j < 0) j = 0;
		if (i < 0)i = 0;
		state3_stage s;
		switch (s(count, layer1, layer2, layer3, layer4))
		{
			//上
		case 1:
			if (j != 0)
			{
				//移動したい場所
				line[count] = 1;
				//1つ上に上がる
				player_pos = j * width + i - width;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//右上
		case 2:
			if ((j != 0) && (i != width - 1))
			{
				//移動したい場所
				line[count] = 2;
				player_pos = j * width + i - width + 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//右
		case 3:
			if (i != (width - 1))
			{
				//移動したい場所
				line[count] = 3;
				player_pos = j * width + i + 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//右下
		case 4:
			if ((i != (width - 1)) && (j != (height - 1)))
			{
				//移動したい場所
				line[count] = 4;
				player_pos = j * width + i + width + 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//下
		case 5:
			if (j != (height - 1))
			{
				//移動したい場所
				line[count] = 5;
				player_pos = j * width + i + width;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//左下
		case 6:
			if ((j != (height - 1)) && (i != 0))
			{
				//移動したい場所
				line[count] = 6;
				player_pos = j * width + i + width - 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//左
		case 7:
			if (i != 0)
			{
				//移動したい場所
				line[count] = 7;
				player_pos = j * width + i - 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
			//左上
		case 8:
			if ((i != 0) && (j != 0))
			{
				//移動したい場所
				line[count] = 8;
				player_pos = j * width + i - width - 1;
			}
			else
			{
				//このルートは使えない
				finish = true;
			}
			break;
		}
		//4回終わった。1つのルートを最後まで通った・。・
		//この後そのルートを返却値として送る
		if(finish)
		{
			count = 0;
			i = 0;
			j = 0;
			int line[4] = {0,0,0,0};
			finish = false;
		}
		else if(count >= 3)
		{
			//探索終わり
			//staticの値をもとに戻す
			count = 0;
			i = 0;
			j = 0;
			memcpy(line_tmp, line, sizeof(int[4]));
			int line[4] = {0,0,0,0};
			//評価値を測る
			evalution1(line_tmp, color); //配列の名前だけで配列のアドレスとして機能する
		}
		else
		{
			//もう一度回らせる
			count++;
			move_range(layer1, layer2, layer3, layer4, player_pos, color);			
		}
	}


	void evalution1(int *root, state color)
	{
		int avail = 0;
		int aa[4] = { 0 };
		memcpy(aa, root, sizeof(int[4]));
	


		std::cout << aa[0] <<","<<aa[1]<<","<<aa[2]<<","<<aa[3] << std::endl;

	}

    //現在の得点の計算
    int calculation(state color){ //引数には１，２
		int sum=0;
		for (int i = 0; i < width * height; i++)
		{
			if (ban_table[i] == static_cast<int>(color))
			{
				sum += panel_point[i];
			}
		}
		sum += areapoint_sum(color); //領域ポイントの加算

		return sum;
    }

	//領域ポイントの計算
	int areapoint_sum(state player_color)
	{
		int sum = 0, time = 0;
		std::list<int> area;  //計算が遅くなるようであればvectorに変更
		int count = 0;
		//横の列を見る
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				if (ban_table[j * height + i] == static_cast<int>(player_color))
				{
					count++;
					if (time == 0)
					{
						time = 1;
					}
					else if (time == 2)
					{
						time = 3;
					}
				}
				else
				{
					if (time == 1) time = 2;
					//一度も自パネルを通ってないから領域ポイントから除外していい
					if (time == 0)
					{
						area.push_front(j * height + i);
					}
				}
			}
			if (count < 2 || time != 3)
			{
				for (int k = 0; k < width; k++)
				{
					area.push_front(j * height + k);
				}
			}
			else
			{
				for (int k = 0; k < width; k++)
				{
					if (ban_table[j * height + k] == static_cast<int>(player_color))
					{
						area.push_front(j * height + k);
					}
					if (k == 0 || k == width - 1)
					{
						area.push_front(j * height + k);
					}
				}
			}
			time = 0;
			count = 0;
		}
		//縦の列を見る
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				if (ban_table[i * width + j] == static_cast<int>(player_color))
				{
					count++;
					if (time == 0)
					{
						time = 1;
					}
					else if (time == 2)
					{
						time = 3;
					}
				}
				else
				{
					if (time == 1) time = 2;
					if (time == 0)
					{
						area.push_front(i * width + j);
					}
				}
			}
			if (count < 2 || time != 3)
			{
				for (int k = 0; k < width; k++)
				{
					area.push_front(k * width + j);
				}
			}
			else
			{
				for (int k = 0; k < width; k++)
				{

					if (ban_table[k * height + j] == static_cast<int>(player_color))
					{
						area.push_front(k * width + j);
					}
					if (k == 0 || k == width - 1)
					{
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
		for (itr = area.begin(); itr != area.end(); itr++, i++)
		{
			while (*itr != i)
			{
				in_area.push_front(i);
				i++;
			}
		}
		in_area.unique();
		std::list<int> in_area_copy;
		int k = 0;
		int ex = 0;
		while (in_area.size() != in_area_copy.size())
		{
			in_area_copy = in_area;
			for (itr = in_area.begin(); itr != in_area.end(); itr++)
			{
				for (int j = 0; j < 4; j++)
				{
					for (itr2 = in_area.begin(); itr2 != in_area.end(); itr2++, k++)
					{
						if (check(j, *itr) == *std::next(in_area.begin(), k))
						{
							ex++;
						}
					}
					if (ban_table[check(j, *itr)] == static_cast<int>(player_color))
					{
						ex++;
					}
					k = 0;
				}
				//!上下左右に自パネルか領域パネルになる可能性のあるパネルがある
				if (ex != 4)
				{
					in_area_copy.remove(*itr);
				}
				ex = 0;
			}
			if (in_area_copy.size() == 0)
			{
				std::cout << "領域ポイントはありません" << std::endl;
				in_area = in_area_copy;
				break;
			}
			auto tmp = in_area;
			in_area = in_area_copy;
			in_area_copy = tmp;
		}
		//領域ポイントの合計点
		for (itr = in_area.begin(); itr != in_area.end(); itr++)
		{
			if (panel_point[*itr] < 0)
			{
				sum += panel_point[*itr] * (-1);
			}
			else
			{
				sum += panel_point[*itr];
			}
		}

		std::cout << "sum == " << sum << std::endl;

		return sum;
	}
	//エリアポイント計算用
	int check(int num, int coord)
	{
		switch (num)
		{
		case 0: return coord - width; break;
		case 1: return coord + 1; break;
		case 2: return coord + width; break;
		case 3: return coord - 1; break;
		}
	}

	//ショートカット１
	int operator ()(int count_op, int layer1, int layer2, int layer3, int layer4)
	{
		switch (count_op)
		{
		case 0: return layer1; break;
		case 1: return layer2; break;
		case 2: return layer3; break;
		case 3: return layer4; break;
		}
	}
};


game(){
    int level = 4;
    //moveは行動、delは除去
    int move = 1, del = 2;
    state3_stage s;

	std::cout << "red:1, blue:2" << std::endl;
	//=================================================UIから
	int p_color = 1;  //赤なら１、青なら２をここに入力
	if (p_color == static_cast<int>(state3_stage::state::red))
	{
		s.player_color = state3_stage::state::red;
		s.enemy_color = state3_stage::state::blue;
	}
	else
	{
		s.player_color = state3_stage::state::blue;
		s.enemy_color = state3_stage::state::red;
	}

	int turn_count = 0;
	while (turn > turn_count)
	{
		s.compute1(level, p_color);

		turn_count++;
	}
}

int main(void){
    game();

    getchar();
    getchar();

    return 0;
}