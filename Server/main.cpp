#include "stdafx.h"
#include "protocol.h"
#include "Player.h"

// 전방선언 라인
enum TASK_TYPE { FIRE_PLAYER_BULLET, MOVE_PLAYER_BULLET, FIRE_ENEMY_BULLET, 
    AI_MOVE, SKILL_END , BULLET_DELETE, CREATE_SET, ENABLE_COLLISION,
    CANCEL_MAGNET , CANCEL_DUAL }; // 뭐가 있을까?
void push_evt_queue(TASK_TYPE ev, int time, std::string& rid);
void addSkillCount(OTYPE type, std::string& id);


class Enemy_Bullet
{
private:
    unsigned short x;
    unsigned short y;

    POINT dir;
public:
    Enemy_Bullet(unsigned short x, unsigned short y, POINT dir) : x{ x }, y{ y }, dir{ dir } {}
    Enemy_Bullet(int x, int y, POINT dir) : x{ static_cast<unsigned short>(x) }, y{ static_cast<unsigned short>(y) }, dir{ dir } {}
    std::pair<unsigned short, unsigned short> getPosition() { return { x, y }; }
    void updatePosition()
    {
        x += -MOVE_DIST * dir.x;
        y += -MOVE_DIST * dir.y;
    }
    unsigned short GetX() { return x; }
    unsigned short GetY() { return y; }
    POINT GetDir() { return dir; }
    void SetDir(POINT new_dir) { dir = new_dir; }
   
};

class Player_Bullet // 무조건 왼쪽으로 이동하기만 함.
{
private:
    unsigned short x = DEFXPOS;
    unsigned short y;
    OTYPE type;

public:

    Player_Bullet(unsigned short y, OTYPE type) :y{ y }, type{ type } {}
    std::pair<unsigned short, unsigned short> getPosition() { return { x, y }; }
    OTYPE getType() { return type; }
    unsigned short damage()
    {
        if (type == P1_SKILLBULLET || type == P2_SKILLBULLET) return DAMAGE * 2;
        else return DAMAGE;
    }
    void updatePosition()
    {
        x += MOVE_DIST;
    }

};

class Enemy
{
    OTYPE type = ENEMY_0; //0 잡몹 1~3 중간보스 4 최종보스
    unsigned short x = 0;
    unsigned short y = 0;
    unsigned short hp = 0;

public:
    short attack_type;
    short bullet_create_cnt = -2;
    //유도되는 정보
    unsigned short width = 0;
    unsigned short height = 0;
private:
    POINT goal = {0,0};
public:
    Enemy(OTYPE a_type, int a_x, int a_y, int a_hp)
    {
        type = a_type;
        x = static_cast<unsigned short>(a_x);
        y = static_cast<unsigned short>(a_y);
        hp = static_cast<unsigned short>(a_hp);

        if (type == 0) width = 84, height = 96;
        else if (type == 1 || type == 2) width = 90, height = 130;
        else if (type == 3) width = 100, height = 120;
        else if (type == 4) width = 256, height = 224;
        goal = { rand() % 200 + 30, rand() % 400 + 50 };

        attack_type = rand() % 4;

    }
    void ai_move()
    {
        // todo: 여기 ai 로직 넣어줘
     
        if (type == ENEMY_0) {

            y += rand() % 40 - 20;
            if (y < 146)y =146;
            if (y > 504)y = 504;

        }
        else {
            x += ((goal.x -x) / sqrt(pow(goal.x - x, 2) + pow(goal.y - y, 2)))*5;
            y += ((goal.y -y) / sqrt(pow(goal.x -y, 2) + pow(goal.y -y, 2)))*5;
            if ((x <= goal.x + 10 && x >= goal.x - 10 &&y <= goal.y + 10 && y >= goal.y - 10)) goal = { rand() % 200 + 30, rand() % 400 + 50 };
        }
        
    }

  
    bool collsion_player_bullet(Player_Bullet& bullet, std::string& id) {
        //적과 총알 충돌 검사
       
        RECT rect = { x, y, x + width, y + height };
        POINT bullet_pos = { bullet.getPosition().first,bullet.getPosition().second };
        if (PtInRect(&rect, bullet_pos)) { // bullet이 사각형 내부에 있을 때 실행할 코드
            if (hp - bullet.damage() < 0) hp = 0; 
            else hp -= bullet.damage();

            addSkillCount(bullet.getType(), id);

            return true; //인수로 들어온 총알과 충돌시 true리턴
        }
        else return false; //인수로 들어온 총알과 충돌하지 않았을시 false리턴
    }
    //// getter - setter

    std::pair<unsigned short, unsigned short> getPosition() { return { x, y }; }
    OTYPE getType() { return type; }
    short getHP() { return hp; }
    void setHP(unsigned short new_hp) { hp = new_hp; };
    bool isDie() { return (hp <= 0); }
};

class Item {
private:
    OTYPE type; // 0: 코인 1:듀얼 2:자석
    unsigned short x;
    unsigned short y;
public:
    Item(OTYPE type, unsigned short x, unsigned short y) : type{type}, x { x }, y{ y } {}
    Item(OTYPE type, int x, int y) : x{ static_cast<unsigned short>(x) }, y{ static_cast<unsigned short>(y) }, type{type} {}
    std::pair<unsigned short, unsigned short> getPosition() { return { x, y }; }
    void updatePosition(bool m,unsigned py )
    {
        //자석이 켜져있는지 아닌지
        if (m) {
            x += -MOVE_DIST;
            if (py > y) y += 10;
            else if (py < y) y -= 10;
        }
        else x += -MOVE_DIST ;
    }
    unsigned short GetX() { return x; }
    unsigned short GetY() { return y; }
    OTYPE getType() { return type; }
};

class Room
{
private:
    Player* p1 = nullptr;
    Player* p2 = nullptr;
    std::string dealer_id; // 누가 딜러인가.
    std::atomic_bool isPlaying = false;
    unsigned int score = 0;
    unsigned short heart = 3; // 음수가 없..?
    unsigned int get_coin = 0; //게임에서 획득한 코인의 양

    std::mutex update_lock; // todo: 락을 분리할지 고민하기

    std::vector<Player_Bullet> p_bullets;
    std::vector<Enemy> enemies;
    std::vector<Enemy_Bullet> e_bullets;
    std::vector<Item> drop_Items;
    
public:
    int clear_set = 0; //몇세트 클리어?
    int clear_stage = 0; //1스테이지에 4세트, 이걸로 난이도 점점 오르게

public:

    Room() {};
    ~Room() {};

    // ai/충돌처리 관련
    void doAIMove()
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        for (auto& e : enemies)
            e.ai_move();
    }

    void doBulletsMove()
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        for (auto& b : p_bullets)
            b.updatePosition();
        for (auto& b : e_bullets)
            b.updatePosition();
        if (p1->isMagnet()) {
            for (auto& i : drop_Items) {
                i.updatePosition(true, p1->getY());
            }
        }
        else if(p2->isMagnet()) {
            for (auto& i : drop_Items) {
                i.updatePosition(true, p2->getY());
            }
        }
        else {
            for (auto& i : drop_Items) {
                i.updatePosition(false, 0);
            }
        }
    }

    void createPbullet(std::string& s) // 두 플레이어한테서 동시에 생성되게 해도 괜찮을까?
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        if (s == p1->getID()) { // p1만 생성하기
            if (p1->getSkill() && getisDealer(p1->getID())) {
                if(p1->isDual())p_bullets.push_back({ static_cast<unsigned short>(p1->getY()+5 ), P1_SKILLBULLET }), p_bullets.push_back({ static_cast<unsigned short>(p1->getY() - 5), P1_SKILLBULLET });
                else p_bullets.push_back({ p1->getY(), P1_SKILLBULLET }); }
            else {
                
                if (p1->isDual())p_bullets.push_back({ static_cast<unsigned short>(p1->getY() + 5), P1_BULLET }), p_bullets.push_back({ static_cast<unsigned short>(p1->getY() - 5), P1_BULLET });
                else p_bullets.push_back({ p1->getY(), P1_BULLET });
            }
        }
        else { // p2만 생성하기
               if (p2->getSkill()&& getisDealer(p2->getID())) {
                if(p2->isDual())p_bullets.push_back({ static_cast<unsigned short>(p2->getY()+5), P2_SKILLBULLET }), p_bullets.push_back({ static_cast<unsigned short>(p2->getY() - 5), P2_SKILLBULLET });
                else p_bullets.push_back({ p2->getY(), P2_SKILLBULLET }); }
            else {
                
                if (p2->isDual())p_bullets.push_back({ static_cast<unsigned short>(p2->getY() + 5), P2_BULLET }), p_bullets.push_back({ static_cast<unsigned short>(p2->getY() - 5), P2_BULLET });
                else p_bullets.push_back({ p2->getY(), P2_BULLET });
            }
        }
    }

    void createEbullet(std::string& s) {
        std::lock_guard<std::mutex> ll{ update_lock };
        for (auto& e : enemies) {
            e.bullet_create_cnt++;
      
                switch (e.getType())
                {
                case ENEMY_0://작은몬스터 총알생성
                    e_bullets.push_back({ e.getPosition().first + 96, e.getPosition().second + 10, POINT{ 1, (rand() % 6 - 3)} });
                    e.bullet_create_cnt = 0;
                    break;
                case ENEMY_4://큰몬스터 총알생성
                    switch (e.attack_type)
                    {
                    case 0://원발사
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ 0, 3 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ 1, 2 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ 1,-2 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{  2, 1 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{  2,-1 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ -3,0 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ 3,0 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ -2, 1} });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ -2,-1 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ -1, 2 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{ -1,-2 } });
                        e_bullets.push_back({ e.getPosition().first + 122,e.getPosition().second + 148,{0,-3 } });
                        break;
                    case 1: //일직선발사
                        for (int i = 0; i < 5; i++) e_bullets.push_back({e.getPosition().first+ e.width - 112,e.getPosition().second + 44 * i,{ 3, 0}});
                        break;
                    case 2: //랜덤발사
                        for (int i = 0; i < 7; i++) {
                            e_bullets.push_back({e.getPosition().first + e.width - 122,e.getPosition().second + 118,{ (rand() % 6 - 3),(rand() % 6 - 3 )}});
                            while (e_bullets.back().GetDir().x == 0 && e_bullets.back().GetDir().y == 0)e_bullets.back().SetDir({ rand() % 6 - 3, rand() % 6 - 3 });
                        }
                        break;
                    case 3: //회오리발사
                        e_bullets.push_back({ e.getPosition().first + e.width - 122,e.getPosition().second + 118 ,{(short)((cos(((e.bullet_create_cnt - 10) * 15) * M_PI / 180))*3), (short)((sin((e.bullet_create_cnt - 10) * 15 * M_PI / 180))*3)}});
                        break;
                    default:
                        break;
                    }
                    if ((e.attack_type != 3 && e.bullet_create_cnt > 10 + 5) || (e.attack_type == 3 && e.bullet_create_cnt > 10 + 200))e.bullet_create_cnt = 0, e.attack_type = rand()%4;
                    break;
                default://중간몬스터 총알생성
                    switch (e.attack_type)
                    {
                    case 0://원발사
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{ 0, 4} });
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{ 2, 2} });
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{ 2,-2} });
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{-4,0 } });
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{ 4,0 } });
                        e_bullets.push_back({ e.getPosition().first + 45,e.getPosition().second + 65,{ -2, 2 } });
                        e_bullets.push_back({ e.getPosition().first + 45,e.getPosition().second + 65,{ -2,-2 } });
                        e_bullets.push_back( {e.getPosition().first + 45,e.getPosition().second + 65,{ 0,-4} });
                        break;
                    case 1: //일직선발사
                        for (int i = 0; i < 3; i++) e_bullets.push_back({ e.getPosition().first + e.width - 45,e.getPosition().second + 40 * i + 20 , { 3,0 }  });
                        break;
                    case 2: //랜덤발사
                        for (int i = 0; i < 5; i++) {
                            e_bullets.push_back({ e.getPosition().first + e.width - 122,e.getPosition().second + 118,{ (rand() % 6 - 3),(rand() % 6 - 3)} });
                            while (e_bullets.back().GetDir().x == 0 && e_bullets.back().GetDir().y == 0)e_bullets.back().SetDir({ rand() % 6 - 3, rand() % 6 - 3 });
                        }
                        break;
                    case 3: //회오리발사
                        e_bullets.push_back({ e.getPosition().first + e.width - 122,e.getPosition().second + 118 ,{(short)((cos(((e.bullet_create_cnt - 10) * 15) * M_PI / 180)) * 3), (short)((sin((e.bullet_create_cnt - 10) * 15 * M_PI / 180)) * 3)} });
                        break;
                    default:
                        break;
                    }
                    if ((e.attack_type != 3 && e.bullet_create_cnt > 10 + 5) || (e.attack_type == 3 && e.bullet_create_cnt > 10 + 200))e.bullet_create_cnt = 0, e.attack_type = rand()%4;
                    break;
                }

            
        }

    }

    void deletebullet(std::string& id)
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        // 1. 나간 총알,아이템 지우기
        std::erase_if(p_bullets, [](Player_Bullet& pb) { return (pb.getPosition().first < 10); });
        std::erase_if(e_bullets, [](Enemy_Bullet& eb) { return (eb.getPosition().first > 800 || eb.getPosition().first < 10|| eb.getPosition().second > 600 || eb.getPosition().second < 10); });
        std::erase_if(drop_Items, [](Item& eb) { return (eb.getPosition().first > 800 || eb.getPosition().first < 10|| eb.getPosition().second > 600 || eb.getPosition().second < 10); });

        // 2. 충돌체크 총알
        for (auto& e : enemies) {
            std::erase_if(p_bullets, [&e, &id](Player_Bullet& pb) { 
                return e.collsion_player_bullet(pb, id); });
        }

        // 3. players[id]와 적 총알 충돌체크
        std::erase_if(e_bullets, [this,&id](Enemy_Bullet& eb) {
            
            bool flag = false;

            Player* healer = nullptr;
            if (getisDealer(p1->getID())) healer = p2;
            else  healer = p1;

            // p1의 충돌체크
            RECT player_rt = { DEFXPOS - 25, p1->getY() - 25, DEFXPOS + 25, p1->getY() + 25 };
            if (PtInRect(&player_rt, { eb.GetX(),eb.GetY()}) == 1 && p1->zombieCount() < 1) {
                if ((healer) && (!(healer->getSkill()))) {//힐러의 보호막이 켜져있지않을때??
                    //커비 하트 깎기
                    heart = max(heart - 1, 0);

                    // 커비 잠시 무적모드 활성화 zombie_cnt를 1로
                    p1->setZombieCnt(1);
                    push_evt_queue(ENABLE_COLLISION, ZOMBIE_TIME, p1->getID());
                }
                flag = true;
            }
            
            // p2의 충돌체크
            player_rt = { DEFXPOS - 25, p2->getY() - 25, DEFXPOS + 25, p2->getY() + 25 };
            if (PtInRect(&player_rt, { eb.GetX(),eb.GetY() }) == 1 && p2->zombieCount() < 1) {
                if ((healer) && (!(healer->getSkill()))) {//힐러의 보호막이 켜져있지않을때??
                    //커비 하트 깎기
                    heart = max(heart - 1, 0);

                    // 커비 잠시 무적모드 활성화 zombie_cnt를 1로
                    p2->setZombieCnt(1);
                    push_evt_queue(ENABLE_COLLISION, ZOMBIE_TIME, p2->getID());
                }
                flag = true;
            }

            return flag;
        });



        //아이템과의 충돌체크
        std::erase_if(drop_Items, [this, &id](Item& eb) {

            bool flag = false;


            // p1의 충돌체크
            RECT player_rt = { DEFXPOS - 25, p1->getY() - 25, DEFXPOS + 25, p1->getY() + 25 };
            RECT item_rt = {eb.GetX(),eb.GetY(),eb.GetX()+50,eb.GetY() + 50 };
            RECT intersection;
            if (IntersectRect(&intersection, &player_rt, &item_rt) && p1->zombieCount() < 1) {

                if (eb.getType() == ITEM_COIN) get_coin += 10;
                if(eb.getType() == ITEM_DUAL&&!p1->isDual()) {
                    p1->setDual(true);
                    push_evt_queue(CANCEL_DUAL, 5000, p1->getID());
                }
                if(eb.getType() == ITEM_MAGNET&&p1->isMagnet()){
                    p1->setMagnet(true);
                    push_evt_queue(CANCEL_MAGNET, 5000, p1->getID());
                }
                flag = true;
            }

            // p2의 충돌체크
            player_rt = { DEFXPOS - 25, p2->getY() - 25, DEFXPOS + 25, p2->getY() + 25 };
            if (PtInRect(&player_rt, { eb.GetX(),eb.GetY() }) == 1 && p2->zombieCount() < 1) {
                if (eb.getType() == ITEM_COIN) get_coin += 10;
                if (eb.getType() == ITEM_DUAL && !p2->isDual()) {
                    p2->setDual(true);
                    push_evt_queue(CANCEL_DUAL, 5000, p2->getID());
                }
                if (eb.getType() == ITEM_MAGNET && p2->isMagnet()) {
                    p2->setMagnet(true);
                    push_evt_queue(CANCEL_MAGNET, 5000, p2->getID());
                }
                flag = true;
            }

            return flag;
        });
    }

    
    void deleteEnemy() // 몬스터 삭제
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        std::erase_if(enemies, [this](Enemy& e) {
            if (e.getHP() == 0) {
            
                int create_item = rand() % 10;
                if (create_item > 1) {
                    drop_Items.push_back({ ITEM_COIN,e.getPosition().first,e.getPosition().second });
                }
                else if (create_item == 0) {
                    drop_Items.push_back({ ITEM_DUAL,e.getPosition().first,e.getPosition().second });
                }
                else {
                    drop_Items.push_back({ ITEM_MAGNET,e.getPosition().first,e.getPosition().second });
                }

            }
            return e.getHP() == 0;
        });
       
    }

    void Create_enemy()
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        enemies.clear();
        if (clear_set % 4 < 3) { //중간세트
            int mid3 = 0;
            if (clear_set % 4 == 2) mid3 = 1;
            //중간몬스터생성
            enemies.push_back({ ENEMY_1,100,180,50 + (clear_set % 4 * 10) + (clear_stage * 10) });
            //작은 몬스터 생성
            enemies.push_back({ ENEMY_0,220,100,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,280,265,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,220,450,10 + (clear_stage * 5) });

            // std::cout <<"몬스터 생성!"<< enemies[0].getType() << " " << enemies[0].getPosition().first << enemies[0].getPosition().second << std::endl;
        }
        else { //보스세트
            //큰몬스터
            enemies.push_back({ ENEMY_4,10,150,100 + (clear_stage * 20) });
            //작은 몬스터 생성
            enemies.push_back({ ENEMY_0,210,100,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,240,170,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,270,265,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,240,350,10 + (clear_stage * 5) });
            enemies.push_back({ ENEMY_0,210,450,10 + (clear_stage * 5) });
        }
    }

    void sendSetup(SC_OBJECT_MOVE_PACKET& p)
    {
        std::lock_guard<std::mutex> ll{ update_lock };
        int i = 0;
        // enemies
        for (i = 0; i < enemies.size(); ++i) {
            p.objs_type[i] = enemies[i].getType();
            p.objs_x[i] = enemies[i].getPosition().first;
            p.objs_y[i] = enemies[i].getPosition().second;
            p.objs_hp[i] = enemies[i].getHP();
        }
        //Items
        int item_cnt = 0;
        for (int j = i; j < i + drop_Items.size(); ++j) {
            p.objs_type[j] = drop_Items[item_cnt].getType();
            p.objs_x[j] = drop_Items[item_cnt].getPosition().first;
            p.objs_y[j] = drop_Items[item_cnt].getPosition().second;
            item_cnt++;
        }
        i += drop_Items.size();
        int m = 0;
        // player bullet
        for (int j = i; j < i + p_bullets.size(); ++j) {
            p.objs_type[j] = p_bullets[m].getType();
            p.objs_x[j] = p_bullets[m].getPosition().first;
            p.objs_y[j] = p_bullets[m].getPosition().second;

            m++;
        }
        i += p_bullets.size();
        int enemy_bullet_cnt = 0;
        for (int j = i; j < i + e_bullets.size()&& j< 100; ++j) {
            p.objs_type[j] = ENEMY_BULLETS;
            p.objs_x[j] = e_bullets[enemy_bullet_cnt].getPosition().first;
            p.objs_y[j] = e_bullets[enemy_bullet_cnt].getPosition().second;
            enemy_bullet_cnt++;
        }
    }


    // Getter - Setter
    unsigned short getP1Y() { return p1->getY(); }
    unsigned short getP2Y() { return p2->getY(); }
    std::string getP1ID() { return p1->getID(); }
    std::string getP2ID()
    {
        if (p2 == nullptr) return "";
        return p2->getID();
    }
    void setP1(Player* p) { p1 = p; }
    void setP2(Player* p) { p2 = p; }
    Player* getP1() { return p1; }
    Player* getP2() { return p2; }
    void setisPlaying(bool a) { isPlaying = a; }
    bool getisPlaying() { return isPlaying; }
    bool getisDealer(std::string myid) { return myid == dealer_id; }
    void setDealer(std::string id) { dealer_id = id; }
    unsigned short number() { return p_bullets.size() + enemies.size() + e_bullets.size(); }
    unsigned short getEnemy_cnt() { return enemies.size(); }
    bool broadcast(char* res, size_t size)
    {
        bool ret = false;
        if (p1 != nullptr) ret = p1->broadcast(res, size);
        if (p2 != nullptr) ret = ret && p2->broadcast(res, size);
        return ret;
    }
    unsigned short getHeart() { return heart; }
    unsigned int getScore() { return score; }
    void addScore(unsigned short i)
    {
        score += i; // 무조건 싱글로 실행될듯 락 필요없음
    }
    unsigned int getCoin() { return get_coin; }
};

class EVENT
{
public:
    TASK_TYPE evt_type;
    std::string room_id;
    std::chrono::system_clock::time_point do_time;

    EVENT() {}

    void setup(TASK_TYPE evt, int s_time, std::string& rid) // time -> ms
    {
        evt_type = evt;
        room_id = rid;
        do_time = std::chrono::system_clock::now() + std::chrono::milliseconds(s_time);
    }

    std::chrono::system_clock::time_point& getTime() { return do_time; }

    bool operator<(const EVENT& other) const
    {
        return do_time > other.do_time;
    }
};

std::unordered_map<std::string, Room*> roomInfo;
std::mutex roomLock;
std::unordered_map<std::string, Player> players;
concurrency::concurrent_priority_queue<EVENT> evt_queue;
concurrency::concurrent_priority_queue<EVENT> task_queue;

// 유저데이터 관리용 : 전체 데이터 덮어쓰기
bool save_all_player_info()
{
    // 덮어쓰기 모드로 파일 열기 (기본 설정)
    std::ofstream out{ "data.txt", std::ios::out | std::ios::trunc };

    if (!out.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt for writing" << std::endl;
        return false;
    }

    for (auto& pair : players) { // players 맵의 모든 항목 순회
        auto& player = pair.second;
        out << player.getID() << " "   // ID
            << player.getCoin() << " " // Coin
            << player.getHigh_score()   // High Score
            << std::endl;              // 각 플레이어 정보를 한 줄로 저장
    }

    return true; // 파일 자동으로 닫힘 (RAII)
}
// 새 유저 추가하기
bool add_new_player(const std::string& id, unsigned int coin, unsigned int highScore)
{
    // 추가 모드로 파일 열기
    std::ofstream out{ "data.txt", std::ios::out | std::ios::app };

    if (!out.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt for appending" << std::endl;
        return false;
    }

    // 새 유저 데이터를 추가
    out << id << " " << coin << " " << highScore << std::endl;

    return true; // 파일 자동으로 닫힘 (RAII)
}
// 기존 유저 추가하기(서버 실행시에 로드)
bool read_player_info() // 데이터 처리
{
    std::ifstream in{ "data.txt" }; // data.txt 파일 열기

    if (!in.is_open()) { // 파일이 제대로 열렸는지 확인
        std::cerr << "Failed to open data.txt" << std::endl;
        return false;
    }

    std::string stmp;
    unsigned int ctmp;
    unsigned int hstmp;
    while (in >> stmp >> ctmp >> hstmp) { // ID, coin, highScore 읽기
        players[stmp].init(stmp, hstmp, ctmp);
    }

    in.close(); // 파일 닫기
    return true;
}
// 랭킹
bool send_top_high_scores(SOCKET& s) {
    SC_RANKING_PACKET res;
    res.size = sizeof(SC_RANKING_PACKET);
    res.type = SC_RANKING;

    std::vector<std::pair<std::string, unsigned int>> playerData;
    for (auto& p : players) {
        playerData.emplace_back(p.second.getID(), p.second.getHigh_score());
    }

    // 점수를 기준으로 내림차순 정렬
    std::sort(playerData.begin(), playerData.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second; // 점수 비교
        });

    // 하드... 복붙
    int cnt = 0;

    strcpy(res.id1, playerData[cnt].first.c_str());
    res.hs1 = playerData[cnt].second;
    cnt++;

    strcpy(res.id2, playerData[cnt].first.c_str());
    res.hs2 = playerData[cnt].second;
    cnt++;

    strcpy(res.id3, playerData[cnt].first.c_str());
    res.hs3 = playerData[cnt].second;
    cnt++;

    strcpy(res.id4, playerData[cnt].first.c_str());
    res.hs4 = playerData[cnt].second;
    cnt++;

    strcpy(res.id5, playerData[cnt].first.c_str());
    res.hs5 = playerData[cnt].second;
    cnt++;

    // 패킷 전송
    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(res), 0);
    if (ret == SOCKET_ERROR) {
        SERVER_err_display("send() failed");
        return false;
    }
    return true;
}

bool send_login_packet(SOCKET& s, CS_LOGIN_PACKET* p) // 로그인 패킷 보내는 함수 따로 뺌.
{
    SC_LOGIN_RESULT_PACKET res;
    res.size = sizeof(SC_LOGIN_RESULT_PACKET);
    res.type = SC_LOGIN_RESULT;

    int k = players.count(p->id);
    auto& pl = players[p->id];
    pl.setID(p->id);
    if (pl.isPlaying()) { // 플레이어가 지금 접속중인가?
        res.success = false;
        res.is_new = false;
    }
    else {
        res.success = true;
        if (k == 0) // 같은 아이디의 플레이어가 존재하는가?
        {
            res.is_new = true;
            std::cout << "새로운 유저 " << p->id << "등장!" << std::endl;
            add_new_player(p->id, pl.getCoin(), pl.getHigh_score());
        }
        else {
            res.is_new = false;
            std::cout << "기존 유저 " << p->id << "등장!" << std::endl;
        }

        pl.setPlaying(true);
        res.coin = pl.getCoin();
        res.high_score = pl.getHigh_score();
    }

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_LOGIN_RESULT_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_room_change_packet(SOCKET& s, std::string& id)
{
    auto t_room = roomInfo[id];
    SC_ROOM_CHANGE_PACKET res;
    res.size = sizeof(SC_ROOM_CHANGE_PACKET);
    res.type = SC_ROOM_CHANGE;
    res.isPlaying = t_room->getisPlaying();
    res.isDealer = t_room->getisDealer(id);
    if (t_room->getP1ID() == id)  memcpy(res.other_pl, t_room->getP2ID().c_str(), ID_LEN);
    else memcpy(res.other_pl, t_room->getP1ID().c_str(), ID_LEN);

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_ROOM_CHANGE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_room_join_fail_packet(SOCKET& s, std::string& id)
{
    SC_ROOM_CHANGE_PACKET res;
    res.size = sizeof(SC_ROOM_CHANGE_PACKET);
    res.type = SC_ROOM_CHANGE;
    res.isPlaying = false;
    res.isDealer = false;
    memcpy(res.other_pl, "cant_join", strlen("cant_join") + 1);

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_ROOM_CHANGE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    std::cout << id << "가 없는방에 드가려고 시도함!" << std::endl;
    return true;
}

bool send_player_move_packet(SOCKET& s, std::string& id)
{
    SC_PLAYER_MOVE_PACKET res;
    res.size = sizeof(SC_PLAYER_MOVE_PACKET);
    res.type = SC_PLAYER_MOVE;
    if (roomInfo[id]->getP1ID() == id) { // 내가 p1이군
        res.this_y = roomInfo[id]->getP1Y();
        res.other_y = roomInfo[id]->getP2Y();
    }
    else {
        res.this_y = roomInfo[id]->getP2Y();
        res.other_y = roomInfo[id]->getP1Y();
    }

    res.hp = roomInfo[id]->getHeart();
    res.score = roomInfo[id]->getScore();
    res.skillCnt = players[id].getSkillCount();
    res.coin = roomInfo[id]->getCoin();
    if (roomInfo[id]->getP1ID() == id) { // 내가 p1이군
        res.skillEnd = roomInfo[id]->getP1()->getSkill();
    }
    else {
        res.skillEnd = roomInfo[id]->getP2()->getSkill();
    }
    std::cout << "하트 " << res.hp << "개 보냈다" << std::endl;
    // std::cout << id << "의 y좌표: " << res.this_y << std::endl;
    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_player_state_change_packet(SOCKET& s, std::string& id)
{
    SC_PLAYER_STATE_CHANGE_PACKET res;
    res.size = sizeof(SC_PLAYER_STATE_CHANGE_PACKET);
    res.type = SC_PLAYER_STATE_CHANGE;

    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_PLAYER_STATE_CHANGE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_object_move_packet(SOCKET& s, std::string& id)
{
    auto t_room = roomInfo[id];
    SC_OBJECT_MOVE_PACKET res;
    res.size = sizeof(SC_OBJECT_MOVE_PACKET);
    res.number = t_room->number();
    t_room->sendSetup(res);
    res.type = SC_OBJECT_MOVE;


    int ret = send(s, reinterpret_cast<char*>(&res), sizeof(SC_OBJECT_MOVE_PACKET), 0);
    if (ret == SOCKET_ERROR) { // 에러 처리
        int error = WSAGetLastError();
        SERVER_err_display("send failed");
        SERVER_err_display(error);  // 오류 코드 출력
        return false;
    }
    return true;
}

bool send_object_change_packet()
{
    return true;
}


bool process_packet(char* packet, SOCKET& s, std::string& id)
{
    switch (packet[2]) // 여기 정확하게 어디 들어오는지 봐야 할듯
    {
    case CS_JOIN_ROOM:
    {
        {
            std::lock_guard<std::mutex> ll{ roomLock };
            CS_JOIN_ROOM_PACKET* p = reinterpret_cast<CS_JOIN_ROOM_PACKET*>(packet);
            // 만약 roomInfo에 p.id로 된 room이 있을때 조인
            // 아닐때 새로 만들어서 초기화
            if ((roomInfo.find(p->id) != roomInfo.end()) && (false == roomInfo[p->id]->getisPlaying())) {  // roomID가 이미 존재하면
                roomInfo[p->id]->setP2(&players[id]);       // 플레이어를 해당 방에 추가
                roomInfo[id] = roomInfo[p->id];
                std::cout << "Player " << id << " joined existing room " << p->id << std::endl;
            }
            else { // roomID가 없다면 새로운 방을 생성하여 초기화
                if (p->id != id) {
                    send_room_join_fail_packet(s, id);
                    break;
                }
                else {
                    roomInfo[id] = new Room(); // roomInfo에 새로운 방 추가
                    roomInfo[id]->setP1(&players[id]);
                    roomInfo[id]->setDealer(id);
                    std::cout << "Player " << id << " created and joined new room " << id << std::endl;
                }
            }
        }

        if (false == send_room_change_packet(s, id)) {
            return false; // 전송 실패.
        }
        break;
    }
    case CS_MOVE: // 플레이어 캐릭터의 이동 -> 나와 동료의 업데이트된 위치 정보 반환.
    {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        players[id].setY(p->y);
        if (true == p->keyDown)
            std::cout << "발싸: " << players[id].getSkillCount() << std::endl;
        if ((true == p->keyDown) && (false == players[id].getSkill()) && (players[id].getSkillCount() >= SKILL_CNT)) {// 스킬사용시.
            players[id].setSkill(true);
            push_evt_queue(SKILL_END, SKILL_TIME, id);
        }

        if (false == send_player_move_packet(s, id)) return false;
        break;
    }
    case CS_ROOM_STATE: // 방 정보 변경
    {
        auto t_room = roomInfo[id];
        CS_ROOM_STATE_PACKET* p = reinterpret_cast<CS_ROOM_STATE_PACKET*>(packet);
        if (t_room->getisPlaying()) {
            if (id == t_room->getP2ID()) {
                send_room_change_packet(s, id);
            }
            send_player_move_packet(s, id);
            break;
        }
        {
            std::lock_guard<std::mutex> ll{ roomLock };
            if (p->isQuit) { // 나가기
                if (t_room->getP1ID() == id)
                    t_room->setP1(&players[t_room->getP2ID()]); // 2를 1로 바꾸기.
                t_room->setP2(nullptr);
            }


            if (id == t_room->getP1ID()) { // 1p만 동작
                if (p->isDealer) t_room->setDealer(id);
                else t_room->setDealer(t_room->getP2ID());
                if (p->isPlaying) t_room->setisPlaying(true);
            }
        }

        if (false == send_room_change_packet(s, id)) return false; // 전송 실패.
        break;
    }
    default:
        std::cout << "undefined packet" << std::endl;
        exit(-1);
    }
    return true;
}

int client_thread(SOCKET s) // 클라이언트와의 통신 스레드
{
    char recv_buf[BUFSIZE];
    std::string pid;

    // 로그인 처리 단계
    {
        // 1. 로그인 패킷 받기
        ZeroMemory(recv_buf, sizeof(recv_buf));
        int ret = recv(s, recv_buf, sizeof(CS_LOGIN_PACKET), MSG_WAITALL);
        if (ret == SOCKET_ERROR) { // 에러처리
            int error = WSAGetLastError();
            SERVER_err_display("recv() failed");
            SERVER_err_display(error);  // 오류 코드 출력
            closesocket(s);
            return -1;
        }
        else if (ret == 0) {
            std::cout << "클라이언트가 접속을 종료하다." << std::endl;
            closesocket(s);
            return 0;
        }


        // 1-1. 로그인 패킷 처리
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(recv_buf);
        client_info(s, p->id);
        if (send_login_packet(s, p)) {
            pid = p->id;
            players[pid].setSocket(s);
        }

        // 2. 방 관련 send - recv 
        while(true) {
            while (true) {
                send_top_high_scores(s);
                // 2-1. 참여할 방의 플레이어 아이디 recv하기.
                ZeroMemory(recv_buf, sizeof(recv_buf));
                int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), MSG_WAITALL);
                if (ret == SOCKET_ERROR) { // 에러처리
                    int error = WSAGetLastError();
                    SERVER_err_display("recv() failed");
                    SERVER_err_display(error);  // 오류 코드 출력
                }
                while (true) {
                    process_packet(recv_buf, s, pid);

                    if (roomInfo.find(pid) != roomInfo.end() && true == roomInfo[pid]->getisPlaying()) {
                        break;
                    }
                    else if (roomInfo.find(pid) == roomInfo.end()) {
                        ZeroMemory(recv_buf, sizeof(recv_buf));
                        int ret = recv(s, recv_buf, sizeof(CS_JOIN_ROOM_PACKET), MSG_WAITALL);
                        if (ret == SOCKET_ERROR) { // 에러처리
                            int error = WSAGetLastError();
                            SERVER_err_display("recv() failed");
                            SERVER_err_display(error);  // 오류 코드 출력
                        }
                    }
                    else {
                        ZeroMemory(recv_buf, sizeof(recv_buf));
                        int ret = recv(s, recv_buf, sizeof(CS_ROOM_STATE_PACKET), MSG_WAITALL);
                        if (ret == SOCKET_ERROR) { // 에러처리
                            int error = WSAGetLastError();
                            SERVER_err_display("recv() failed");
                            SERVER_err_display(error);  // 오류 코드 출력
                        }
                    }
                }

                if (true == roomInfo[pid]->getisPlaying()) {
                    std::cout << pid << " 플레이씬 넘어가기" << std::endl;
                    break; // 게임 시작
                }
            }

            if (roomInfo[pid]->getP1ID() == pid) {// 호스트이면
                // 그 방의 적 생성, 총알 이동도 호스트 혼자 처리
                roomInfo[pid]->Create_enemy();
                push_evt_queue(MOVE_PLAYER_BULLET, 100, pid); // 총알이동
                push_evt_queue(CREATE_SET, 0, pid); // 세트를 클리어했는지 체크하는 이벤트
                push_evt_queue(AI_MOVE, 100, pid); //몬스터이동
            }
            push_evt_queue(FIRE_PLAYER_BULLET, 0, pid); // 총알발사
            push_evt_queue(FIRE_ENEMY_BULLET, 0, pid); // 적총알발사
            send_player_move_packet(s, pid);
            while (true) {
               

                send_object_move_packet(s, pid);
                if (roomInfo[pid]->getHeart() == 0) break; // 게임 종료

                // recv.CS_MOVE_PACKET
                ZeroMemory(recv_buf, sizeof(recv_buf));
                int ret = recv(s, recv_buf, sizeof(CS_MOVE_PACKET), MSG_WAITALL);
                if (ret == SOCKET_ERROR) { // 에러처리
                    int error = WSAGetLastError();
                    SERVER_err_display("recv() failed");
                    SERVER_err_display(error);  // 오류 코드 출력
                }
                process_packet(recv_buf, s, pid);
              
            }
            
            // 게임 종료 -> 랭킹 등록
            auto score = roomInfo[pid]->getScore();
            if (players[pid].getHigh_score() < score)
                players[pid].setHighScore(score);
            if (roomInfo[pid]->getP1ID() == pid) { // 방장만
                save_all_player_info();
                auto p = roomInfo[pid];
                roomInfo.erase(pid);
                roomInfo.erase(p->getP2ID());
                delete p;
            }
        }
    }
}

// timer thread: ai 동작
void timer_thread()
{
    while (true)
    {
        EVENT ev;
        if (! evt_queue.empty() && evt_queue.try_pop(ev))
        {
            if (ev.getTime() <= std::chrono::system_clock::now())
                task_queue.push(ev);
            else
                evt_queue.push(ev);
        }
    }
}

void push_evt_queue(TASK_TYPE ev, int time, std::string& rid) // time: milisecond 단위.
{
    EVENT evt;
    evt.setup(ev, time, rid);
    evt_queue.push(evt);
}

void addSkillCount(OTYPE type, std::string& id)
{
    switch (type) // 점수증가
    {
    case P1_BULLET:
        roomInfo[id]->getP1()->addSkillCount();
        roomInfo[id]->addScore(DAMAGE);
    case P1_SKILLBULLET:
        roomInfo[id]->getP1()->addSkillCount();
        roomInfo[id]->addScore(DAMAGE * 2);
        break;

    case P2_BULLET:
        roomInfo[id]->getP1()->addSkillCount();
        roomInfo[id]->addScore(DAMAGE);
    case P2_SKILLBULLET:
        roomInfo[id]->getP2()->addSkillCount();
        roomInfo[id]->addScore(DAMAGE * 2);
        break;
    }
}

void push_evt_queue(EVENT& evt)
{
    evt_queue.push(evt);
}

// task 처리하는 스레드 -> 룸 당 하나였으면 좋겠는데 어떻게 하면 좋을지 모르겠음.
void ai_thread()
{
    while (true) {
        EVENT ev;
        if (task_queue.empty() || !task_queue.try_pop(ev)) continue;
        if (roomInfo[ev.room_id] == nullptr)continue;

        switch (ev.evt_type) {
        case FIRE_PLAYER_BULLET:
            roomInfo[ev.room_id]->createPbullet(ev.room_id);
            push_evt_queue(FIRE_PLAYER_BULLET, 500, ev.room_id); // .5초에 한개씩 발사
            break;

        case MOVE_PLAYER_BULLET:
            if (ev.room_id == roomInfo[ev.room_id]->getP1ID()) {
                roomInfo[ev.room_id]->doBulletsMove();
                push_evt_queue(MOVE_PLAYER_BULLET, 100, ev.room_id); // 총알이동
                roomInfo[ev.room_id]->deletebullet(ev.room_id); // 총알 충돌체크
                roomInfo[ev.room_id]->deleteEnemy();
            }
            break;

        case FIRE_ENEMY_BULLET:
            roomInfo[ev.room_id]->createEbullet(ev.room_id);
            push_evt_queue(FIRE_ENEMY_BULLET, 1000, ev.room_id); // 1초에 한개씩 발사
            break;

        case AI_MOVE:
            roomInfo[ev.room_id]->doAIMove();
            push_evt_queue(AI_MOVE, 100, ev.room_id);
            break;

        case SKILL_END:
            players[ev.room_id].setSkill(false);
            players[ev.room_id].resetSkillCount();
            break;

        case CREATE_SET:
            if (roomInfo[ev.room_id]->getEnemy_cnt() < 1) { 
                roomInfo[ev.room_id]->clear_set++;
                if (roomInfo[ev.room_id]->clear_set % 4 == 0 && roomInfo[ev.room_id]->clear_set != 0) roomInfo[ev.room_id]->clear_stage++;
                roomInfo[ev.room_id]->Create_enemy();
            }
            push_evt_queue(CREATE_SET, 1000, ev.room_id);
            break;
      
        case ENABLE_COLLISION:
            players[ev.room_id].setZombieCnt(0);
            break;

        case CANCEL_DUAL:
            players[ev.room_id].setDual(0);
            break;

        case CANCEL_MAGNET:
            players[ev.room_id].setMagnet(0);
            break;

        default:
            std::cout << "unknown event" << std::endl;
            exit(-1);
        }
    }
}


int main()
{
    read_player_info();

    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) SERVER_err_quit("socket()");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) SERVER_err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) SERVER_err_quit("listen()");

    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;

    std::vector<std::thread> threads;  // std::thread를 저장할 벡터
    std::thread timer_th{ timer_thread };
    std::thread ai_th{ ai_thread };

    while (true) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "accept() failed" << std::endl;
            break;
        }

        // 스레드 생성...
        threads.emplace_back(client_thread, client_sock);
    }

    for (auto& th : threads) {
        th.join();
    }

    closesocket(listen_sock);
    WSACleanup();
}