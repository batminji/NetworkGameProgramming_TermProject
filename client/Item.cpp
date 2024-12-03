#include "Item.h"
void coin::render(HDC mdc)
{
	coin_bmp->TransparentBlt(mdc, x, y, 30, 30, 0, 0, 53, 53, RGB(0, 255, 0));
}


void margnet::render(HDC mdc)
{
	magnet_bmp->TransparentBlt(mdc, x, y, 30, 30, 0, 0, 53, 53, RGB(0, 255, 0));
}

void dual::render(HDC mdc)
{
	dual_bmp->TransparentBlt(mdc, x, y, 30, 30, 0, 0, 53, 53, RGB(0, 255, 0));
}

Item::Item(const unsigned short ax, const unsigned short ay)
{
	coin_bmp = &ResourceManager::getInstance().coin_bmp;
	dual_bmp = &ResourceManager::getInstance().dual_bmp;
	magnet_bmp = &ResourceManager::getInstance().magnet_bmp;

	x = ax;
	y = ay;
}

Item::Item()
{

}
