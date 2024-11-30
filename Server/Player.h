#pragma once
#include "stdafx.h"

class Player
{
private:
    SOCKET s = -1;
    std::string id = "";
    bool inGame = false; // ���� �������� ���̵��ΰ�?
    unsigned short y = 300;
    unsigned int high_score = 0;
    unsigned int coin = 0; // �����÷ο� ���� int�� ����

    // ������ �������� �� 
    bool isSkill = false;
    unsigned short skillCount = 0; // ����� ��Ҵ���.


public:

    Player() {}

    bool broadcast(char* res, size_t size)
    {
        int ret = send(s, reinterpret_cast<char*>(&res), size, 0);
        if (ret == SOCKET_ERROR) { // ���� ó��
            int error = WSAGetLastError();
            SERVER_err_display("send failed");
            SERVER_err_display(error);  // ���� �ڵ� ���
            return false;
        }
        return true;
    }

    void init(std::string& str, unsigned int high_score, unsigned short gold)
    {
        this->high_score = high_score;
        this->coin = gold;
        id = str;
        s = -1; // ���� ���̴ϱ� ���� �ʱ�ȭ
    }

    // getter - setter �η�
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
};