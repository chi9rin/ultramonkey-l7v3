#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>

#define BASEPATH "/proc/"

bool    findProcess(std::string &processname)
{
        bool retflg = false;

        DIR *dp = opendir(BASEPATH);
        if (!dp) {
                return false; //don't open /proc
        }

        struct dirent *dent = readdir(dp);
        while (dent) {
                if (dent->d_type == DT_DIR) {
                        std::string dirname = dent->d_name;
                        if (0x2F < dent->d_name[0] && 0x3A > dent->d_name[0]) {
                                std::string filename = BASEPATH;
                                filename += dent->d_name;
                                filename += "/status";
                                std::string strbuff;
                                std::ifstream ifs(filename.c_str());
                                std::getline(ifs, strbuff);
                                if (strbuff.find(processname) != std::string::npos) {
                                        retflg = true;
                                        break;
                                }
                        }
                }
                dent = readdir(dp);
        }
        closedir(dp);
        return retflg;
}
