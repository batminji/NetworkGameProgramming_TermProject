#include "Enemy.h"

Enemy::Enemy(short arg_type, RECT arg_rt, short arg_hp, short arg_cnt, short arg_frame, short arg_attack_type)
{
	type             = arg_type;
	rt               = arg_rt;
	hp               = arg_hp;
	cnt              = arg_cnt;
	frame            = arg_frame;
	attack_type       = arg_attack_type;

	bitmap = ResourceManager::getInstance().m[type];
}
