#include "Utils/CSingleton.h"

class CProfileData
{
public:
	DECLARE_SINGLETON(CProfileData)

	CProfileData();

	~CProfileData();

public:

	std::wstring Name;

};