#include "json.h"

namespace Json
{
	Json::Reader* createReader()
	{
		return new Json::Reader();
	}

	void destroyReader(Json::Reader* reader)
	{
		delete reader;
	}

	Json::Value* createValue()
	{
		return new Json::Value();
	}

	void destroyValue(Json::Value* value)
	{
		delete value;
	}
}
