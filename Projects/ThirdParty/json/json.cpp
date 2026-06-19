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

	Json::FastWriter* createFastWriter()
	{
		return new Json::FastWriter();
	}

	Json::StyledWriter* createStyledWriter()
	{
		return new Json::StyledWriter();
	}

	Json::StyledStreamWriter* createStyledStreamWriter()
	{
		return new Json::StyledStreamWriter();
	}

	void destroyWriter(Json::Writer* writer)
	{
		delete writer;
	}
}
