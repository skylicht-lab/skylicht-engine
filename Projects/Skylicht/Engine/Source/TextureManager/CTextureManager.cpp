#include "pch.h"
#include "CTextureManager.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	const char* CTextureManager::GlobalPackage = "Global";

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

	void CTextureManager::registerTexture(ITexture* tex)
	{
		if (tex == NULL)
			return;

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			if ((*i)->texture == tex)
				return;

			i++;
		}

		m_textureList.push_back(new STexturePackage());

		STexturePackage* package = m_textureList.back();
		package->package = m_currentPackage;
		package->texture = tex;
	}

	void CTextureManager::removeAllTexture()
	{
		IVideoDriver* driver = getVideoDriver();

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			char log[512];
			sprintf(log, "Remove Texture: %s", (*i)->texture->getName().getPath().c_str());
			os::Printer::log(log);

			driver->removeTexture((*i)->texture);
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
			if ((*i)->texture == tex)
			{
				driver->removeTexture((*i)->texture);
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
		bool needContinue = false;
		do
		{
			needContinue = false;

			std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
			while (i != end)
			{
				if ((*i)->package == namePackage)
				{
					char log[512];
					sprintf(log, "Remove Texture: %s", (*i)->texture->getName().getPath().c_str());
					os::Printer::log(log);

					driver->removeTexture((*i)->texture);
					delete (*i);

					m_textureList.erase(i);
					needContinue = true;
					break;
				}
				i++;
			}

		} while (needContinue);

	}

	ITexture* CTextureManager::getTextureFromRealPath(const char* path)
	{
		IVideoDriver* driver = getVideoDriver();

		ITexture* texture = NULL;
		texture = driver->getTexture(path);

		if (texture)
			registerTexture(texture);
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
		char ansiPath[512];

		IVideoDriver* driver = getVideoDriver();
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		std::string fixPath = CPath::normalizePath(path);
		strcpy(ansiPath, fixPath.c_str());

		// try to load compress texture
		if (driver->getDriverType() == video::EDT_OPENGLES)
		{
#if defined(ANDROID) || defined(IOS)
			// etc compress
			CStringImp::replacePathExt(ansiPath, ".etc2");
#elif defined(MACOS)
			// dxt compress
			CStringImp::replacePathExt(ansiPath, ".dds");
#elif defined(__EMSCRIPTEN__)
			if (driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT) == true)
				CStringImp::replacePathExt(ansiPath, ".dds");
			else
				CStringImp::replacePathExt(ansiPath, ".etc2");
#else
			CStringImp::replacePathExt(ansiPath, ".pvr");
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
								return false;
							}
						}
					}
				}
			}
		}

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

	ITexture* CTextureManager::getTexture(const char* path)
	{
		char ansiPath[512];

		std::string fixPath = CPath::normalizePath(path);
		strcpy(ansiPath, fixPath.c_str());

		IVideoDriver* driver = getVideoDriver();
		io::IFileSystem* fs = getIrrlichtDevice()->getFileSystem();

		// try to load compress texture
		if (driver->getDriverType() == video::EDT_OPENGLES)
		{
#if defined(ANDROID) || defined(IOS)
			// etc compress
			CStringImp::replacePathExt(ansiPath, ".etc2");
#elif defined(MACOS)
			// dxt compress
			CStringImp::replacePathExt(ansiPath, ".dds");
#elif defined(__EMSCRIPTEN__)
			if (driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT) == true)
				CStringImp::replacePathExt(ansiPath, ".dds");
			else
				CStringImp::replacePathExt(ansiPath, ".etc2");
#else
			CStringImp::replacePathExt(ansiPath, ".pvr");
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
								sprintf(errorLog, "Can not load texture (file not found): %s", path);
								os::Printer::log(errorLog);
								return NULL;
							}
						}
					}
				}
			}
		}

		ITexture* texture = NULL;
		texture = driver->getTexture(ansiPath);

		// register the texture
		if (texture)
			registerTexture(texture);
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
			char ansiPath[512];
			strcpy(ansiPath, paths[i].c_str());

			// try to load compress texture
			if (driver->getDriverType() == video::EDT_OPENGLES)
			{
#if defined(ANDROID)
				// etc compress
				CStringImp::replacePathExt(ansiPath, ".etc2");
#elif defined(__EMSCRIPTEN__)
				if (driver->queryFeature(EVDF_TEXTURE_COMPRESSED_DXT) == true)
					CStringImp::replacePathExt(ansiPath, ".dds");
				else
					CStringImp::replacePathExt(ansiPath, ".etc2");
#else
				CStringImp::replacePathExt(ansiPath, ".pvr");
#endif
			}
			else if (driver->getDriverType() == video::EDT_DIRECT3D11 || driver->getDriverType() == video::EDT_OPENGL)
				CStringImp::replacePathExt(ansiPath, ".dds");

			bool loadImage = true;

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
									sprintf(errorLog, "Can not load array texture (file not found): %s", paths[i].c_str());
									os::Printer::log(errorLog);
									loadImage = false;
								}
							}
						}
					}
				}
			}

			IImage* image = NULL;

			if (loadImage == true)
			{
				char log[1024];
				sprintf(log, "Load array texture: %s", ansiPath);
				os::Printer::log(log);

				io::IReadFile* file = fs->createAndOpenFile(ansiPath);
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
						sprintf(errorLog, "Error: Texture size is not equal %d-%d: %s", w, h, ansiPath);
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
				registerTexture(texture);
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
			registerTexture(texture);
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s", paths[0].c_str());
			os::Printer::log(errorLog);
		}

		return texture;
	}
}
