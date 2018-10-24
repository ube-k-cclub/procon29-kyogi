<<<<<<< HEAD
﻿/*
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
#include <fstream>
#include <algorithm>  // std::sort, std::unique



//UIの生成時にデータを入れてほしい者たち

//盤面のサイズ(生成時の値をこちらに代入)
//========================================================UIから
std::size_t width, height;

const int max_panel = 144;

//パネルの位置を保存しておく
int ban_table[max_panel];//===========================テキスト保持

//マスの得点
//=========================================================UIから
int panel_point[max_panel];

 
//プレイヤーの座標//=======================================================テキスト保持
int red1 = 12;
int red2 = 82;
int blue1 = 17;
int blue2 = 87;

//ターン数(こっちも代入)　今は仮
//=========================================================UIから
std::size_t turn = 100;

//プレイヤーカラー=================================================UIから
int p_color = 1;  //赤なら１、青なら２をここに入力

//ギア(ギアシステム)
//=========================================================UIから
int gear1 = 1;
int gear2 = 1;

//現在のターン数
int turn_count = 1;

//各チームの得点
int my_panel_point = 0;
int my_area_point = 0;
int my_sum_point = 0;
int enemy_panel_point = 0;
int enemy_area_point = 0;
int enemy_sum_point = 0;

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

	//現在の盤面の得点
	int red_avail;
	int blue_avail;

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

	int* compute1(int color)
	{
		const int num = 1;
		int *ad;
		int avail[4] = {0};
		state player;
		int player_pos = 0;
		if (color == static_cast<int>(state::red))
		{
			player = state::red;
			player_pos = red1;
		}
		else if (color == static_cast<int>(state::blue))
		{
			player = state::blue;
			player_pos = blue1;
		}
		else
		{
			std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
		}

		ad = move(player, player_pos, num); //どう動くか
		memcpy(avail, ad, sizeof(int[4]));
		std::cout << "こうやって動きたい:" << avail[0] << avail[1] << avail[2] << avail[3] << std::endl;

		return ad;
	}

	int* compute2(int color)
	{
		const int num = 2;
		int *ad;
		int avail[4] = { 0 };
		state player;
		int player_pos = 0;
		if (color == static_cast<int>(state::red))
		{
			player = state::red;
			player_pos = red2;
		}
		else if (color == static_cast<int>(state::blue))
		{
			player = state::blue;
			player_pos = blue2;
		}
		else
		{
			std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
		}

		ad = move(player, player_pos, num); //どう動くか
		memcpy(avail, ad, sizeof(int[4]));
		std::cout << "こうやって動きたい:" << avail[0] << avail[1] << avail[2] << avail[3] << std::endl;

		return ad;
	}

	//４ターンで動ける動作を全動作試してる
	int* move(state player, int player_pos_, int num)
	{
		static int next_avail = -1000;
		static int avail = 0;
		static int count_move[4] = {0,0,0,0}; //このcount_moveの要素数は読む深さを現している。初めはすべて"1"
		static int max_eva[4] = { 0,0,0,0 };
		static bool reset = false;
		if (num == 2 && !reset)
		{			
			next_avail = -1000;
			avail = 0;
			count_move[0] = 0;
			count_move[1] = 0;
			count_move[2] = 0;
			count_move[3] = 0;
			max_eva[0] = 0;
			max_eva[1] = 0;
			max_eva[2] = 0;
			max_eva[3] = 0;
			reset = true;
		}

		avail = move_range(count_move[0], count_move[1], count_move[2], count_move[3], player_pos_, player, num);

		//std::cout << avail << std::endl;

		if (avail > next_avail)
		{
			next_avail = avail;
			
			for (int i = 0; i < 4; i++)
			{
				max_eva[i] = count_move[i];
			}
		}

		count_move[3]++;
		if (count_move[3] == 9)
		{
			count_move[3] = 0;
			count_move[2]++;
		}
		if (count_move[2] == 9)
		{
			count_move[2] = 0;
			count_move[1]++;
		}
		if (count_move[1] == 9)
		{
			count_move[1] = 0;
			count_move[0]++;
		}
		if (count_move[0] == 9)
		{
			if (num == 1)
			{
				std::cout << "プレイヤー：１ギア:"<<gear1  << "最大値:" << next_avail << " 現在地:" << player_pos_ << std::endl;
			}
			else
			{
				std::cout << "プレイヤー：２ギア:" << gear2 << "最大値:" << next_avail << " 現在地:" << player_pos_ << std::endl;
			}
			return max_eva;
		}
		move(player, player_pos_, num);
	}

	//上の続きで壁などにぶつかったルート以外をギアに合わせて点数を評価する
	int move_range(int layer1, int layer2, int layer3, int layer4, int player_pos, state color, int num)
	{
		static int pos_tmp = player_pos;
		int avail = 0;
		static int count = 0;
		static int line[4] = { 0 };
		int line_tmp[4] = { 0 };
		static int i = 0, j = 0;
		static bool finish = false;
		static bool reset = false;
		//初期化1
		if (num == 2 && !reset)
		{
			std::cout << "リセット" << std::endl;
			pos_tmp = player_pos;
			count = 0;
			line[0] = 0;
			line[1] = 0;
			line[2] = 0;
			line[3] = 0;
			i = 0, j = 0;
			finish = false;
			reset = true;
		}
		j = static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width)));
		i = static_cast<int>(floor(static_cast<float>(player_pos) - (static_cast<float>(j) * static_cast<float>(width))));
		if (j < 0) j = 0;
		if (i < 0)i = 0;
		state3_stage s;
		switch (s(count, layer1, layer2, layer3, layer4))
		{
			//その場
		case 0:line[count] = 0;
			break;
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
			return avail;
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

			//ギアの判定
			//プレイヤー1
			if (num == 1)
			{
				switch (gear1)
				{
				case 1:
					//ギア1：斜め移動多様のはじめの動作
					//ギア１は計算がいまは甘い
					//std::cout << "ギア：1" << std::endl;
					avail = evalution1(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 2:
					//ギア2：外周周りを移動する中盤の動作
					//std::cout << "ギア：2" << std::endl;
					avail = evalution2(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 3:
					//ギア3：とにかく高い点数を取りまくる終盤の動作
					avail = evalution3(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					//avail = evalution3(line_tmp, color, player_pos); //配列の名前だけで配列のアドレスとして機能する
					break;
				default:
					//現在はほかのギアの予定はない
					break;
				}
			}
			//プレイヤー2
			else if(num == 2)
			{
				switch (gear2)
				{
				case 1:
					//ギア1：斜め移動多様のはじめの動作
					//ギア１は計算がいまは甘い
					//std::cout << "ギア：1" << std::endl;
					avail = evalution1(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 2:
					//ギア2：外周周りを移動する中盤の動作
					//std::cout << "ギア：2" << std::endl;
					avail = evalution2(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 3:
					//ギア3：とにかく高い点数を取りまくる終盤の動作
					avail = evalution3(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					//avail = evalution3(line_tmp, color, player_pos); //配列の名前だけで配列のアドレスとして機能する
					break;
				default:
					//現在はほかのギアの予定はない
					break;
				}
			}
			else
			{
				std::cout << "エラー：プレイヤーナンバーがおかしいです。" << std::endl;
				exit(1);
			}
			return avail;
		}
		else
		{
			//もう一度回らせる
			count++;
			move_range(layer1, layer2, layer3, layer4, player_pos, color, num);		
		}
	}


	//点数で返す(数字がギアと同じ数字)
	int evalution1(int *root, state color, int player_pos)
	{
		int avail = 0;
		int ban_table_gear[max_panel];
		int ban_table_eva[max_panel];
		int ban_table_copy[max_panel];
		int move[4] = {0};
		int player = 0;
		const int dia = 3; //探索するプレイヤーから見て斜めの位置に存在するパネルを示す値
		const int crs = 1; //探索するプレイヤーから見て十字の位置に存在するパネルを示す値
		const int best_point = 10; //とるパネルの点数としては最高
		const int middle_point = 5; //とるパネルの点数としては微妙
		const int worst_point = 0; //とるパネルの点数としては最悪
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//プレイヤー座標に対して斜めの場所をban_tabele_gearに各座標に"3"とする
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				//widthが偶数
				if (width % 2 == 0)
				{
					//プレイヤー座標が偶数
					if (player_pos % 2 == 0)
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
					}
					//プレイヤー座標が奇数
					else
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
					}
				}
				//widthが奇数
				else
				{
					//プレイヤー座標が偶数
					if (player_pos % 2 == 0)
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
					}
					//プレイヤー座標が奇数
					else
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
					}
				}
			}
		}
		//各パネルの評価値の決定
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				//ポイントとしてベスト
				if (panel_point[j * width + i] > 4 && panel_point[j * width + i] < 10)
				{
					ban_table_eva[j * width + i] = best_point * ban_table_gear[j * width + i];
				}
				//妥協点
				else if(panel_point[j * width + i] > -1 && panel_point[j * width + i] < 17)
 				{
					ban_table_eva[j * width + i] = middle_point * ban_table_gear[j * width + i];
				}
				//マイナス値はありえない
				else
				{
					ban_table_eva[j * width + i] = worst_point * ban_table_gear[j * width + i];
				}	
			}
		}
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < sizeof(move) / sizeof(move[0]); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			}
		}
		//計算結果
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{	
				if (ban_table_copy[j * width + i] == static_cast<int>(color))
				{
					avail += ban_table_eva[j * width + i];
				}
			}
		}

		return avail;
	}

	int evalution2(int *root, state color, int player_pos)
	{
		int avail = 0;
		int ban_table_gear[max_panel];
		int ban_table_eva[max_panel];
		int ban_table_copy[max_panel];
		int move[4] = { 0 };
		int player = 0;
		const int outside = 5; //探索するプレイヤーから見て一番外周の位置に存在するパネルを示す値
		const int side = 2; //探索するプレイヤーから見て外周の1マス内側の位置に存在するパネルを示す値
		const int oth = 1; //探索するプレイヤーから見て角か内周の位置に存在するパネルを示す値
		const int best_point = 10; //とるパネルの点数としては最高
		const int middle_point = 3; //とるパネルの点数としては微妙
		const int bad_point = 1; //とるパネルの点数としてはわるい
		const int worst_point = 0; //とるパネルの点数としては最悪
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//std::cout << move[0]<< ","<<move[1]<<","<<move[2]<<","<<move[3]  << std::endl;
		//外周の得点を高くするようにする。角は点数低く
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				//角
				if ((i == 0 && j == 0) || (j == 0 && i == width - 1) || (j == height - 1 && i == 0) || (j == height - 1 && i == width - 1))
				{
					ban_table_gear[j * width + i] = oth;
				}
				//一番外周
				else if (j == 0 || j == height - 1 || i == 0 || i == width - 1)
				{
					ban_table_gear[j * width + i] = outside;
				}
				//1マス内側外周
				else if (j < 2 || j > height - 3 || i < 2 || i > width - 3)
				{
					if (j * width + i == 28)
					{
						//std::cout <<"!!!!!!!!!!"<< j * width + i << std::endl;
					}
					ban_table_gear[j * width + i] = side;
				}
				//他
				else
				{
					ban_table_gear[j * width + i] = oth;
				}
			}
		}
		//各パネルの評価値決定
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				//ポイントとしてベスト
				if (panel_point[j * width + i] > 9)
				{
					ban_table_eva[j * width + i] = best_point * ban_table_gear[j * width + i];
				}
				//妥協点
				else if (panel_point[j * width + i] > 4)
				{
					ban_table_eva[j * width + i] = middle_point * ban_table_gear[j * width + i];
				}
				//マイナス値よりはいい
				else if(panel_point[j * width + i] > -1)
				{
					ban_table_eva[j * width + i] = bad_point * ban_table_gear[j * width + i];
				}
				//マイナス値はありえない
				else
				{
					ban_table_eva[j * width + i] = worst_point * ban_table_gear[j * width + i];
				}
			}
		}
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < sizeof(move) / sizeof(move[0]); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			}
		}
		//計算結果
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				if (ban_table_copy[j * width + i] == static_cast<int>(color))
				{
					avail += ban_table_eva[j * width + i];
				}
			}
		}
		return avail;
	}

	int evalution3(int *root, state color, int player_pos)
	{
		int sum = 0;
		int ban_table_copy[max_panel];
		int move[4] = { 0 };
		int player = 0;
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < sizeof(move) / sizeof(move[0]); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			}
		}
		//点数計算=====================================================================対戦相手との比較型のほうがいいかもというかいい
		sum = calculation(color, ban_table_copy) - calculation(reverse(color), ban_table_copy);

		return sum;
	}


    //現在の得点の計算
    int calculation(state color,int *table){ //引数には１，２
		int sum=0;
		int table_tmp[max_panel];
		memcpy(table_tmp, table, sizeof(table_tmp));
		for (int i = 0; i < width * height; i++)
		{
			if (table_tmp[i] == static_cast<int>(color))
			{
				sum += panel_point[i];
			}
		}

		if (static_cast<int>(color) == p_color)
		{
			my_panel_point = sum;
		}
		else
		{
			enemy_panel_point = sum;
		}

		sum += areapoint_sum(color,table_tmp); //領域ポイントの加算

		return sum;
    }
	//領域ポイントの計算
	int areapoint_sum(state player_color, int *table)
	{
		int table_tmp[max_panel];
		memcpy(table_tmp, table, sizeof(table_tmp));
		int count = 0;
		int sum = 0, time = 0;
		std::list<int> area;  //計算が遅くなるようであればvectorに変更
		//横の列を見る
		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				if (table_tmp[j * height + i] == static_cast<int>(player_color))
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
					if (table_tmp[j * height + k] == static_cast<int>(player_color))
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
				if (table_tmp[i * width + j] == static_cast<int>(player_color))
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

					if (table_tmp[k * height + j] == static_cast<int>(player_color))
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
					if (table_tmp[check(j, *itr)] == static_cast<int>(player_color))
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
		//今計算しているのがプレイヤーの色ならば
		if (static_cast<int>(player_color) == p_color)
		{
			my_area_point = sum;
		}
		//今計算しているのが敵プレイヤーの色ならば
		else
		{
			enemy_area_point = sum;
		}

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


void game(){

	state3_stage s;
	int list1[4] = {0};
	int list2[4] = {0};
	int *ad;

	//現在のターンが設定したターンよりも少なければ続ける
	if (turn >= turn_count)
	{
		ad = s.compute1(p_color);//配列アドレスの確保
		memcpy(list1, ad, sizeof(list1));
		ad = s.compute2(p_color);
		memcpy(list2, ad, sizeof(list2));

		//メインプログラム終了。GUIへの出力へ移行
		//現在の全体の点数の計算
		my_sum_point = s.calculation(state3_stage::state::red, ban_table);
		enemy_sum_point = s.calculation(state3_stage::state::blue, ban_table);

		std::ofstream ofs("output.txt");

		ofs << my_panel_point << std::endl;
		ofs << my_area_point << std::endl;
		ofs << my_sum_point << std::endl;
		ofs << enemy_panel_point << std::endl;
		ofs << enemy_area_point << std::endl;
		ofs << enemy_sum_point << std::endl;
		ofs << list1[0] << std::endl;
		ofs << list1[1] << std::endl;
		ofs << list1[2] << std::endl;
		ofs << list1[3] << std::endl;
		ofs << list2[0] << std::endl;
		ofs << list2[1] << std::endl;
		ofs << list2[2] << std::endl;
		ofs << list2[3] << std::endl;

		ofs.close();

	}
}

int main(void){
	std::ifstream con("connect.txt");
	std::ifstream set("setup.txt");
	std::string str;

	if (set.fail())
	{
		std::cout << "setup.txtが存在しません。動作を終了します。" << std::endl;
		exit(1);
	}
	else
	{
		int count = 0;
		while (getline(set, str))
		{
			switch (count)
			{
			case 0:turn = std::stoi(str); break;
			case 1:width = std::stoi(str); break;
			case 2:height = std::stoi(str); break;
			case 3:p_color = std::stoi(str); break;
			default:panel_point[count - 4] = std::stoi(str); break;
			}
			count++;
		}
	}

	if (con.fail())
	{
		std::cout << "connect.txtが存在しません。動作を終了します。" << std::endl;
		exit(1);
	}
	else
	{
		int count = 0;
		while (getline(con, str))
		{
			switch (count)
			{
			case 0:red1 = std::stoi(str); break;
			case 1:red2 = std::stoi(str); break;
			case 2:blue1 = std::stoi(str); break;
			case 3:blue2 = std::stoi(str); break;
			case 4:gear1 = std::stoi(str); break;
			case 5:gear2 = std::stoi(str); break;
			case 6:turn_count = std::stoi(str); break;
			default:ban_table[count - 7] = std::stoi(str); break;
			}
			count++;
		}
	}

	//ファイル閉じる
	con.close();
	set.close();

	//ここまで来たらセットアップ完了。メインプログラムへ移行
    game();
	

    return 0;
}

=======
﻿/*
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
#include <fstream>
#include <algorithm>  // std::sort, std::unique



//UIの生成時にデータを入れてほしい者たち

//盤面のサイズ(生成時の値をこちらに代入)
//========================================================UIから
std::size_t width, height;

const int max_panel = 144;

//パネルの位置を保存しておく
int ban_table[max_panel];//===========================テキスト保持

//マスの得点
//=========================================================UIから
int panel_point[max_panel];

 
//プレイヤーの座標//=======================================================テキスト保持
int red1 = 12;
int red2 = 82;
int blue1 = 17;
int blue2 = 87;

//ターン数(こっちも代入)　今は仮
//=========================================================UIから
std::size_t turn = 0;

//プレイヤーカラー=================================================UIから
int p_color = 1;  //赤なら１、青なら２をここに入力

//ギア(ギアシステム)
//=========================================================UIから
int gear1 = 1;
int gear2 = 1;

//現在のターン数
int turn_count = 1;

//各チームの得点
int my_panel_point = 0;
int my_area_point = 0;
int my_sum_point = 0;
int enemy_panel_point = 0;
int enemy_area_point = 0;
int enemy_sum_point = 0;

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

	//現在の盤面の得点
	int red_avail;
	int blue_avail;

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

	int* compute1(int color)
	{
		const int num = 1;
		int *ad;
		int avail[4] = {0};
		state player;
		int player_pos = 0;
		if (color == static_cast<int>(state::red))
		{
			player = state::red;
			player_pos = red1;
		}
		else if (color == static_cast<int>(state::blue))
		{
			player = state::blue;
			player_pos = blue1;
		}
		else
		{
			std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
		}

		ad = move(player, player_pos, num); //どう動くか
		memcpy(avail, ad, sizeof(int[4]));
		std::cout << "こうやって動きたい:" << avail[0] << avail[1] << avail[2] << avail[3] << std::endl;

		return ad;
	}

	int* compute2(int color)
	{
		const int num = 2;
		int *ad;
		int avail[4] = { 0 };
		state player;
		int player_pos = 0;
		if (color == static_cast<int>(state::red))
		{
			player = state::red;
			player_pos = red2;
		}
		else if (color == static_cast<int>(state::blue))
		{
			player = state::blue;
			player_pos = blue2;
		}
		else
		{
			std::cout << "エラー:プレイヤーの色の判別ができませんでした" << std::endl;
		}

		ad = move(player, player_pos, num); //どう動くか
		memcpy(avail, ad, sizeof(int[4]));
		std::cout << "こうやって動きたい:" << avail[0] << avail[1] << avail[2] << avail[3] << std::endl;

		return ad;
	}

	//４ターンで動ける動作を全動作試してる
	int* move(state player, int player_pos_, int num)
	{
		static int next_avail = -1000;
		static int avail = 0;
		static int count_move[4] = {0,0,0,0}; //このcount_moveの要素数は読む深さを現している。初めはすべて"1"
		static int max_eva[4] = { 0,0,0,0 };
		static bool reset = false;
		if (num == 2 && !reset)
		{			
			next_avail = -1000;
			avail = 0;
			count_move[0] = 0;
			count_move[1] = 0;
			count_move[2] = 0;
			count_move[3] = 0;
			max_eva[0] = 0;
			max_eva[1] = 0;
			max_eva[2] = 0;
			max_eva[3] = 0;
			reset = true;
		}

		avail = move_range(count_move[0], count_move[1], count_move[2], count_move[3], player_pos_, player, num);

		//std::cout << avail << std::endl;

		if (avail > next_avail)
		{
			next_avail = avail;
			
			for (int i = 0; i < 4; i++)
			{
				max_eva[i] = count_move[i];
			}
		}

		count_move[3]++;
		if (count_move[3] == 9)
		{
			count_move[3] = 0;
			count_move[2]++;
		}
		if (count_move[2] == 9)
		{
			count_move[2] = 0;
			count_move[1]++;
		}
		if (count_move[1] == 9)
		{
			count_move[1] = 0;
			count_move[0]++;
		}
		if (count_move[0] == 9)
		{
			if (num == 1)
			{
				std::cout << "プレイヤー：１ギア:"<<gear1  << "最大値:" << next_avail << " 現在地:" << player_pos_ << std::endl;
			}
			else
			{
				std::cout << "プレイヤー：２ギア:" << gear2 << "最大値:" << next_avail << " 現在地:" << player_pos_ << std::endl;
			}
			return max_eva;
		}
		move(player, player_pos_, num);
	}

	//上の続きで壁などにぶつかったルート以外をギアに合わせて点数を評価する
	int move_range(int layer1, int layer2, int layer3, int layer4, int player_pos, state color, int num)
	{
		static int pos_tmp = player_pos;
		int avail = 0;
		static int count = 0;
		static int line[4] = { 0 };
		int line_tmp[4] = { 0 };
		static int i = 0, j = 0;
		static bool finish = false;
		static bool reset = false;
		//初期化1
		if (num == 2 && !reset)
		{
			std::cout << "リセット" << std::endl;
			pos_tmp = player_pos;
			count = 0;
			line[0] = 0;
			line[1] = 0;
			line[2] = 0;
			line[3] = 0;
			i = 0, j = 0;
			finish = false;
			reset = true;
		}
		j = static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width)));
		i = static_cast<int>(floor(static_cast<float>(player_pos) - (static_cast<float>(j) * static_cast<float>(width))));
		if (j < 0) j = 0;
		if (i < 0)i = 0;
		state3_stage s;
		switch (s(count, layer1, layer2, layer3, layer4))
		{
			//その場
		case 0:line[count] = 0;
			break;
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
			if (j != 0 && i != static_cast<int>(width) - 1)
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
			if (i != static_cast<int>(width) - 1)
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
			if (i != static_cast<int>(width) - 1 && j != static_cast<int>(height) - 1)
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
			if (j != static_cast<int>(height) - 1)
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
			if ((j != static_cast<int>(height) - 1) && (i != 0))
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
			finish = false;
			return avail;
		}
		else if(count >= 3)
		{
			//探索終わり
			//staticの値をもとに戻す
			count = 0;
			i = 0;
			j = 0;
			memcpy(line_tmp, line, sizeof(int[4]));

			//ギアの判定
			//プレイヤー1
			if (num == 1)
			{
				switch (gear1)
				{
				case 1:
					//ギア1：斜め移動多様のはじめの動作
					//ギア１は計算がいまは甘い
					//std::cout << "ギア：1" << std::endl;
					avail = evalution1(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 2:
					//ギア2：外周周りを移動する中盤の動作
					//std::cout << "ギア：2" << std::endl;
					avail = evalution2(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 3:
					//ギア3：とにかく高い点数を取りまくる終盤の動作
					avail = evalution3(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					//avail = evalution3(line_tmp, color, player_pos); //配列の名前だけで配列のアドレスとして機能する
					break;
				default:
					//現在はほかのギアの予定はない
					break;
				}
			}
			//プレイヤー2
			else if(num == 2)
			{
				switch (gear2)
				{
				case 1:
					//ギア1：斜め移動多様のはじめの動作
					//ギア１は計算がいまは甘い
					//std::cout << "ギア：1" << std::endl;
					avail = evalution1(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 2:
					//ギア2：外周周りを移動する中盤の動作
					//std::cout << "ギア：2" << std::endl;
					avail = evalution2(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					break;
				case 3:
					//ギア3：とにかく高い点数を取りまくる終盤の動作
					avail = evalution3(line_tmp, color, pos_tmp); //配列の名前だけで配列のアドレスとして機能する
					//avail = evalution3(line_tmp, color, player_pos); //配列の名前だけで配列のアドレスとして機能する
					break;
				default:
					//現在はほかのギアの予定はない
					break;
				}
			}
			else
			{
				std::cout << "エラー：プレイヤーナンバーがおかしいです。" << std::endl;
				exit(1);
			}
			return avail;
		}
		else
		{
			//もう一度回らせる
			count++;
			move_range(layer1, layer2, layer3, layer4, player_pos, color, num);		
		}
	}


	//点数で返す(数字がギアと同じ数字)
	//斜め、中くらいの点数を取る
	int evalution1(int *root, state color, int player_pos)
	{
		int avail = 0;
		int ban_table_gear[max_panel];
		int ban_table_eva[max_panel];
		int ban_table_copy[max_panel];
		int move[4] = {0};
		int player = 0;
		const int dia = 3; //探索するプレイヤーから見て斜めの位置に存在するパネルを示す値
		const int crs = 1; //探索するプレイヤーから見て十字の位置に存在するパネルを示す値
		const int best_point = 10; //とるパネルの点数としては最高
		const int middle_point = 5; //とるパネルの点数としては微妙
		const int worst_point = 0; //とるパネルの点数としては最悪
		const int center = 3;
		const int out = 1;
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//プレイヤー座標に対して斜めの場所をban_tabele_gearに各座標に"3"とする
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				//widthが偶数
				if (static_cast<int>(width) % 2 == 0)
				{
					//プレイヤー座標が偶数
					if (player_pos % 2 == 0)
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
					}
					//プレイヤー座標が奇数
					else
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
					}
				}
				//widthが奇数
				else
				{
					//プレイヤー座標が偶数
					if (player_pos % 2 == 0)
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
						}
					}
					//プレイヤー座標が奇数
					else
					{
						//プレイヤー座標がｊの偶数列
						if (static_cast<int>(floor(static_cast<float>(player_pos) / static_cast<float>(width))) % 2 == 0)
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
						//プレイヤー座標がｊの奇数列
						else
						{
							//jが偶数
							if (j % 2 == 0)
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = crs;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = dia;
								}
							}
							//jが奇数
							else
							{
								//iが偶数
								if (i % 2 == 0)
								{
									ban_table_gear[j * width + i] = dia;
								}
								//iが奇数
								else
								{
									ban_table_gear[j * width + i] = crs;
								}
							}
						}
					}
				}
			}
		}
		//各パネルの評価値の決定
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				//ポイントとしてベスト
				if (panel_point[j * width + i] > 4 && panel_point[j * width + i] < 10)
				{
					ban_table_eva[j * width + i] = best_point * ban_table_gear[j * width + i];
				}
				//妥協点
				else if (panel_point[j * width + i] > -1 && panel_point[j * width + i] < 17)
				{
					ban_table_eva[j * width + i] = middle_point * ban_table_gear[j * width + i];
				}
				//マイナス値はありえない
				else
				{
					ban_table_eva[j * width + i] = worst_point * ban_table_gear[j * width + i];
				}

				//中心に近いかどうか 
				if (static_cast<int>(width) / 2 + 2 > j && static_cast<int>(width) / 2 - 2 < j && static_cast<int>(height) / 2 + 2 > i && static_cast<int>(height) / 2 - 2 < i)
				{
					ban_table_eva[j * width + i] *= center;
				}
				else
				{
					ban_table_eva[j * width + i] *= out;
				}
			}
		}
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < static_cast<int>(sizeof(move) / sizeof(move[0])); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			}
		}
		//計算結果
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{	
				if (ban_table_copy[j * width + i] == static_cast<int>(color))
				{
					avail += ban_table_eva[j * width + i];
				}
			}
		}

		return avail;
	}
	//外周点数も高め優先
	int evalution2(int *root, state color, int player_pos)
	{
		int avail = 0;
		int ban_table_gear[max_panel];
		int ban_table_eva[max_panel];
		int ban_table_copy[max_panel];
		int move[4] = { 0 };
		int player = 0;
		const int outside = 5; //探索するプレイヤーから見て一番外周の位置に存在するパネルを示す値
		const int side = 2; //探索するプレイヤーから見て外周の1マス内側の位置に存在するパネルを示す値
		const int oth = 1; //探索するプレイヤーから見て角か内周の位置に存在するパネルを示す値
		const int best_point = 10; //とるパネルの点数としては最高
		const int middle_point = 3; //とるパネルの点数としては微妙
		const int bad_point = 1; //とるパネルの点数としてはわるい
		const int worst_point = 0; //とるパネルの点数としては最悪
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//std::cout << move[0]<< ","<<move[1]<<","<<move[2]<<","<<move[3]  << std::endl;
		//外周の得点を高くするようにする。角は点数低く
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				//角
				if ((i == 0 && j == 0) || (j == 0 && i == static_cast<int>(width) - 1) || (j == static_cast<int>(height) - 1 && i == 0) || (j == static_cast<int>(height) - 1 && i == static_cast<int>(width) - 1))
				{
					ban_table_gear[j * width + i] = oth;
				}
				//一番外周
				else if (j == 0 || j == static_cast<int>(height) - 1 || i == 0 || i == static_cast<int>(width) - 1)
				{
					ban_table_gear[j * width + i] = outside;
				}
				//1マス内側外周
				else if (j < 2 || j > static_cast<int>(height) - 3 || i < 2 || i > static_cast<int>(width) - 3)
				{
					ban_table_gear[j * width + i] = side;
				}
				//他
				else
				{
					ban_table_gear[j * width + i] = oth;
				}
			}
		}
		//各パネルの評価値決定
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				//ポイントとしてベスト
				if (panel_point[j * static_cast<int>(width) + i] > 9)
				{
					ban_table_eva[j * width + i] = best_point * ban_table_gear[j * width + i];
				}
				//妥協点
				else if (panel_point[j * static_cast<int>(width) + i] > 4)
				{
					ban_table_eva[j * width + i] = middle_point * ban_table_gear[j * width + i];
				}
				//マイナス値よりはいい
				else if(panel_point[j * static_cast<int>(width) + i] > -1)
				{
					ban_table_eva[j * width + i] = bad_point * ban_table_gear[j * width + i];
				}
				//マイナス値はありえない
				else
				{
					ban_table_eva[j * width + i] = worst_point * ban_table_gear[j * width + i];
				}
			}
		}
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < static_cast<int>(sizeof(move) / sizeof(move[0])); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					avail = 0;
					return avail;
				}
				break;
			}
		}
		//計算結果
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				if (ban_table_copy[j * static_cast<int>(width) + i] == static_cast<int>(color))
				{
					avail += ban_table_eva[j * width + i];
				}
			}
		}
		return avail;
	}
	//4ターンで動ける範囲で最高点になるように移動する
	int evalution3(int *root, state color, int player_pos)
	{
		int sum = 0;
		int ban_table_copy[max_panel];
		int move[4] = { 0 };
		int player = 0;
		memcpy(ban_table_copy, ban_table, sizeof(ban_table));
		memcpy(move, root, sizeof(int[4]));
		//実際に動いてみる
		player = player_pos;
		int dir = player;
		for (int i = 0; i < static_cast<int>(sizeof(move) / sizeof(move[0])); i++)
		{
			switch (move[i])
			{
			case 0:
				dir = player;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				//自陣の色
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 1:
				dir = player - width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 2:
				dir = player - width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 3:
				dir = player + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 4:
				dir = player + width + 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 5:
				dir = player + width;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 6:
				dir = player + width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 7:
				dir = player - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			case 8:
				dir = player - width - 1;
				//敵の色である
				if (ban_table_copy[dir] == static_cast<int>(reverse(color)))
				{
					ban_table_copy[dir] = static_cast<int>(state::null);
				}
				//何色でもない
				else if (ban_table_copy[dir] == static_cast<int>(state::null))
				{
					ban_table_copy[dir] = static_cast<int>(color);
					player = dir;
				}
				//自陣の色(あり得ない)
				else
				{
					ban_table_copy[dir] = static_cast<int>(color);
				}
				break;
			}
		}
		//点数計算=====================================================================対戦相手との比較型のほうがいいかもというかいい
		sum = calculation(color, ban_table_copy) - calculation(reverse(color), ban_table_copy);

		return sum;
	}
    //現在の得点の計算
    int calculation(state color,int *table){ //引数には１，２
		int sum=0;
		int table_tmp[max_panel];
		memcpy(table_tmp, table, sizeof(table_tmp));
		for (int i = 0; i < static_cast<int>(width) * static_cast<int>(height); i++)
		{
			if (table_tmp[i] == static_cast<int>(color))
			{
				sum += panel_point[i];
			}
		}

		if (static_cast<int>(color) == p_color)
		{
			my_panel_point = sum;
		}
		else
		{
			enemy_panel_point = sum;
		}

		sum += areapoint_sum(color,table_tmp); //領域ポイントの加算

		return sum;
    }
	//領域ポイントの計算
	int areapoint_sum(state player_color, int *table)
	{
		int table_tmp[max_panel];
		memcpy(table_tmp, table, sizeof(table_tmp));
		int count = 0;
		int sum = 0, time = 0;
		std::list<int> area;  //計算が遅くなるようであればvectorに変更
		//横の列を見る
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				if (table_tmp[j * static_cast<int>(height) + i] == static_cast<int>(player_color))
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
				for (int k = 0; k < static_cast<int>(width); k++)
				{
					area.push_front(j * height + k);
				}
			}
			else
			{
				for (int k = 0; k < static_cast<int>(width); k++)
				{
					if (table_tmp[j * static_cast<int>(height) + k] == static_cast<int>(player_color))
					{
						area.push_front(j * height + k);
					}
					if (k == 0 || k == static_cast<int>(width) - 1)
					{
						area.push_front(j * height + k);
					}
				}
			}
			time = 0;
			count = 0;
		}
		//縦の列を見る
		for (int j = 0; j < static_cast<int>(height); j++)
		{
			for (int i = 0; i < static_cast<int>(width); i++)
			{
				if (table_tmp[i * static_cast<int>(width) + j] == static_cast<int>(player_color))
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
				for (int k = 0; k < static_cast<int>(width); k++)
				{
					area.push_front(k * width + j);
				}
			}
			else
			{
				for (int k = 0; k < static_cast<int>(width); k++)
				{

					if (table_tmp[k * static_cast<int>(height) + j] == static_cast<int>(player_color))
					{
						area.push_front(k * width + j);
					}
					if (k == 0 || k == static_cast<int>(width) - 1)
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
					if (table_tmp[check(j, *itr)] == static_cast<int>(player_color))
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
		//今計算しているのがプレイヤーの色ならば
		if (static_cast<int>(player_color) == p_color)
		{
			my_area_point = sum;
		}
		//今計算しているのが敵プレイヤーの色ならば
		else
		{
			enemy_area_point = sum;
		}

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


void game(){

	state3_stage s;
	int list1[4] = {0};
	int list2[4] = {0};
	int *ad;

	//現在のターンが設定したターンよりも少なければ続ける
	if (static_cast<int>(turn) >= turn_count)
	{
		ad = s.compute1(p_color);//配列アドレスの確保
		memcpy(list1, ad, sizeof(list1));
		ad = s.compute2(p_color);
		memcpy(list2, ad, sizeof(list2));

		//メインプログラム終了。GUIへの出力へ移行
		//現在の全体の点数の計算
		my_sum_point = s.calculation(state3_stage::state::red, ban_table);
		enemy_sum_point = s.calculation(state3_stage::state::blue, ban_table);

		std::ofstream ofs("output.txt");

		ofs << my_panel_point << std::endl;
		ofs << my_area_point << std::endl;
		ofs << my_sum_point << std::endl;
		ofs << enemy_panel_point << std::endl;
		ofs << enemy_area_point << std::endl;
		ofs << enemy_sum_point << std::endl;
		ofs << list1[0] << std::endl;
		ofs << list1[1] << std::endl;
		ofs << list1[2] << std::endl;
		ofs << list1[3] << std::endl;
		ofs << list2[0] << std::endl;
		ofs << list2[1] << std::endl;
		ofs << list2[2] << std::endl;
		ofs << list2[3] << std::endl;

		ofs.close();

	}
}

int main(void){
	std::ifstream con("connect.txt");
	std::ifstream set("setup.txt");
	std::string str;

	if (set.fail())
	{
		std::cout << "setup.txtが存在しません。動作を終了します。" << std::endl;
		exit(1);
	}
	else
	{
		int count = 0;
		while (getline(set, str))
		{
			switch (count)
			{
			case 0:turn = std::stoi(str); break;
			case 1:width = std::stoi(str); break;
			case 2:height = std::stoi(str); break;
			case 3:p_color = std::stoi(str); break;
			default:panel_point[count - 4] = std::stoi(str); break;
			}
			count++;
		}
	}

	if (con.fail())
	{
		std::cout << "connect.txtが存在しません。動作を終了します。" << std::endl;
		exit(1);
	}
	else
	{
		int count = 0;
		while (getline(con, str))
		{
			switch (count)
			{
			case 0:red1 = std::stoi(str); break;
			case 1:red2 = std::stoi(str); break;
			case 2:blue1 = std::stoi(str); break;
			case 3:blue2 = std::stoi(str); break;
			case 4:gear1 = std::stoi(str); break;
			case 5:gear2 = std::stoi(str); break;
			case 6:turn_count = std::stoi(str); break;
			default:ban_table[count - 7] = std::stoi(str); break;
			}
			count++;
		}
	}

	//ファイル閉じる
	con.close();
	set.close();

	//ここまで来たらセットアップ完了。メインプログラムへ移行
    game();
	

    return 0;
}
>>>>>>> solve
