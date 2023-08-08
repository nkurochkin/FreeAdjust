#include <windows.h>
#include <string>

bool get_temp_file_name(std::string &rv,char *file)
{
    char path[512];
    if(!GetTempPathA(512,path))
        return false;
    rv=path;
    rv+=file;
    return true;
}
