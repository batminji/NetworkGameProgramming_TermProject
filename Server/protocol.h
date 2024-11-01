#pragma once
#include "stdafx.h"

enum PACKET {};

// client to server

struct CS_LOGIN_PACKET
{
	unsigned short size;
	PACKET type;
	char array[ID_LEN];
};

// server to client

struct SC_LOGIN_SUCCESS_PACKET
{
	unsigned short size;
	PACKET type;
};

struct SC_LOGIN_FAIL_PACKET
{
	unsigned short size;
	PACKET type;
};