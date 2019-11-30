#include "pch.h"
#include "CBuildConfig.h"

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

#ifdef _WIN32

#ifdef USE_DATA_IN_ZIP_FILE
bool CBuildConfig::UseZipData = true;
#else
bool CBuildConfig::UseZipData = false;
#endif

#else
bool CBuildConfig::UseZipData = true;
#endif

#ifdef IOS
std::string CBuildConfig::DLCVersion = "v1.1.5i";
#elif ANDROID
std::string	CBuildConfig::DLCVersion = "v1.1.5a";
#endif
