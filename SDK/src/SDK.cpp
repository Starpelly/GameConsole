#include "SDK.hpp"

QString SDK::GetProjectPath()
{
    return "D:/Soulcast/test/Sandbox";
}

QString SDK::GetProjectDataPath()
{
    return GetProjectPath() + "/Data/";
}
