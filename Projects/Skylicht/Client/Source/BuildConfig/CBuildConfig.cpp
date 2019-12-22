/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CBuildConfig.h"

namespace Skylicht
{

	std::string	CBuildConfig::SaveFolder = "";
	std::string	CBuildConfig::ProfileFolder = "";
	std::string	CBuildConfig::MountFolder = "";
	std::string	CBuildConfig::DataFolder = "";

	std::string CBuildConfig::DeviceID = "";

#ifdef WIN32
	std::string CBuildConfig::OSVersion = "WINDOWS PC";
	std::string CBuildConfig::Factory = "DESKTOP PC";
	std::string CBuildConfig::Model = "DESKTOP PC";
#else
	std::string CBuildConfig::OSVersion = "";
	std::string CBuildConfig::Factory = "";
	std::string CBuildConfig::Model = "";
#endif

	std::string CBuildConfig::CPID = "0";

	std::string CBuildConfig::APKPath = "";
	std::string	CBuildConfig::ObbPath = "";

	std::string	CBuildConfig::BundleID = "";

#ifdef USE_DATA_IN_ZIP_FILE
	bool CBuildConfig::UseZipData = true;
#else
	bool CBuildConfig::UseZipData = false;
#endif

#ifdef IOS
	std::string CBuildConfig::DLCVersion = "v0.0.1i";
#elif ANDROID
	std::string	CBuildConfig::DLCVersion = "v0.0.1a";
#endif

}