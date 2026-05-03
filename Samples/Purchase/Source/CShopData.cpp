#include "pch.h"
#include "CShopData.h"

CShopData::CShopData() :
	CObjectSerializable("shop_data"),
	Iap(this, "iap"),
	Item(this, "item"),
	Value(this, "value"),
	Price(this, "price"),
	Sprite(this, "sprite"),
	Icon(this, "icon")
{

}

CShopData::~CShopData()
{

}