#pragma once
#include "stdafx.h"

class Player
{
private:
    SOCKET s = -1;
    std::string id = "";
    bool inGame = false; // 현재 접속중인 아이디인가?
    unsigned short y = 300;
    unsigned int high_score = 0;
    unsigned int coin = 0; // 오버플로우 떠서 int로 수정
    short zombie_cnt = 0; //1 이상이 되면 잠시 무적모드, 1이상이 되면 1프레임에 1씩 점점 올려서 30이 되면 다시 좀비카운트를 0으로,

    // 게임을 실행중일 때 
    bool isSkill = false;
    unsigned short skillCount = 0; // 몇마리를 잡았는지.

    bool magnet_eat = false;
    bool dual_eat = false;


public:

    Player() {}

    bool broadcast(char* res, size_t size)
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // 에러 처리
            int error = WSAGetLastError();
            SERVER_err_display("send failed");
            SERVER_err_display(error);  // 오류 코드 출력
            return false;
        }
        return true;
    }

    void init(std::string& str, unsigned int high_score, unsigned short gold)
    {
        this->high_score = high_score;
        this->coin = gold;
        id = str;
        s = -1; // 접속 전이니까 대충 초기화
    }

    // getter - setter 부류
    bool isPlaying() { return inGame; }
    void setPlaying(bool x) { inGame = x; }
    unsigned short getY() { return y; }
    void setY(unsigned short y) { this->y = y; }
    std::string getID() { return id; }
    void setID(std::string& str) { id = str; }
    void setID(char* str) { id = std::string(str); }
    void setSocket(SOCKET& s) { this->s = s; }
    unsigned short getCoin() { return coin; }
    unsigned int getHigh_score() { return high_score; }
    bool getSkill() { return isSkill; }
    void setSkill(bool b) { isSkill = b; }
    void addSkillCount() { skillCount++; }
    void setZombieCnt(short new_zombie_cnt) { zombie_cnt = new_zombie_cnt; }
    short zombieCount() { return zombie_cnt; }

    bool isMagnet() {return magnet_eat; }
    bool isDual() {return dual_eat; }
    void setMagnet(bool c) { magnet_eat = c; }
    void setDual(bool c) { dual_eat = c; }

    unsigned short getSkillCount() { return skillCount; }
    void resetSkillCount() { skillCount -= SKILL_CNT; }
};