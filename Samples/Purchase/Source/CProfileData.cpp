#include "pch.h"
#include "CProfileData.h"

IMPLEMENT_SINGLETON(CProfileData);

CProfileData::CProfileData() :
	Gold(0),
	Diamond(0)
{
	Name = L"Username";
}

CProfileData::~CProfileData()
{

}
