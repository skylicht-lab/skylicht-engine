#ifndef JSON_JSON_H_INCLUDED
# define JSON_JSON_H_INCLUDED

# include "autolink.h"
# include "value.h"
# include "reader.h"
# include "writer.h"
# include "features.h"

namespace Json
{
	JSON_API Json::Reader* createReader();

	JSON_API void destroyReader(Json::Reader* reader);

	JSON_API Json::Value* createValue();

	JSON_API void destroyValue(Json::Value* value);
}

#endif // JSON_JSON_H_INCLUDED
