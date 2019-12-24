#include "pch.h"
#include "CTextureManager.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	const char *CTextureManager::GlobalPackage = "Global";

	CTextureManager::CTextureManager()
	{
		m_currentPackage = GlobalPackage;
		m_loadCommonPos = 0;
	}

	CTextureManager::~CTextureManager()
	{
		removeAllTexture();
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
		IVideoDriver *driver = getVideoDriver();

		std::vector<STexturePackage*>::iterator i = m_textureList.begin(), end = m_textureList.end();
		while (i != end)
		{
			char log[512];
			sprintf(log, "Remove Texture: %s\n", (*i)->texture->getName().getPath().c_str());
			os::Printer::log(log);

			driver->removeTexture((*i)->texture);
			delete (*i);
			i++;
		}
		m_textureList.clear();
	}

	void CTextureManager::removeTexture(ITexture *tex)
	{
		IVideoDriver *driver = getVideoDriver();

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

	void CTextureManager::removeTexture(const char *namePackage)
	{
		IVideoDriver *driver = getVideoDriver();
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
					sprintf(log, "Remove Texture: %s\n", (*i)->texture->getName().getPath().c_str());
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

	ITexture* CTextureManager::getTextureFromRealPath(const char *path)
	{
		IVideoDriver *driver = getVideoDriver();

		ITexture *texture = NULL;
		texture = driver->getTexture(path);

		if (texture)
			registerTexture(texture);
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s\n", path);
			os::Printer::log(errorLog);
		}

		return texture;
	}

	ITexture* CTextureManager::getTexture(const char *path)
	{
		char ansiPath[512];

		std::string fixPath = CPath::normalizePath(path);
		strcpy(ansiPath, fixPath.c_str());

		IVideoDriver *driver = getVideoDriver();
		IrrlichtDevice *device = getIrrlichtDevice();

		// try to load compress texture
		if (driver->getDriverType() == video::EDT_OPENGLES)
		{
#ifdef IOS
			CStringImp::replacePathExt(ansiPath, ".pvr");
#else
			CStringImp::replacePathExt(ansiPath, ".etc");
#endif
		}
		else if (driver->getDriverType() == video::EDT_DIRECT3D11 || driver->getDriverType() == video::EDT_OPENGL)
		{
			CStringImp::replacePathExt(ansiPath, ".dds");
		}

		if (device->getFileSystem()->existFile(ansiPath) == false)
		{
			CStringImp::replacePathExt(ansiPath, ".tga");

			if (device->getFileSystem()->existFile(ansiPath) == false)
			{
				CStringImp::replacePathExt(ansiPath, ".bmp");

				if (device->getFileSystem()->existFile(ansiPath) == false)
				{
					CStringImp::replacePathExt(ansiPath, ".png");

					if (device->getFileSystem()->existFile(ansiPath) == false)
					{
						CStringImp::replacePathExt(ansiPath, ".jpeg");

						if (device->getFileSystem()->existFile(ansiPath) == false)
						{
							CStringImp::replacePathExt(ansiPath, ".jpg");
							if (device->getFileSystem()->existFile(ansiPath) == false)
							{
								char errorLog[512];
								sprintf(errorLog, "Can not load texture (file not found): %s\n", path);
								os::Printer::log(errorLog);
								return NULL;
							}
						}
					}
				}
			}
		}

		ITexture *texture = NULL;
		texture = driver->getTexture(ansiPath);

		// register the texture
		if (texture)
			registerTexture(texture);
		else
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture: %s\n", path);
			os::Printer::log(errorLog);
		}

		return texture;
	}

	ITexture* CTextureManager::getTextureArray(std::vector<std::string>& listTexture)
	{
		IVideoDriver *driver = getVideoDriver();
		IrrlichtDevice *device = getIrrlichtDevice();

		core::array<io::path> paths;

		for (int i = 0, n = listTexture.size(); i < n; i++)
		{
			std::string fixPath = CPath::normalizePath(listTexture[i]);
			paths.push_back(fixPath.c_str());
		}

		std::vector<IImage*> listImage;
		int w = 0;
		int h = 0;

		IImage* baseImage = NULL;

		for (int i = 0, n = (int)paths.size(); i < n; i++)
		{
			char ansiPath[512];
			strcpy(ansiPath, paths[i].c_str());

			// try to load compress texture
			if (driver->getDriverType() == video::EDT_OPENGLES)
#ifdef IOS
				CStringImp::replacePathExt(ansiPath, ".pvr");
#else
				CStringImp::replacePathExt(ansiPath, ".etc");
#endif
			else if (driver->getDriverType() == video::EDT_DIRECT3D11 || driver->getDriverType() == video::EDT_OPENGL)
				CStringImp::replacePathExt(ansiPath, ".dds");

			bool loadImage = true;

			if (device->getFileSystem()->existFile(ansiPath) == false)
			{
				CStringImp::replacePathExt(ansiPath, ".tga");

				if (device->getFileSystem()->existFile(ansiPath) == false)
				{
					CStringImp::replacePathExt(ansiPath, ".bmp");

					if (device->getFileSystem()->existFile(ansiPath) == false)
					{
						CStringImp::replacePathExt(ansiPath, ".png");

						if (device->getFileSystem()->existFile(ansiPath) == false)
						{
							CStringImp::replacePathExt(ansiPath, ".jpeg");

							if (device->getFileSystem()->existFile(ansiPath) == false)
							{
								CStringImp::replacePathExt(ansiPath, ".jpg");

								if (device->getFileSystem()->existFile(ansiPath) == false)
								{
									char errorLog[512];
									sprintf(errorLog, "Can not load array texture (file not found): %s\n", paths[i].c_str());
									os::Printer::log(errorLog);
									loadImage = false;
								}
							}
						}
					}
				}
			}

			IImage *image = NULL;

			if (loadImage == true)
			{
				char log[512];
				sprintf(log, "Load array texture: %s", ansiPath);
				os::Printer::log(log);

				io::IReadFile* file = device->getFileSystem()->createAndOpenFile(ansiPath);
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
						char errorLog[512];
						sprintf(errorLog, "Error: Texture size is not equal %d-%d: %s\n", w, h, ansiPath);
						os::Printer::log(errorLog);

						image->drop();

						image = baseImage;
						baseImage->grab();
					}
				}
			}

			listImage.push_back(image);
		}

		ITexture *texture = NULL;

		if (listImage.size() > 0 && listImage[0] == NULL)
		{
			char errorLog[512];
			sprintf(errorLog, "Can not load texture array: %s\n", paths[0].c_str());
			os::Printer::log(errorLog);
		}
		else
		{
			texture = driver->getTextureArray(listImage.data(), listImage.size());

			// register the texture
			if (texture)
				registerTexture(texture);
			else
			{
				char errorLog[512];
				sprintf(errorLog, "Can not load texture array: %s\n", paths[0].c_str());
				os::Printer::log(errorLog);
			}
		}

		// release
		for (int i = 0, n = (int)listImage.size(); i < n; i++)
		{
			if (listImage[i] != NULL)
				listImage[i]->drop();
		}

		return texture;
	}

	// getCubeTexture
	ITexture* CTextureManager::getCubeTexture(
		const char *pathX1,
		const char *pathX2,
		const char *pathY1,
		const char *pathY2,
		const char *pathZ1,
		const char *pathZ2)
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

		IVideoDriver *driver = getVideoDriver();
		IrrlichtDevice *device = getIrrlichtDevice();

		ITexture *texture = NULL;
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
			sprintf(errorLog, "Can not load texture: %s\n", paths[0].c_str());
			os::Printer::log(errorLog);
		}

		return texture;
	}
}