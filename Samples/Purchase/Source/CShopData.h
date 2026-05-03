#pragma once

#include "Serializable/CObjectSerializable.h"

class CShopData : public CObjectSerializable
{
public:
	CStringProperty Iap;
	CStringProperty Item;
	CIntProperty Value;
	CFloatProperty Price;
	CStringProperty Sprite;
	CStringProperty Icon;

	std::string PriceLocalize;
public:
	CShopData();

	virtual ~CShopData();
};