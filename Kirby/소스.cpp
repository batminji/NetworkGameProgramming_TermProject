#include "..\Server\stdafx.h"
#include "..\Server\protocol.h"
#include "fmod.hpp"
#include "fmod_errors.h"

using namespace std;
using namespace FMOD;
//using namespace FMOD;
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Kirby";
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

/////////////데이터////////////////////////////////
struct Data {
	TCHAR name[10];
	int money;
	int score;
};
vector <Data> all_data;
Data My;
void load(TCHAR ID[10]);
void save();
bool not_first = 0;
char name_c[10];
void load(TCHAR ID[10]) {
	std::ifstream in{ L"data.txt" };
	all_data.clear();
	TCHAR temp[10];
	while (in.good()) {
		    in >> name_c;
			if (strlen(name_c)<=0)
				continue;
			MultiByteToWideChar(CP_ACP, 0, name_c,10, temp, 10);
			if (_tcscmp(ID, temp) == 0) {
				_tcscpy(My.name, temp);
				in >> My.money >> My.score;
				all_data.emplace_back(My);
				not_first = 1;
			}
			else {
				Data insert_data;
				_tcscpy(insert_data.name, temp);
				in >> insert_data.money >> insert_data.score;
				all_data.emplace_back(insert_data);
			}
		
	}
	in.close();
	if (not_first == 0) {
		_tcscpy(My.name,ID);
		My.money = 0, My.score = 0;
		all_data.emplace_back(My);
	}
}

void save() {
	std::ofstream out{ L"data.txt" };
	for (Data& o : all_data) {
		
		if (_tcscmp(o.name, My.name) == 0) {
			o.score = My.score;
			o.money = My.money;
		}
		else {
			WideCharToMultiByte(CP_ACP, 0, o.name, 10, name_c, 10, NULL, NULL);
			out << name_c << " " << o.money << " " << o.score << std::endl;
		}
	}
	WideCharToMultiByte(CP_ACP, 0, My.name, 10, name_c, 10, NULL, NULL);
	out << name_c << " " <<My.money << " " <<My.score << std::endl;
	out.close();
	sort(all_data.begin(), all_data.end(), [](Data a, Data b)->bool{return a.score>b.score; });
}

/////////////////////////////////////////////////////////
struct Item {
	RECT rt; // 좌표값
	BOOL buy; // 구매 유무
	int buy_cnt; // 구매 & 취소
	int price; // 가격
	CImage img; // 게임 화면에 표시
};

struct Kirby {
	int x;
	int y;
	RECT rt;
	int xPos; // 사진 xPos
	BOOL skill; // skill 사용중이면 true
	int skill_cnt; // skill 4초 세는 변수
};

Kirby kirby; // 커비 구조체
int heart_cnt = 3; // 하트 갯수
int play_score = 0;//이번판의 점수
int play_gold = 0;//이번판의 돈
vector<POINT>bullet; // 일반 총알 벡터
vector<POINT>st_bullet; // 스킬 총알 벡터
void bullet_delete(); //화면나간 총알삭제
void bullet_delete() {
	for (int i = 0; i < st_bullet.size(); i++) {
		if (st_bullet[i].x < 0) {
			st_bullet.erase(st_bullet.begin() + i);
			return;
		}
	}
	for (int i = 0; i < bullet.size(); i++) {
		if (bullet[i].x < 0) {
			bullet.erase(bullet.begin() + i);
			return;
		}
	}
}

/////////////////코인//////////////////
vector <POINT> coins;

void getcoin();
void coinmove();
void coindelete();

BOOL get_coin_sound = FALSE;
void getcoin() {
	for (int i = 0; i < coins.size(); i++) {
		if (PtInRect(&kirby.rt, coins[i]) == 1) {
			play_gold += 10;
			coins.erase(coins.begin() + i);
			get_coin_sound = TRUE;
			return;
		}
	}
}
void coinmove() {
	for (POINT& c : coins) c.x += 30;
}
void coindelete() {
	for (int i = 0; i < coins.size(); i++) {
		if (coins[i].x > 800) {
			coins.erase(coins.begin() + i);
			return;
		}
	}
}
//////////////////////이펙트/////////////////////////////////
struct Effect {
	int type;
	RECT rt;
	int cnt;
};
vector <Effect> effect;
void effect_cnt() {
	for (Effect& e : effect) {
		e.cnt++;
	}
}
void effect_delete() {
	for (int i = 0; i < effect.size(); i++) {
		switch (effect[i].type)
		{case 0: //커비 총알 적한테 맞았을때
			if (effect[i].cnt >= 7) {
				effect.erase(effect.begin()+i);
				return;
			}
			break;
		case 1: // 적죽었을때 
			if (effect[i].cnt >= 12) {
				effect.erase(effect.begin() + i);
				return;
			}
			break;
		default:
			break;
		}
	}
}


/////////////////////////적 변수함수///////////////////////////////////////////////////////
struct enemy { //적구조체

	int type; //0:스몰 1~3:미드 4:빅
	RECT rt;
	int hp;
	int cnt;//탄막생성카운트
	int frame;
	int attak_type;
};

struct enemy_bullet {
	POINT xy;
	double xs;
	double ys;
	int speed;
};

vector <enemy> e;
vector <enemy_bullet> eb;
int clear_stage = 0; //클리어스테이지
int clear_set = 0;//클리어 세트 

// 4세트 = 1스테이지 

void create_enemy();
void hit_monster();
void die_monster();
void create_enemy_bullet();
void move_enemy_bullet();
void enemy_bullet_delete();
void monster_move();
void hit_me();
void Kirby_collide();

void create_enemy() {
	if (clear_set % 4 < 3) { //중간세트
		int mid3 = 0;
		if (clear_set % 4 == 2) mid3 = 1;
		//중간몬스터생성
		e.push_back({ (clear_set % 4 + 1),{100,180,100 + (45 + (mid3 * 5)) * 2,180 + (65 - (mid3 * 5)) * 2},50 + (clear_set % 4 * 10) + (clear_stage * 10),-20,3,-1 });
		//작은 몬스터 생성
		e.push_back({ 0,{220,100,220 + 42 * 2,100 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{280,265,280 + 42 * 2,265 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{220,450,220 + 42 * 2,450 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
	}
	else { //보스세트
	   //큰몬스터
		e.push_back({ 4,{10,150,10 + (128 * 2),150 + (112 * 2)},100 + (clear_stage * 20),-20 ,6,-1 });
		//작은 몬스터 생성
		e.push_back({ 0,{210,100,210 + 42 * 2,100 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{240,170,240 + 42 * 2,170 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{270,265,270 + 42 * 2,265 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{240,350,240 + 42 * 2,350 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
		e.push_back({ 0,{210,450,210 + 42 * 2,450 + 48 * 2},10 + (clear_stage * 5),-20,3,-1 });
	}
}
void hit_monster() {
	for (enemy& e : e) {
		for (int i = 0; i < bullet.size(); i++) {
			if (PtInRect(&e.rt, bullet[i]) == 1&& e.cnt >=0) {
				if (e.hp - 1 < 0) e.hp = 0;
				else e.hp--;
				effect.push_back({ 0,{bullet[i].x-15,bullet[i].y-15,bullet[i].x + 15,bullet[i].y + 15},0});
				play_score += 10;
				bullet.erase(bullet.begin() + i);
				return;
			}
		}
	}
	for (enemy& e : e) {
		for (int i = 0; i < st_bullet.size(); i++) {
			if (PtInRect(&e.rt, st_bullet[i]) == 1 && e.cnt >= 0) {
				if (e.hp - 2 < 0)e.hp = 0;
				else e.hp -= 2;
				effect.push_back({ 0,{st_bullet[i].x - 15,st_bullet[i].y - 15,st_bullet[i].x + 15,st_bullet[i].y + 15},0 });
				play_score += 20;
				st_bullet.erase(st_bullet.begin() + i);
				return;
			}
		}
	}
}

void die_monster() {
	for (int i = 0; i < e.size(); i++) {
		if (e[i].hp <= 0) {
			if (e[i].type == 0) coins.push_back({ e[i].rt.right,e[i].rt.top });
			effect.push_back({ 1,{e[i].rt},0});
			e.erase(e.begin() + i);
			return;
		}
	}
}
void create_enemy_bullet() {
	
	for (enemy& e : e) {
		e.cnt++;
		if (e.cnt > (10.0 - ((double)clear_stage / 2))) {

			switch (e.type)
			{
			case 0://작은몬스터 총알생성
				eb.push_back({ {e.rt.right,e.rt.top + 10},1.0,(double)(rand() % 6 - 3),1 });
				e.cnt = 0;
				break;
			case 4://큰몬스터 총알생성
				if (e.attak_type == -1) e.attak_type = rand() % 4;
				switch (e.attak_type)
				{
				case 0://원발사
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, 0, 3,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, 1, 2,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, 1,-2,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148},  2, 1,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148},  2,-1,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, -3,0,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, 3,0,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, -2, 1,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, -2,-1,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, -1, 2,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148}, -1,-2,1 });
					eb.push_back({ {e.rt.left + 122,e.rt.top + 148},0,-3,1 });
					break;
				case 1: //일직선발사
					for (int i = 0; i < 5; i++) eb.push_back({ {e.rt.right - 112,e.rt.top + 44 * i}, 3,0,1 });
					break;
				case 2: //랜덤발사
					for (int i = 0; i < 7; i++) {
						eb.push_back({ {e.rt.right - 122,e.rt.top + 118}, (double)(rand() % 6 - 3), (double)(rand() % 6 - 3),rand() % 3 + 1 });
						while (eb.back().xs == 0 && eb.back().ys == 0)eb.back().xs = rand() % 6 - 3, eb.back().ys = rand() % 6 - 3;
					}
					break;
				case 3: //회오리발사
					eb.push_back({ {e.rt.right - 122,e.rt.top + 118} ,cos(((e.cnt - 10) * 15) * M_PI / 180),sin((e.cnt - 10) * 15 * M_PI / 180),1 });
					break;
				default:
					break;
				}
				if ((e.attak_type != 3 && e.cnt > 10 + 5) || (e.attak_type == 3 && e.cnt > 10 + 200))e.cnt = 0, e.attak_type = -1;
				break;
			default://중간몬스터 총알생성
				if (e.attak_type == -1) e.attak_type = rand() % 4;
				switch (e.attak_type)
				{
				case 0://원발사
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, 0, 4,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, 2, 2,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, 2,-2,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65},-4,0,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, 4,0,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, -2, 2,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, -2,-2,1 });
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65}, 0,-4,1 });
					break;
				case 1: //일직선발사
					for (int i = 0; i < 3; i++) eb.push_back({ {e.rt.right - 45,e.rt.top + 40 * i + 20}, 3,0,1 });
					break;
				case 2: //랜덤발사
					for (int i = 0; i < 5; i++) {
						eb.push_back({ {e.rt.right - 122,e.rt.top + 118}, (double)(rand() % 6 - 3), (double)(rand() % 6 - 3),rand() % 3 + 1 });
						while (eb.back().xs == 0 && eb.back().ys == 0)eb.back().xs = rand() % 6 - 3, eb.back().ys = rand() % 6 - 3;
					}
					break;
				case 3: //회오리발사
					eb.push_back({ {e.rt.left + 45,e.rt.top + 65} ,cos(((e.cnt - 10) * 15) * M_PI / 180),sin((e.cnt - 10) * 15 * M_PI / 180),1 });
					break;
				default:
					break;
				}
				if ((e.attak_type != 3 && e.cnt > 10 + 5) || (e.attak_type == 3 && e.cnt > 10 + 200))e.cnt = 0, e.attak_type = -1;
				break;
			}

		}
	}
}

void move_enemy_bullet() {
	for (enemy_bullet& eb : eb) {
		eb.xy.x += 10 * eb.xs * eb.speed + (clear_stage * 2);
		eb.xy.y += 10 * eb.ys * eb.speed + (clear_stage * 2);
	}
}

void enemy_bullet_delete() {
	for (int i = 0; i < eb.size(); i++) {
		if (eb[i].xy.x < 0 || eb[i].xy.x> 800 || eb[i].xy.y < 0 || eb[i].xy.y>600) {
			eb.erase(eb.begin() + i);
			return;
		}
	}
}

void monster_move() {
	int wid, hei;
	static POINT goal = { rand() % 200 - 30, rand() % 400 + 50 };
	for (int i = 0; i < e.size(); i++) {
		switch (e[i].type)
		{
		case 0:
			e[i].rt.top += rand() % 40 - 20;
			e[i].rt.bottom = e[i].rt.top + 84;
			if (e[i].rt.top < 50)e[i].rt.top = 50, e[i].rt.bottom = e[i].rt.top + 84;
			if (e[i].rt.bottom > 600)e[i].rt.top = 600 - 84, e[i].rt.bottom = 600;
			break;
		default:
			if (e[i].attak_type != 3) {
				wid = e[i].rt.right - e[i].rt.left;
				hei = e[i].rt.bottom - e[i].rt.top;
				e[i].rt.left += (goal.x - e[i].rt.left) / sqrt(pow(goal.x - e[i].rt.left, 2) + pow(goal.y - e[i].rt.top, 2)) * 10;
				e[i].rt.right = e[i].rt.left + wid;
				e[i].rt.top += (goal.y - e[i].rt.top) / sqrt(pow(goal.x - e[i].rt.left, 2) + pow(goal.y - e[i].rt.top, 2)) * 10;
				e[i].rt.bottom = e[i].rt.top + hei;
				if (e[i].rt.left <= goal.x + 10 && e[i].rt.left >= goal.x - 10 && e[i].rt.top <= goal.y + 10 && e[i].rt.top >= goal.y - 10) goal = { rand() % 200 - 30, rand() % 400 + 50 };
			}
			break;
		}
	}
}

int zombie_cnt = 0;
void hit_me() {
	for (int i = 0; i < eb.size(); i++) {
		if (PtInRect(&kirby.rt, eb[i].xy) == 1 && zombie_cnt < 1) {
			if (heart_cnt > 0)heart_cnt--;
			eb.erase(eb.begin() + i);
			zombie_cnt = 1;
			return;
		}
	}
}

struct Missile {
	int mcnt; //상태를 나타내는 카운트 0~20: 경고 30:폭발 44:삭제
	RECT rt;
};
vector <Missile> missile;
int create_missile_cnt = 0;
void create_missile();
void hit_missile();
void move_missile();
void delete_missile();
void create_missile() {
	missile.push_back({ 0,{-80,kirby.y - 22,0, kirby.y + 22} });
}
RECT temprect;
BOOL hit_missile_sound = FALSE;
void hit_missile() {
	for (Missile& m : missile) {
		if (IntersectRect(&temprect, &m.rt, &kirby.rt) == 1 && zombie_cnt < 1) {
			if (heart_cnt > 0)heart_cnt -= 1;
			if (heart_cnt < 0) heart_cnt = 0;
			zombie_cnt = 1;
			m.mcnt = 30;
			hit_missile_sound = TRUE;
		}
	}
}
void move_missile() {
	//미사일이 움직이고 삭제되는함수
	for (int i = 0; i < missile.size(); i++) {
		if (missile[i].mcnt < 20) {
			missile[i].mcnt++;
			missile[i].rt.top = kirby.y - 22, missile[i].rt.bottom = kirby.y + 22;
		}
		else if (missile[i].mcnt >= 30) missile[i].mcnt++;
		else { //미사일이동
			missile[i].rt.left += 20 + 10 * ((clear_set + 1));
			missile[i].rt.right += 20 + 10 * ((clear_set + 1));
		}
	}
}

void delete_missile() {
	for (int i = 0; i < missile.size(); i++) {
		if (missile[i].rt.left > 800 || missile[i].mcnt > 40) {
			missile.erase(missile.begin() + i);
			return;
		}
	}
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	srand((unsigned)time(NULL));
	HWND hwnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	hwnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}
CImage start_screen;
CImage title;
CImage cartoon[6];
CImage cartoon_bg;
CImage main_screen;
CImage start_button;
CImage item_check;
CImage click_cursor;
CImage game_bg;
CImage kirby_fly;
CImage heart;
CImage basic_bullet;
CImage game_bg2;
CImage skill;
CImage skill_color;
CImage skill_bullet;
CImage m[5];
CImage monster_bullet;
CImage number;
CImage hp_bar, hp_empty;
CImage skill_effects;
CImage game_over;
CImage over_number;
CImage kirby_hit;
CImage kirby_zombie;
CImage warning;
CImage missile_explosion;
CImage missile_bmp;
CImage coin_bmp;
CImage monster_create;
CImage monster_die;
CImage hit_effect;
CImage rezero_bg;
CImage kirby_die;
System* ssystem;
Sound* title_bgm;
Sound* main_bgm;
Sound* cartoon_bgm;
Sound* battle_bgm;
Sound* click_sound;
Sound* damage_sound;
Sound* gameover_sound;
Sound* shop_sound;
Sound* skill_sound;
Sound* cant_shop_sound;
Sound* shop_cancle_sound;
Sound* explosion_sound;
Sound* warning_sound;
Sound* coin_sound;
Channel* channel = 0;
FMOD_RESULT result;
void* extradriverdata = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC mdc;
	HBITMAP hBitmap;
	static RECT rt; // 전체 윈도우 크기

	static int screen; // 화면 변경
	static int ani = 0;
	static int title_x, title_y;
	static BOOL title_direction;
	static int count;
	static TCHAR ID[10] = { '\0' }; // ID 입력 받는 변수
	static TCHAR number_text[99] = { '\0' };//점수, 골드 출력용
	static int num;
	static TCHAR hp_paint[10]; //hp출력용변수 (임시)
	static int cartoon_cnt = 0;

	static int stbt_x, stbt_y; // start button
	static BOOL stbt_direction;
	static RECT stbt_rt = { 460,440,760,560 };
	static int mx, my; // 마우스 좌표 입력 받음
	static POINT mypt;
	static Item item[4] // item
		= { {{666,177,700,211},FALSE,0,500},
			{{666,225,700,259},FALSE,0,1000},
			{{666,270,700,304},FALSE,0,500},
			{{666,319,700,353},FALSE,0,1000} };
	static int click_xPos = 0; // 사진 xPos

	static int bg_xPos = 800; // 배경 사진 xPos
	static POINT heart_pt[4] = { {765,25},{730,25},{695,25},{660,25} }; // 하트 찍는 위치

	static POINT bullet_pt; // 총알 POINT
	static int basic_bullet_xPos = 0; // 총알 사진 xPos
	static int skill_bullet_xPos = 0; // 총알 사진 xPos

	static int skill_dy = 0; // skill 버튼 차오르는 변수
	static BOOL skill_ef; // skill 이펙트 창 뜨게 하기
	static int skill_ef_cnt = 0;
	int full_hp[5] = { 0, (50 + (clear_set % 4 * 10) + (clear_stage * 10)) ,(50 + (clear_set % 4 * 10) + (clear_stage * 10)) ,(50 + (clear_set % 4 * 10) + (clear_stage * 10)) ,(100 + (clear_stage * 20)) };

	static BOOL game_over_screen;
	static RECT game_over_rt = { 486,392,677,479 };

	static HPEN hp;
	static HPEN ohp;

	switch (iMsg) {
	case WM_CREATE:
		GetClientRect(hwnd, &rt); // 윈도우 크기 저장
		// 타이머
		SetTimer(hwnd, 0, 50, NULL);
		SetTimer(hwnd, 1, 10, NULL);
		// 이미지 로드
		{
			title.LoadFromResource(g_hInst, MAKEINTRESOURCE(TITLE));
			start_screen.LoadFromResource(g_hInst, MAKEINTRESOURCE(LOGIN_SCREEN));
			cartoon[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_1));
			cartoon[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_2));
			cartoon[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_3));
			cartoon[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_4));
			cartoon[4].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_5));
			cartoon[5].LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_6));
			cartoon_bg.LoadFromResource(g_hInst, MAKEINTRESOURCE(CARTOON_BG));
			main_screen.LoadFromResource(g_hInst, MAKEINTRESOURCE(MAIN_SCREEN));
			start_button.LoadFromResource(g_hInst, MAKEINTRESOURCE(START_BUTTON));
			item_check.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM_CHECK));
			click_cursor.LoadFromResource(g_hInst, MAKEINTRESOURCE(CLiCK_CURSOR));
			game_bg.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_BACKGROUND));
			kirby_fly.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_FLY_LEFT));
			heart.LoadFromResource(g_hInst, MAKEINTRESOURCE(HEART));
			basic_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(BASIC_BULLET));
			game_bg2.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_BACKGROUND_2));
			item[0].img.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM1));
			item[1].img.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM2));
			item[2].img.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM3));
			item[3].img.LoadFromResource(g_hInst, MAKEINTRESOURCE(ITEM4));
			skill.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL));
			skill_color.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_COLOR));
			skill_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_BULLET));
			m[0].LoadFromResource(g_hInst, MAKEINTRESOURCE(SMALL_MONSTER));
			m[1].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID1_MONSTER));
			m[2].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID2_MONSTER));
			m[3].LoadFromResource(g_hInst, MAKEINTRESOURCE(MID3_MONSTER));
			m[4].LoadFromResource(g_hInst, MAKEINTRESOURCE(BIG_MONSTER));
			monster_bullet.LoadFromResource(g_hInst, MAKEINTRESOURCE(ENEMY_BULLET));
			number.LoadFromResource(g_hInst, MAKEINTRESOURCE(NUMBER));
			hp_bar.LoadFromResource(g_hInst, MAKEINTRESOURCE(HP_BAR));
			hp_empty.LoadFromResource(g_hInst, MAKEINTRESOURCE(HP_EMPTY));
			skill_effects.LoadFromResource(g_hInst, MAKEINTRESOURCE(SKILL_EFFECTS));
			game_over.LoadFromResource(g_hInst, MAKEINTRESOURCE(GAME_OVER));
			over_number.LoadFromResource(g_hInst, MAKEINTRESOURCE(OVER_NUMBER));
			kirby_hit.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_HIT));
			kirby_zombie.LoadFromResource(g_hInst, MAKEINTRESOURCE(KIRBY_ZOMBIE));
			warning.LoadFromResource(g_hInst, MAKEINTRESOURCE(WARNING));
			missile_explosion.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE_EXPLOSION));
			missile_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(MISSILE));
			coin_bmp.LoadFromResource(g_hInst, MAKEINTRESOURCE(COIN));
			monster_create.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_CREATE));
			hit_effect.LoadFromResource(g_hInst, MAKEINTRESOURCE(HIT_EFFECT));
			monster_die.LoadFromResource(g_hInst, MAKEINTRESOURCE(MONSTER_DIE));
			
			
		}

		// bgm 로드
		{
			result = System_Create(&ssystem);
			if (result != FMOD_OK)
				exit(0);
			ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
			ssystem->createSound("title_bgm.OGG", FMOD_LOOP_NORMAL, 0, &title_bgm);
			ssystem->createSound("cartoon_bgm.OGG", FMOD_LOOP_NORMAL, 0, &cartoon_bgm);
			ssystem->createSound("click.OGG", FMOD_DEFAULT, 0, &click_sound);
			ssystem->createSound("damage.OGG", FMOD_DEFAULT, 0, &damage_sound);
			ssystem->createSound("gameover.OGG", FMOD_DEFAULT, 0, &gameover_sound);
			ssystem->createSound("Shop.OGG", FMOD_DEFAULT, 0, &shop_sound);
			ssystem->createSound("Skill.OGG", FMOD_DEFAULT, 0, &skill_sound);
			ssystem->createSound("buzzer.OGG", FMOD_DEFAULT, 0, &cant_shop_sound);
			ssystem->createSound("cancle.OGG", FMOD_DEFAULT, 0, &shop_cancle_sound);
			ssystem->createSound("Explosion.OGG", FMOD_DEFAULT, 0, &explosion_sound);
			ssystem->createSound("warning.OGG", FMOD_DEFAULT, 0, &warning_sound);
			ssystem->createSound("coin.OGG", FMOD_DEFAULT, 0, &coin_sound);
		}
		ssystem->playSound(title_bgm, 0, false, &channel);

		title_x = 0; title_y = 0; title_direction = TRUE;
		count = 0; screen = 0;
		stbt_x = 0; stbt_y = 0; stbt_direction = TRUE;
		kirby.x = 675; kirby.y = 300; // 커비 시작 위치
		kirby.rt = { kirby.x - 25,kirby.y - 25,kirby.x + 25,kirby.y + 25 }; kirby.xPos = 0;
		kirby.skill = FALSE; kirby.skill_cnt = 0;
		skill_ef = FALSE;
		game_over_screen = FALSE;
		create_enemy();
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
		mdc = CreateCompatibleDC(hdc);
		SelectObject(mdc, hBitmap);
		Rectangle(mdc, 0, 0, rt.right, rt.bottom);
		//-------------------------------------------------------------------------------------------
		switch (screen) {
		case 0: // login screen
			start_screen.StretchBlt(mdc, 0, 0, rt.right, rt.bottom, 0, 0, 800, 600, SRCCOPY);
			title.TransparentBlt(mdc, 10 - title_x, 10 - title_y, title_x + 800, title_y + 600, 0, 0, 800, 600, RGB(255, 0, 255));
			SetBkMode(mdc, TRANSPARENT);
			TextOut(mdc, 280, 430, ID, lstrlen(ID));
			break;
		case 1: // cartoon
			cartoon_bg.StretchBlt(mdc, 0, 0, rt.right, rt.bottom, 0, 0, 800, 600, SRCCOPY);
			for (int i = 0; i < cartoon_cnt + 1; ++i)
				cartoon[i].TransparentBlt(mdc, 0, 0, rt.right, rt.bottom, 0, 0, 800, 600, RGB(255, 0, 255));
			click_cursor.TransparentBlt(mdc, rt.right - 100, rt.bottom - 100, 90, 90, click_xPos, 0, 52, 52, RGB(255, 0, 255));
			break;
		case 2: // main screen
			main_screen.StretchBlt(mdc, 0, 0, rt.right, rt.bottom, 0, 0, 800, 600, SRCCOPY);
			start_button.TransparentBlt(mdc, 10 - stbt_x, 10 - stbt_y, rt.right, rt.bottom, 0, 0, 800, 600, RGB(255, 0, 255));
			for (int i = 0; i < 4; ++i)
				if (item[i].buy) item_check.TransparentBlt(mdc, item[i].rt.left, item[i].rt.top - 10, item[i].rt.right - item[i].rt.left, item[i].rt.bottom - item[i].rt.top, 0, 0, 24, 24, RGB(255, 0, 255));
			//내정보
			SetBkMode(mdc, TRANSPARENT);
			TextOut(mdc, 155, 75,My.name, lstrlen(My.name));
			wsprintf(number_text, L"%d", My.score);
			SetBkMode(mdc, TRANSPARENT);
			TextOut(mdc, 270, 100, number_text, lstrlen(number_text));
			wsprintf(number_text, L"%d G", My.money);
			SetBkMode(mdc, TRANSPARENT);
			TextOut(mdc, 555, 110, number_text, lstrlen(number_text));
			//랭킹
			for (int i = 0; (i < 5)&&(i<all_data.size());  i++) {
				if (i < 3) {
					SetBkMode(mdc, TRANSPARENT);
					TextOut(mdc, 158, (60 * i) + 210, all_data[i].name, lstrlen(all_data[i].name));
					wsprintf(number_text, L"%d", all_data[i].score);
					SetBkMode(mdc, TRANSPARENT);
					TextOut(mdc, 158, (60 * i) + 230, number_text, lstrlen(number_text));
				}
				else {
					SetBkMode(mdc, TRANSPARENT);
					TextOut(mdc, 158, (60 * i) + 200, all_data[i].name, lstrlen(all_data[i].name));
					wsprintf(number_text, L"%d", all_data[i].score);
					SetBkMode(mdc, TRANSPARENT);
					TextOut(mdc, 158, (60 * i) + 220, number_text, lstrlen(number_text));
				}
			}
			break;
		case 3: // game screen
			game_bg.StretchBlt(mdc, 0, 50, 800, rt.bottom - 50, bg_xPos, 0, 800, 500, SRCCOPY);
			if (bg_xPos >= 800)game_bg.StretchBlt(mdc, 1600 - bg_xPos, 50, bg_xPos - 800, rt.bottom - 50, 0, 0, bg_xPos - 800, 500, SRCCOPY);
			game_bg2.StretchBlt(mdc, 0, 0, 800, 50, 0, 0, 800, 50, SRCCOPY);
			// 아이템
			for (int i = 0; i < 4; ++i)
				if (item[i].buy)item[i].img.TransparentBlt(mdc, 0, 0, 800, 50, 0, 0, 800, 50, RGB(255, 0, 255));
			// 하트
			for (int i = 0; i < heart_cnt; ++i)
				heart.TransparentBlt(mdc, heart_pt[i].x - 15, heart_pt[i].y - 15, 30, 30, 0, 0, 18, 16, RGB(0, 255, 0));
			skill.StretchBlt(mdc, rt.right - 75, 470, 70, 85, 0, 0, 70, 85, SRCCOPY);
			skill_color.StretchBlt(mdc, rt.right - 75, 555 - skill_dy, 70, skill_dy, 0, 85 - skill_dy, 70, skill_dy, SRCCOPY);
			//점수
			wsprintf(number_text, L"%d", play_score);
			if (lstrlen(number_text) == 0)number.TransparentBlt(mdc, 10, 12, 24, 28, 0, 0, 12, 14, RGB(255, 0, 255));
			for (int i = 0; i < lstrlen(number_text); i++) {
				num = number_text[i] - 48;
				number.TransparentBlt(mdc, i * 24 + 10, 12, 24, 28, num * 12, 0, 12, 14, RGB(255, 0, 255));
			}
			// 코인 수
			wsprintf(number_text, L"%d", play_gold);
			if (lstrlen(number_text) == 0)number.TransparentBlt(mdc, 10, 12, 24, 28, 0, 0, 12, 14, RGB(255, 0, 255));
			for (int i = 0; i < lstrlen(number_text); i++) {
				num = number_text[i] - 48;
				number.TransparentBlt(mdc, i * 24 + 520, 12, 24, 28, num * 12, 0, 12, 14, RGB(255, 0, 255));
			}
			// 커비 총알
			for (int i = 0; i < st_bullet.size(); ++i)
				skill_bullet.TransparentBlt(mdc, st_bullet[i].x - 20, st_bullet[i].y - 15, 40, 30, skill_bullet_xPos, 0, 260, 260, RGB(255, 0, 255));
			// 커비 총알
			for (int i = 0; i < bullet.size(); ++i)
				basic_bullet.TransparentBlt(mdc, bullet[i].x - 20, bullet[i].y - 10, 40, 20, basic_bullet_xPos, 0, 270, 160, RGB(255, 0, 255));
			// 커비
			if (zombie_cnt == 0) {
				kirby.rt = { kirby.x - 25,kirby.y - 25,kirby.x + 25,kirby.y + 25 };
				kirby_fly.TransparentBlt(mdc, kirby.rt.left, kirby.rt.top, 50, 50, kirby.xPos, 0, 25, 25, RGB(0, 255, 0));
			}
			else if (zombie_cnt < 8) {
				kirby.rt = { kirby.x - 25,kirby.y - 25,kirby.x + 25,kirby.y + 25 };
				kirby_hit.TransparentBlt(mdc, kirby.rt.left, kirby.rt.top, 50, 50, (zombie_cnt % 7) * 25, 0, 25, 25, RGB(0, 255, 0));
			}
			else {
				kirby.rt = { kirby.x - 25,kirby.y - 25,kirby.x + 25,kirby.y + 25 };
				kirby_zombie.TransparentBlt(mdc, kirby.rt.left, kirby.rt.top, 50, 50, (zombie_cnt % 9) * 25, 0, 25, 25, RGB(0, 255, 0));
			}
			//코인 
			for (const POINT c : coins) coin_bmp.TransparentBlt(mdc, c.x, c.y, 52, 40, (ani % 8) * 230, 0, 230, 130, RGB(0, 255, 0));
			//적 
			if (e.size() > 0) {
				for (const enemy paint_enemy : e) {
					if (paint_enemy.cnt < 0) {
						monster_create.TransparentBlt(mdc, paint_enemy.rt.left,paint_enemy.rt.top, (paint_enemy.rt.right-paint_enemy.rt.left),(paint_enemy.rt.bottom-paint_enemy.rt.top),33*(6+(paint_enemy.cnt/3)),0,33,33,RGB(255,0,255));
					}
					else {
						m[paint_enemy.type].TransparentBlt(mdc, paint_enemy.rt.left, paint_enemy.rt.top, (paint_enemy.rt.right - paint_enemy.rt.left), (paint_enemy.rt.bottom - paint_enemy.rt.top), ((paint_enemy.rt.right - paint_enemy.rt.left) / 2) * (ani % (paint_enemy.frame)), 0, (paint_enemy.rt.right - paint_enemy.rt.left) / 2, (paint_enemy.rt.bottom - paint_enemy.rt.top) / 2, RGB(255, 0, 255));
						if (paint_enemy.type > 0) {
							num = paint_enemy.rt.right - paint_enemy.rt.left;
							hp_empty.TransparentBlt(mdc, paint_enemy.rt.left - 30, paint_enemy.rt.top - 22, 224 * ((double)num / 160), 22, 0, 0, 224, 22, RGB(0, 255, 0));
							hp_bar.TransparentBlt(mdc, paint_enemy.rt.left - 30, paint_enemy.rt.top - 22, abs((((double)paint_enemy.hp / (double)full_hp[paint_enemy.type]) * 224) * ((double)num / 160) + 1), 22, 0, 0, ((double)paint_enemy.hp / (double)full_hp[paint_enemy.type]) * 224, 22, RGB(0, 255, 0));
						}
					}
				}
			}
			//이펙트
			for (const Effect& e : effect) {
				switch (e.type)
				{case 0:
					hit_effect.TransparentBlt(mdc, e.rt.left, e.rt.top, (e.rt.right - e.rt.left), (e.rt.bottom - e.rt.top), 30 * e.cnt, 0, 30, 30, RGB(255,0,255));
					break;
				case 1:
					monster_die.TransparentBlt(mdc, e.rt.left, e.rt.top, (e.rt.right - e.rt.left), (e.rt.bottom - e.rt.top), 44 * e.cnt, 0, 44, 44, RGB(255, 0, 255));
					break;
				default:
					break;
				}
			}
			//적탄막
			for (const enemy_bullet eb : eb)monster_bullet.TransparentBlt(mdc, eb.xy.x, eb.xy.y, 10, 10, 0, 0, 30, 30, RGB(255, 0, 255));
			//거대탄막 
			for (const Missile m : missile) {
				if (m.mcnt < 20) {
					hp = CreatePen(PS_SOLID, 4, RGB(255, 0, 0)); // 새로운 객체 만들기
					ohp = (HPEN)SelectObject(mdc, hp);
					MoveToEx(mdc, 0, m.rt.top + 11, NULL);
					LineTo(mdc, rt.right, m.rt.top + 11);
					DeleteObject(hp);
					warning.TransparentBlt(mdc, 300, m.rt.top - 13, 70, 70, 71 * (m.mcnt % 7), 0, 71, 71, RGB(0, 255, 0));
				}
				else if (m.mcnt >= 30 && m.mcnt < 40) {
					missile_explosion.TransparentBlt(mdc, m.rt.left + 40, m.rt.top, 64, 64, 32 * (m.mcnt % 14), 0, 32, 32, RGB(255, 0, 255));
				}
				else if (m.mcnt >= 20 && m.mcnt < 30) {
					missile_bmp.TransparentBlt(mdc, m.rt.left, m.rt.top, 80, 44, 40 * (ani % 5), 0, 40, 22, RGB(255, 0, 255));
				}
			}
			// 스킬 이펙트 ( 제일 위에 그려야 함)
			if (skill_ef) skill_effects.StretchBlt(mdc, 0, 225, rt.right, 143, 0, 0, 800, 143, SRCCOPY);
			//  game over
			if (game_over_screen) {
				game_over.TransparentBlt(mdc, 0, 0, rt.right, rt.bottom, 0, 0, 800, 600, RGB(255, 0, 255));
				wsprintf(number_text, L"%d", play_score);
				if (lstrlen(number_text) == 0)number.TransparentBlt(mdc, 470, 180, 95, 135, 0, 0, 100, 135, RGB(255, 0, 255));
				for (int i = lstrlen(number_text) - 1; i >= 0; --i) {
					num = number_text[i] - 48;
					over_number.TransparentBlt(mdc, ((lstrlen(number_text) - i - 1) * -100) + 470, 180, 95, 135, num * 100, 0, 100, 135, RGB(255, 0, 255));
				}
				wsprintf(number_text, L"%d", play_gold);
				if (lstrlen(number_text) == 0)number.TransparentBlt(mdc, 315, 360, 50, 80, 0, 0, 100, 135, RGB(255, 0, 255));
				for (int i = 0; i < lstrlen(number_text); i++) {
					num = number_text[i] - 48;
					over_number.TransparentBlt(mdc, ((lstrlen(number_text) - i - 1) * -55) + 315, 360, 50, 80, num * 100, 0, 100, 135, RGB(255, 0, 255));
				}
			}
			break;
		}
		//-------------------------------------------------------------------------------------------
		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteDC(mdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_CHAR:
		switch (screen) {
		case 0: // login screen
			if (wParam == VK_BACK) {
				if (count == 0);
				else {
					ID[count - 1] = NULL;
					count--;
				}
			}
			else if (wParam == VK_RETURN) {
				channel->stop();
				title_bgm->release();
				ssystem->playSound(click_sound, 0, false, &channel);
				ssystem->playSound(cartoon_bgm, 0, false, &channel);
				My.money = 0;
				My.score = 0;
				_tcscpy(My.name,ID);
				load(ID);
				save();
				(not_first == 0 )?screen = 1:screen =2;
			}
			else if (count < 10) ID[count++] = wParam;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (screen) {
		case 3:
			switch (wParam) {
			case VK_RETURN: // 스킬 사용
				if (skill_dy == 85) {
					ssystem->playSound(skill_sound, 0, false, &channel);
					skill_dy = 0; kirby.skill = TRUE; kirby.skill_cnt = 0;
					skill_ef = TRUE; skill_ef_cnt = 0;
				}
				break;
			case VK_SPACE: // 잠시 멈춤

				break;
			}
			break;
		}
		break;
	case WM_TIMER:
		switch (wParam) {
		case 0:
			switch (screen) {
			case 0: // login screen
				if (title_direction) {
					title_x += 4; title_y += 3;
					if (title_x == 20)title_direction = FALSE;
				}
				else {
					title_x -= 4; title_y -= 3;
					if (title_x == 0)title_direction = TRUE;
				}
				break;
			case 1: // story screen
				click_xPos += 52;
				if (click_xPos == 104)click_xPos = 0;
				break;
			case 2: // main screen
				if (stbt_direction) {
					stbt_x += 2; stbt_y += 3;
					if (stbt_x == 8)stbt_direction = FALSE;
				}
				else {
					stbt_x -= 2; stbt_y -= 3;
					if (stbt_x == 0)stbt_direction = TRUE;
				}
				break;
			case 3: // game screen
				if (game_over_screen == FALSE) {
					if (heart_cnt <= 0) {
						if (item[3].buy) {
							heart_cnt = 2;
							item[3].buy = FALSE;
						}
						else {
							channel->stop();
							battle_bgm->release();
							ssystem->createSound("gameover.OGG", 0, 0, &gameover_sound);
							ssystem->playSound(gameover_sound, 0, false, &channel);
							game_over_screen = TRUE;
						}
					}
					if (skill_ef) { // 스킬 이펙트 사용
						if (skill_ef_cnt == 10)skill_ef = FALSE;
						skill_ef_cnt++;
					}
					else {
						create_enemy_bullet();
						move_enemy_bullet();
						coinmove();
						effect_cnt();
						if (missile.size() > 0)move_missile();
						if (zombie_cnt > 0) {
							if (zombie_cnt == 1 && hit_missile_sound == FALSE)ssystem->playSound(damage_sound, 0, false, &channel);
							zombie_cnt++;
							if (zombie_cnt > 30)zombie_cnt = 0;
						}
						create_missile_cnt++;
						if (create_missile_cnt > 200 - (clear_set / 2))create_missile_cnt = 0, create_missile();
						for (int i = 0; i < missile.size(); i++) {
							if (missile[i].mcnt == 0)ssystem->playSound(warning_sound, 0, false, &channel);
						}
						if (hit_missile_sound) {
							ssystem->playSound(explosion_sound, 0, false, &channel);
							hit_missile_sound = FALSE;
						}
						if (get_coin_sound) {
							ssystem->playSound(coin_sound, 0, false, &channel);
							get_coin_sound = FALSE;
						}
						if (e.size() > 0)monster_move();
						for (int i = 0; i < bullet.size(); ++i)bullet[i].x -= 20;
						for (int i = 0; i < st_bullet.size(); ++i)st_bullet[i].x -= 20;
						if (kirby.skill) {
							if (kirby.skill_cnt == 40) kirby.skill = FALSE;
							else {
								bullet_pt = { kirby.x,kirby.y };
								st_bullet.push_back(bullet_pt);
								kirby.skill_cnt++;
							}
						}
						else {
							if (skill_dy == 85);
							else skill_dy++;
							if (e.size() == 0) {
								clear_set++;
								if (clear_set % 4 == 0 && clear_set != 0) clear_stage++;
								create_enemy();
							}
							bullet_pt = { kirby.x,kirby.y };
							bullet.push_back(bullet_pt);
						}
					}
				}
				ani++;
				kirby.xPos += 25;
				if (kirby.xPos == 200)kirby.xPos = 0;
				basic_bullet_xPos += 270;
				if (basic_bullet_xPos == 1350)basic_bullet_xPos = 0;
				skill_bullet_xPos += 260;
				if (skill_bullet_xPos == 1300)skill_bullet_xPos = 0;
				break;
			}
			break;
		case 1: // game background timer
			switch (screen) {
			case 3:
				if (game_over_screen == FALSE) {
					hit_me();
					hit_missile();
					hit_monster();
					die_monster();
					bullet_delete();
					enemy_bullet_delete();
					delete_missile();
					getcoin();
					coindelete();
					effect_delete();
					if (bg_xPos == 0) bg_xPos = 1600;
					else bg_xPos--;
				}
				break;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	case WM_LBUTTONDOWN:
		switch (screen) {
		case 1: // cartoon
			ssystem->playSound(click_sound, 0, false, &channel);
			cartoon_cnt++;
			if (cartoon_cnt == 6) {
				channel->stop();
				cartoon_bgm->release();
				ssystem->createSound("main_bgm.OGG", FMOD_LOOP_NORMAL, 0, &main_bgm);
				ssystem->playSound(main_bgm, 0, false, &channel);
				screen = 2;
			}
			break;
		case 2: // main screen
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			mypt = { mx,my };
			if (PtInRect(&stbt_rt, mypt)) { // start
				channel->stop();
				main_bgm->release();
				ssystem->playSound(click_sound, 0, false, &channel);
				ssystem->createSound("Battle_bgm.OGG", FMOD_LOOP_NORMAL, 0, &battle_bgm);
				ssystem->playSound(battle_bgm, 0, false, &channel);
				screen = 3;
				play_score = 0;
				play_gold = 0;
				clear_set = 0; clear_stage = 0;
				e.clear();
				eb.clear();
				create_missile_cnt = 0;
				missile.clear();
				bullet.clear();
				st_bullet.clear();
				zombie_cnt = 0;
				if (item[0].buy)heart_cnt = 4;
				else heart_cnt = 3;
				if (item[2].buy)skill_dy = 85;
				else skill_dy = 0;
				game_over_screen = FALSE;
				
			}
			for (int i = 0; i < 4; ++i) {
				if (PtInRect(&item[i].rt, mypt)) {
					if (item[i].buy_cnt % 2 == 0) {
						if (item[i].price <= My.money) {
							ssystem->playSound(shop_sound, 0, false, &channel);
							item[i].buy = TRUE;
							My.money -= item[i].price;
							save();
							item[i].buy_cnt++;
						     }
						else { // 여기에 버저효과음 깔아주셈요
						}
					
					}
					else {
						item[i].buy = FALSE;
						My.money += item[i].price;
						ssystem->playSound(shop_cancle_sound, 0, false, &channel);
						item[i].buy_cnt++;
					}
					
				}
			}
			break;
		case 3:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			mypt = { mx,my };
			if (game_over_screen) {
				if (PtInRect(&game_over_rt, mypt)) {
					channel->stop();
					gameover_sound->release();
					ssystem->createSound("main_bgm.OGG", FMOD_LOOP_NORMAL, 0, &main_bgm);
					ssystem->playSound(main_bgm, 0, false, &channel);
					My.money += play_gold;
					if (My.score < play_score) My.score = play_score;
					save();
					sort(all_data.begin(), all_data.end(), [](Data a, Data b)->bool {return a.score > b.score; });
					screen = 2;
				}
			}
			break;
		}
		break;
	case WM_MOUSEMOVE:
		switch (screen) {
		case 3:
			my = HIWORD(lParam);
			if (my <= 75 || my >= rt.bottom - 25);
			else kirby.y = my;
			break;
		}
		break;
	case WM_DESTROY:
		title.Destroy();
		start_screen.Destroy();
		cartoon[0].Destroy();
		cartoon[1].Destroy();
		cartoon[2].Destroy();
		cartoon[3].Destroy();
		cartoon[4].Destroy();
		cartoon[5].Destroy();
		cartoon_bg.Destroy();
		main_screen.Destroy();
		start_button.Destroy();
		item_check.Destroy();
		click_cursor.Destroy();
		game_bg.Destroy();
		kirby_fly.Destroy();
		heart.Destroy();
		basic_bullet.Destroy();
		game_bg2.Destroy();
		item[0].img.Destroy();
		item[1].img.Destroy();
		item[2].img.Destroy();
		item[3].img.Destroy();
		skill.Destroy();
		skill_color.Destroy();
		skill_bullet.Destroy();
		m[0].Destroy();
		m[1].Destroy();
		m[2].Destroy();
		m[3].Destroy();
		m[4].Destroy();
		monster_bullet.Destroy();
		number.Destroy();
		hp_bar.Destroy();
		hp_empty.Destroy();
		KillTimer(hwnd, 0);
		KillTimer(hwnd, 1);
		ssystem->close();
		ssystem->release();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}