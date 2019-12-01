#ifndef _CBUILD_CONFIG_H_
#define _CBUILD_CONFIG_H_

#include "pch.h"

class CBuildConfig
{
public:
	static std::string	SaveFolder;
	static std::string	ProfileFolder;
	static std::string	MountFolder;
	static std::string	DataFolder;
    
	static std::string  DeviceID;
    static std::string  OSVersion;
    static std::string  Factory;
    static std::string  Model;

	static std::string	CPID;

	static std::string	APKPath;
	static std::string	ObbPath;

	static std::string	BundleID;

	static std::string	DLCVersion;

	static bool UseZipData;
};

#endif