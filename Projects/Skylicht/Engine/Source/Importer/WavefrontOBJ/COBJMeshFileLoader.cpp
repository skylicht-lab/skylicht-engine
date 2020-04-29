#include "pch.h"
#include "COBJMeshFileLoader.h"
#include "TextureManager/CTextureManager.h"

#define WORD_BUFFER_LENGTH 512

namespace Skylicht
{
	COBJMeshFileLoader::COBJMeshFileLoader()
	{

	}

	COBJMeshFileLoader::~COBJMeshFileLoader()
	{

	}

	void COBJMeshFileLoader::addTextureFolder(const char *folder)
	{
		m_textureFolder.push_back(folder);
	}

	bool COBJMeshFileLoader::loadModel(const char *resource, CEntityPrefab* output, bool normalMap, bool texcoord2, bool batching)
	{
		return false;
	}

	const c8* COBJMeshFileLoader::readTextures(const c8* bufPtr, const c8* const bufEnd)
	{
		u8 type = 0;

		// map_Kd - diffuse color texture map
		// map_Ks - specular color texture map
		// map_Ka - ambient color texture map
		// map_Ns - shininess texture map
		if ((!strncmp(bufPtr, "map_bump", 8)) || (!strncmp(bufPtr, "bump", 4)))
			type = 1; // normal map
		else if ((!strncmp(bufPtr, "map_d", 5)) || (!strncmp(bufPtr, "map_opacity", 11)))
			type = 2; // opacity map
		else if (!strncmp(bufPtr, "map_refl", 8))
			type = 3; // reflection map

		// extract new material's name
		c8 textureNameBuf[WORD_BUFFER_LENGTH];
		bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

		f32 bumpiness = 6.0f;
		bool clamp = false;

		float materialParam = 0.0f;

		// handle options
		while (textureNameBuf[0] == '-')
		{
			if (!strncmp(bufPtr, "-bm", 3))
			{
				bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				materialParam = core::fast_atof(textureNameBuf);
				bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				continue;
			}
			else
				if (!strncmp(bufPtr, "-blendu", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-blendv", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-cc", 3))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-clamp", 6))
					bufPtr = readBool(bufPtr, clamp, bufEnd);
				else if (!strncmp(bufPtr, "-texres", 7))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-type", 5))
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				else if (!strncmp(bufPtr, "-mm", 3))
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
				}
				else if (!strncmp(bufPtr, "-o", 2)) // texture coord translation
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
				else if (!strncmp(bufPtr, "-s", 2)) // texture coord scale
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
				else if (!strncmp(bufPtr, "-t", 2))
				{
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);

					// next parameters are optional, so skip rest of loop if no number is found
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
					bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
					if (!core::isdigit(textureNameBuf[0]))
						continue;
				}
			// get next word
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		}

		if ((type == 1) && (core::isdigit(textureNameBuf[0])))
		{
			materialParam = core::fast_atof(textureNameBuf);
			bufPtr = goAndCopyNextWord(textureNameBuf, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		}

		if (clamp)
		{
			// getMaterial().setFlag(video::EMF_TEXTURE_WRAP, video::ETC_CLAMP);
		}

		std::string texname(textureNameBuf);

		if (texname.size() > 0)
		{
			video::ITexture * texture = CTextureManager::getInstance()->getTexture(texname.c_str());
			if (texture)
			{
				if (type == 0)
				{
					// getMaterial().setTexture(0, texture);
				}
				else if (type == 1)
				{
					// normal map
				}
				else if (type == 2)
				{
					// alpha map
				}
				else if (type == 3)
				{
					// reflection map
				}
			}
		}

		return bufPtr;
	}

	//! skip space characters and stop on first non-space
	const c8* COBJMeshFileLoader::goFirstWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
	{
		// skip space characters
		if (acrossNewlines)
			while ((buf != bufEnd) && core::isspace(*buf))
				++buf;
		else
			while ((buf != bufEnd) && core::isspace(*buf) && (*buf != '\n'))
				++buf;

		return buf;
	}


	//! skip current word and stop at beginning of next one
	const c8* COBJMeshFileLoader::goNextWord(const c8* buf, const c8* const bufEnd, bool acrossNewlines)
	{
		// skip current word
		while ((buf != bufEnd) && !core::isspace(*buf))
			++buf;

		return goFirstWord(buf, bufEnd, acrossNewlines);
	}


	//! Read until line break is reached and stop at the next non-space character
	const c8* COBJMeshFileLoader::goNextLine(const c8* buf, const c8* const bufEnd)
	{
		// look for newline characters
		while (buf != bufEnd)
		{
			// found it, so leave
			if (*buf == '\n' || *buf == '\r')
				break;
			++buf;
		}
		return goFirstWord(buf, bufEnd);
	}


	u32 COBJMeshFileLoader::copyWord(c8* outBuf, const c8* const inBuf, u32 outBufLength, const c8* const bufEnd)
	{
		if (!outBufLength)
			return 0;
		if (!inBuf)
		{
			*outBuf = 0;
			return 0;
		}

		u32 i = 0;
		while (inBuf[i])
		{
			if (core::isspace(inBuf[i]) || &(inBuf[i]) == bufEnd)
				break;
			++i;
		}

		u32 length = core::min_(i, outBufLength - 1);
		for (u32 j = 0; j < length; ++j)
			outBuf[j] = inBuf[j];

		outBuf[length] = 0;
		return length;
	}


	core::stringc COBJMeshFileLoader::copyLine(const c8* inBuf, const c8* bufEnd)
	{
		if (!inBuf)
			return core::stringc();

		const c8* ptr = inBuf;
		while (ptr < bufEnd)
		{
			if (*ptr == '\n' || *ptr == '\r')
				break;
			++ptr;
		}
		// we must avoid the +1 in case the array is used up
		return core::stringc(inBuf, (u32)(ptr - inBuf + ((ptr < bufEnd) ? 1 : 0)));
	}


	const c8* COBJMeshFileLoader::goAndCopyNextWord(c8* outBuf, const c8* inBuf, u32 outBufLength, const c8* bufEnd)
	{
		inBuf = goNextWord(inBuf, bufEnd, false);
		copyWord(outBuf, inBuf, outBufLength, bufEnd);
		return inBuf;
	}

	//! Read RGB color
	const c8* COBJMeshFileLoader::readColor(const c8* bufPtr, video::SColor& color, const c8* const bufEnd)
	{
		const u32 COLOR_BUFFER_LENGTH = 16;
		c8 colStr[COLOR_BUFFER_LENGTH];

		color.setAlpha(255);
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setRed((s32)(core::fast_atof(colStr) * 255.0f));
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setGreen((s32)(core::fast_atof(colStr) * 255.0f));
		bufPtr = goAndCopyNextWord(colStr, bufPtr, COLOR_BUFFER_LENGTH, bufEnd);
		color.setBlue((s32)(core::fast_atof(colStr) * 255.0f));
		return bufPtr;
	}


	//! Read 3d vector of floats
	const c8* COBJMeshFileLoader::readVec3(const c8* bufPtr, core::vector3df& vec, const c8* const bufEnd)
	{
		c8 wordBuffer[WORD_BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.X = -core::fast_atof(wordBuffer); // change handedness
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Y = core::fast_atof(wordBuffer);
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Z = core::fast_atof(wordBuffer);
		return bufPtr;
	}


	//! Read 2d vector of floats
	const c8* COBJMeshFileLoader::readUV(const c8* bufPtr, core::vector2df& vec, const c8* const bufEnd)
	{
		c8 wordBuffer[WORD_BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.X = core::fast_atof(wordBuffer);
		bufPtr = goAndCopyNextWord(wordBuffer, bufPtr, WORD_BUFFER_LENGTH, bufEnd);
		vec.Y = 1 - core::fast_atof(wordBuffer); // change handedness
		return bufPtr;
	}


	//! Read boolean value represented as 'on' or 'off'
	const c8* COBJMeshFileLoader::readBool(const c8* bufPtr, bool& tf, const c8* const bufEnd)
	{
		const u32 BUFFER_LENGTH = 8;
		c8 tfStr[BUFFER_LENGTH];

		bufPtr = goAndCopyNextWord(tfStr, bufPtr, BUFFER_LENGTH, bufEnd);
		tf = strcmp(tfStr, "off") != 0;
		return bufPtr;
	}
}