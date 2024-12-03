#include "Item.h"


Item::Item(OTYPE otype , const unsigned short ax, const unsigned short ay)
{
	type = otype;
	coin_bmp = &ResourceManager::getInstance().coin_bmp;
	dual_bmp = &ResourceManager::getInstance().dual_bmp;
	magnet_bmp = &ResourceManager::getInstance().magnet_bmp;

	x = ax;
	y = ay;
}

Item::Item()
{

}

void Item::render(HDC mdc)
{
	switch (type)
	{
	case ITEM_COIN:
		coin_bmp->TransparentBlt(mdc, x, y, 52, 40,230*DataManager::getInstance().coin_ani_frame, 0,230, 130, RGB(0, 255, 0));
		break;
	case ITEM_MAGNET:
		magnet_bmp->TransparentBlt(mdc, x, y, 30, 30, 0, 0, 53, 53, RGB(0, 255, 0));
		break;
	case ITEM_DUAL:
		dual_bmp->TransparentBlt(mdc, x, y, 30, 30, 0, 0, 53, 53, RGB(0, 255, 0));
		break;
	default:
		break;
	}
}
