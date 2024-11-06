#include "Item.h"
void coin::render()
{
	coin_bmp.TransparentBlt(mdc, c.x, c.y, 52, 40, (ani % 8) * 230, 0, 230, 130, RGB(0, 255, 0));
}


void margnet::render()
{
}

void dual::render()
{
}

