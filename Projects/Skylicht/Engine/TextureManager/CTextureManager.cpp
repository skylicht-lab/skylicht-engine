#include "pch.h"
#include "CTextureManager.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	const char* GlobalPackage = "Global";

	IMPLEMENT_SINGLETON(CTextureManager);

	const char* CTextureManager::getGlobalName()
	{
		return GlobalPackage;
	}

	CTextureManager::CTextureManager() :
		m_nullNormalMap(NULL),
		m_nullTexture(NULL)
	{
		m_currentPackage = GlobalPackage;
		m_loadCommonPos = 0;
	}

	CTextureManager::~CTextureManager()
	{
		removeAllTexture();

		IVideoDriver* driver = getVideoDriver();
		if (m_nullNormalMap)
		{
			driver->removeTexture(m_nullNormalMap);
			m_nullNormalMap = NULL;
		}

		if (m_nullTexture)
		{
			driver->removeTexture(m_nullTexture);
			m_nullTexture = NULL;
		}
	}

	std::vector<std::string> CTextureManager::getTextureExts()
	{
		std::vector<std::string> textureExts = { "png", "jpg", "jpeg", "tga", "bmp" };
		return textureExts;
	}

	bool CTextureManager::isTextureExt(const char* ext)
	{
		std::vector<std::string> listExt = getTextureExts();
		for (auto s : listExt)
			if (s == ext)
				return true;
		return false;
	}

	void CTextureManager::registerTexture(ITexture* tex, const char* path)
	{
		if (tex == NULL)
			return;

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			if ((*i)->Texture == tex)
				return;

			i++;
		}

		m_textureList.push_back(new STexturePackage());

		STexturePackage* package = m_textureList.back();
		package->Package = m_currentPackage;
		package->Texture = tex;
		package->Path = path;
	}

	void CTextureManager::removeAllTexture()
	{
		IVideoDriver* driver = getVideoDriver();

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			ITexture* texture = (*i)->Texture;

			char log[1024];
			sprintf(log, "Remove texture: %s - refCount: %d",
				texture->getName().getPath().c_str(),
				texture->getReferenceCount() - 1
			);
			os::Printer::log(log);

			driver->removeTexture(texture);
			delete (*i);
			i++;
		}
		m_textureList.clear();
	}

	void CTextureManager::removeTexture(ITexture* tex)
	{
		IVideoDriver* driver = getVideoDriver();

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			ITexture* texture = (*i)->Texture;

			if (texture == tex)
			{
				char log[1024];
				sprintf(log, "Remove texture: %s - refCount: %d",
					texture->getName().getPath().c_str(),
					texture->getReferenceCount() - 1);
				os::Printer::log(log);

				driver->removeTexture(texture);
				delete (*i);

				m_textureList.erase(i);
				break;
			}
			i++;
		}
	}

	void CTextureManager::removeTexture(const char* namePackage)
	{
		IVideoDriver* driver = getVideoDriver();
		char log[1024];

		std::map<ITexture*, int> skips;

		int pos = 0;
		bool needContinue = false;
		do
		{
			needContinue = false;
			std::vector<STexturePackage*>::iterator i = m_textureList.begin() + pos, end = m_textureList.end();
			while (i != end)
			{
				if ((*i)->Package == namePackage)
				{
					ITexture* texture = (*i)->Texture;
					if (texture->getReferenceCount() == 1)
					{
						sprintf(log, "Remove Texture: %s - refCount: %d",
							texture->getName().getPath().c_str(),
							texture->getReferenceCount() - 1);
						os::Printer::log(log);

						driver->removeTexture(texture);
						delete (*i);

						m_textureList.erase(i);
						needContinue = true;
						break;
					}
					else
					{
						skips[texture] = 1;
					}
				}
				++i;
				++pos;
			}

		} while (needContinue);

		for (auto i : skips)
		{
			ITexture* texture = i.first;
			sprintf(log, "Skip remove Texture: %s - refCount: %d",
				texture->getName().getPath().c_str(),
				texture->getReferenceCount() - 1);
			os::Printer::log(log);
		}
	}

	const char* CTextureManager::getTexturePath(ITexture* tex)
	{
		for (auto i : m_textureList)
		{
			if (i->Texture == tex)
			{
				return i->Path.c_str();
			}
		}
		return NULL;
	}

	ITexture* CTextureManager::getTextureFromRealPath(const char* path)
	{
		IVideoDriver* driver = getVideoDriver();

		ITexture* texture = NULL;
		texture = driver->getTexture(path);

		if (texture)
			registerTexture(texture, path);
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s", path);
			os::Printer::log(errorLog);
		}

		return texture;
	}

	bool CTextureManager::existTexture(const char* path)
	{
		std::string realPath;

		if (!findRealTexturePath(path, realPath))
			return false;

		return true;
	}

	ITexture* CTextureManager::getTexture(const char* filename, const std::vector<std::string>& textureFolder)
	{
		ITexture* t = getTexture(filename);
		if (t != NULL)
			return t;

		char realFileName[512];

		CStringImp::getFileName(realFileName, filename);

		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		for (u32 i = 0, n = (u32)textureFolder.size(); i < n; i++)
		{
			std::string s = textureFolder[i];
			s += "/";
			s += realFileName;

			ITexture* texture = CTextureManager::getInstance()->getTexture(s.c_str());
			if (texture != NULL)
			{
				return texture;
			}

			if (realFileName != filename)
			{
				// test file name
				s = textureFolder[i];
				s += "/";
				s += filename;

				texture = CTextureManager::getInstance()->getTexture(s.c_str());
				if (texture != NULL)
				{
					return texture;
				}
			}
		}

		char errLog[512];
		sprintf(errLog, "[CTextureManager::getTexture] Load texture %s in list folders failed!", filename);
		os::Printer::log(errLog);
		return NULL;
	}

	bool CTextureManager::isTextureLoaded(const char* path)
	{
		std::string realPath;
		if (!findRealTexturePath(path, realPath))
			return false;

		for (STexturePackage* t : m_textureList)
		{
			if (t->Texture && t->Path == realPath.c_str())
			{
				return true;
			}
		}
		return false;
	}

	bool CTextureManager::findRealTexturePath(const char* path, std::string& result)
	{
		char ansiPath[1024];

		std::string fixPath = CPath::normalizePath(path);
		strcpy(ansiPath, fixPath.c_str());

		IVideoDriver* driver = getVideoDriver();
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		// try to load compress texture
		if (driver->getDriverType() == video::EDT_OPENGLES)
		{
#if defined(ANDROID)
			// etc compress
			CStringImp::replacePathExt(ansiPath, ".etc2");
#elif defined(IOS)
#if defined(USE_ETC_TEXTURE)
			CStringImp::replacePathExt(ansiPath, ".etc2");
#else
			CStringImp::replacePathExt(ansiPath, ".pvr");
#endif
#elif defined(MACOS)
			// dxt compress
			CStringImp::replacePathExt(ansiPath, ".dds");
#elif defined(__EMSCRIPTEN__)
			if (driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT) == true)
				CStringImp::replacePathExt(ansiPath, ".dds");
			else
				CStringImp::replacePathExt(ansiPath, ".etc2");
#else
			CStringImp::replacePathExt(ansiPath, ".tga");
#endif
		}
		else if (driver->getDriverType() == video::EDT_DIRECT3D11 || driver->getDriverType() == video::EDT_OPENGL)
		{
			CStringImp::replacePathExt(ansiPath, ".dds");
		}

		if (fs->existFile(ansiPath) == false)
		{
			CStringImp::replacePathExt(ansiPath, ".tga");

			if (fs->existFile(ansiPath) == false)
			{
				CStringImp::replacePathExt(ansiPath, ".bmp");

				if (fs->existFile(ansiPath) == false)
				{
					CStringImp::replacePathExt(ansiPath, ".png");

					if (fs->existFile(ansiPath) == false)
					{
						CStringImp::replacePathExt(ansiPath, ".jpeg");

						if (fs->existFile(ansiPath) == false)
						{
							CStringImp::replacePathExt(ansiPath, ".jpg");
							if (fs->existFile(ansiPath) == false)
							{
								char errorLog[512];
								sprintf(errorLog, "Texture file not found: %s", path);
								os::Printer::log(errorLog);
								return false;
							}
						}
					}
				}
			}
		}

		result = ansiPath;
		return true;
	}

	ITexture* CTextureManager::getTexture(const char* path)
	{
		std::string realPath;
		if (!findRealTexturePath(path, realPath))
			return NULL;

		IVideoDriver* driver = getVideoDriver();
		ITexture* texture = driver->getTexture(realPath.c_str());

		// register the texture
		if (texture)
			registerTexture(texture, realPath.c_str());
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s", path);
			os::Printer::log(errorLog);
		}

		// load null
		if (m_nullNormalMap == NULL)
			m_nullNormalMap = driver->getTexture("BuiltIn/Textures/NullNormalMap.png");

		if (m_nullTexture == NULL)
			m_nullTexture = driver->getTexture("BuiltIn/Textures/NullTexture.png");

		return texture;
	}

	ITexture* CTextureManager::getTextureArray(std::vector<std::string>& listTexture)
	{
		IVideoDriver* driver = getVideoDriver();
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		core::array<io::path> paths;

		for (u32 i = 0, n = (u32)listTexture.size(); i < n; i++)
		{
			std::string fixPath = CPath::normalizePath(listTexture[i]);
			paths.push_back(fixPath.c_str());
		}

		std::vector<IImage*> listImage;
		int w = 0;
		int h = 0;

		IImage* baseImage = NULL;

		for (u32 i = 0, n = paths.size(); i < n; i++)
		{
			std::string realPath;
			IImage* image = NULL;

			bool loadImage = findRealTexturePath(paths[i].c_str(), realPath);

			if (loadImage == true)
			{
				char log[1024];
				sprintf(log, "Load array texture: %s", realPath.c_str());
				os::Printer::log(log);

				io::IReadFile* file = fs->createAndOpenFile(realPath.c_str());
				if (file != NULL)
				{
					image = driver->createImageFromFile(file);
					file->drop();
				}
			}

			if (image != NULL)
			{
				int imageW = image->getDimension().Width;
				int imageH = image->getDimension().Height;

				if (i == 0)
				{
					w = imageW;
					h = imageH;

					baseImage = image;
				}

				if (image->isCompressed() == true)
				{
					if (w != imageW || h != imageH)
					{
						char errorLog[1024];
						sprintf(errorLog, "Error: Texture size is not equal %d-%d: %s", w, h, realPath.c_str());
						os::Printer::log(errorLog);

						image->drop();

						image = baseImage;
						baseImage->grab();
					}
				}
			}

			listImage.push_back(image);
		}

		ITexture* texture = NULL;

		if (listImage.size() > 0 && listImage[0] == NULL)
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture array: %s", paths[0].c_str());
			os::Printer::log(errorLog);
		}
		else
		{
			texture = driver->getTextureArray(listImage.data(), (u32)listImage.size());

			// register the texture
			if (texture)
				registerTexture(texture, paths[0].c_str());
			else
			{
				char errorLog[512];
				sprintf(errorLog, "Can not load texture array: %s", paths[0].c_str());
				os::Printer::log(errorLog);
			}
		}

		// release
		for (u32 i = 0, n = (u32)listImage.size(); i < n; i++)
		{
			if (listImage[i] != NULL)
				listImage[i]->drop();
		}

		return texture;
	}

	// getCubeTexture
	ITexture* CTextureManager::getCubeTexture(
		const char* pathX1,
		const char* pathX2,
		const char* pathY1,
		const char* pathY2,
		const char* pathZ1,
		const char* pathZ2)
	{
		std::string hash = pathX1;
		CStringImp::replaceAll(hash, std::string("_X1.png"), std::string(""));
		hash += ".cube";

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			if ((*i)->Path == hash)
				return (*i)->Texture;
			i++;
		}

		std::vector<std::string> paths;
		paths.push_back(pathX1);
		paths.push_back(pathX2);
		paths.push_back(pathY1);
		paths.push_back(pathY2);
		paths.push_back(pathZ1);
		paths.push_back(pathZ2);

		for (int j = 0; j < 6; j++)
		{
			char ansiPath[512];
			std::string fixPath = CPath::normalizePath(paths[j].c_str());
			strcpy(ansiPath, fixPath.c_str());
			paths[j] = ansiPath;
		}

		IVideoDriver* driver = getVideoDriver();
		IrrlichtDevice* device = getIrrlichtDevice();

		ITexture* texture = NULL;
		texture = driver->getTextureCube
		(
			paths[0].c_str(),
			paths[1].c_str(),
			paths[2].c_str(),
			paths[3].c_str(),
			paths[4].c_str(),
			paths[5].c_str()
		);

		// register the texture
		if (texture)
			registerTexture(texture, hash.c_str());
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s", paths[0].c_str());
			os::Printer::log(errorLog);
		}

		return texture;
	}

	ITexture* CTextureManager::createTransformTexture2D(const char* name, core::matrix4* transforms, int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();
		IrrlichtDevice* device = getIrrlichtDevice();

		int imageSizeW = 4 * w; // 4 pixels type A32B32G32R32F
		int imageSizeH = core::max_(h, 4);

		float* color = new float[4 * imageSizeW * imageSizeH];
		memset(color, 0, sizeof(float) * 4 * imageSizeW * imageSizeH);

		float* c = color;
		core::matrix4* m = transforms;

		for (int i = 0, n = w * h; i < n; i++)
		{
			memcpy(c, m->pointer(), sizeof(float) * 16);
			m++;
			c += 16;
		}

		core::dimension2d<u32> size(imageSizeW, imageSizeH);
		IImage* img = driver->createImageFromData(ECF_A32B32G32R32F, size, color);

		bool configCreateMipmap = driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
		ITexture* transformTexture = driver->addTexture(name, img);
		driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, configCreateMipmap);

		img->drop();
		delete[]color;

		if (transformTexture)
			registerTexture(transformTexture, name);

		return transformTexture;
	}

	ITexture* CTextureManager::createVectorTexture2D(const char* name, core::vector3df* vectors, int w, int h)
	{
		IVideoDriver* driver = getVideoDriver();
		IrrlichtDevice* device = getIrrlichtDevice();

		int imageSizeW = core::max_(w, 4);
		int imageSizeH = core::max_(h, 4);

		float* color = new float[4 * imageSizeW * imageSizeH];
		memset(color, 0, sizeof(float) * 4 * imageSizeW * imageSizeH);

		float* c = color;
		core::vector3df* p = vectors;

		for (int i = 0, n = w * h; i < n; i++)
		{
			c[0] = p->X;
			c[1] = p->Y;
			c[2] = p->Z;
			c[3] = 0.0f;

			p++;
			c += 4;
		}

		core::dimension2d<u32> size(imageSizeW, imageSizeH);
		IImage* img = driver->createImageFromData(ECF_A32B32G32R32F, size, color);

		bool configCreateMipmap = driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
		ITexture* transformTexture = driver->addTexture(name, img);
		driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, configCreateMipmap);

		img->drop();
		delete[]color;

		if (transformTexture)
			registerTexture(transformTexture, name);

		return transformTexture;
	}
}
