#include "Funkcje_str.h"

namespace Game_api {

    std::string delstr(std::string str, const char &znak, const std::string &del) {
        std::string temp;
        std::string temp2;
        std::size_t found;

        while (!str.empty()) {
            found = str.find(znak); //pierwszy znak liczac od 0

            if (found != std::string::npos) {
                temp.clear();
                temp.insert(0, str, 0, found); //na pozycji 0 umieszcza str od 0(liczy od 0) do found(liczy od 1)
                str.erase(0, found + 1);
            } else {
                temp = str;
                str.clear();
            }

            if (temp != del) {
                temp2 += temp;

                if (!str.empty()) { temp2 += znak; }//dodaje znak jezeli nie koniec str
            }
        }

        return temp2;
    }

    bool has(const std::string &str, const char &znak) {
        std::size_t found = str.find(znak);
        return found != std::string::npos;
    }

    std::string convertInt(int32_t number) {
        char str[12] = {0};
        std::sprintf(str, "%i", number);
        return str;
    }

    std::string convertFloat(float number) {
        char str[50] = {0};
        std::sprintf(str, "%f", number);
        return str;
    }

    std::string shortenFloat(float number, uint32_t ile) {
        std::string result;
        result = convertFloat(number);
        result = swypisz(result, '.') + "." + rwypisz(result, '.').erase(ile);

        return result;
    }

    int32_t convertString(std::string str) {
        return atoi(str.c_str());
    }

    float convertString_float(std::string str) {
        return static_cast<float>(atof(str.c_str()));
    }

    bool isInt(std::string str) {
        int32_t result;
        return static_cast<bool>(sscanf(str.c_str(), "%i", &result));
    }

    std::string smallString(const std::string &str) {
        std::string result;
        for (char i : str) {
            result += static_cast<char>(std::tolower(i));
        }
        return result;
    }


    namespace plik {
#ifdef _WIN32

        std::vector<std::string> dirD(std::string gdzie) {
            _finddata_t data{};
            std::vector<std::string> result;

            if (gdzie.back() != '/') { gdzie += "/"; }
            if (gdzie.back() == '/') { gdzie += "*"; }

            intptr_t handle = _findfirst(gdzie.c_str(), &data);
            if (handle == -1) return result;

            if (data.attrib & _A_SUBDIR) { result.emplace_back(data.name); }
            int32_t find = _findnext(handle, &data);
            while (find != -1) {
                if (data.attrib & _A_SUBDIR) {
                    result.emplace_back(data.name);
                }
                find = _findnext(handle, &data);
            }
            _findclose(handle);
            return result;
        }

        std::vector<std::string> dirF(std::string gdzie) {
            _finddata_t data{};
            std::vector<std::string> result;

            if (gdzie.back() != '/') { gdzie += "/"; }
            if (gdzie.back() == '/') { gdzie += "*"; }

            intptr_t handle = _findfirst(gdzie.c_str(), &data);
            if (handle == -1) return result;

            if (!(data.attrib & _A_SUBDIR)) { result.emplace_back(data.name); }
            int32_t find = _findnext(handle, &data);
            while (find != -1) {
                if (!(data.attrib & _A_SUBDIR)) {
                    result.emplace_back(data.name);
                }
                find = _findnext(handle, &data);
            }
            _findclose(handle);
            return result;
        }

        /*
        std::string dir(std::string gdzie)
        {
            _finddata_t data;
            std::string result = "";

            if(gdzie.back() != '/'){gdzie += "/";}
            if(gdzie.back() == '/'){gdzie += "*";}

            long handle = _findfirst(gdzie.c_str(), & data);
            if(handle == -1) return result;

            result += data.name;
            int32_t find = _findnext(handle, & data);
            while(find != -1) {
                result += '\n';
                result += data.name;
                find = _findnext(handle, & data);
            }
            _findclose(handle);
            return result;
        }*/

        bool mkdir(std::string jaki) {
            if (::mkdir(jaki.c_str())) {
                switch (errno) {
                    case EEXIST:
                        return true;
                    case ENOENT:
                        return false;
                    default:
                        return false;
                }
            }
            return true;
        }

#endif // _WIN32
#ifdef __linux__

        std::vector<std::string> dirD(std::string gdzie)
        {
            DIR *dir;
            dirent *drnt;
            std::vector<std::string> result;

            if(gdzie.empty()){gdzie = ".";}

            if( (dir = opendir(gdzie.c_str())) != nullptr) //otwieram folder
            {
                drnt = readdir( dir ); //czytam 1

                while(drnt != nullptr)
                {
                    if(drnt->d_type == DT_DIR){result.emplace_back(drnt->d_name);} //czytam nazwe
                    drnt = readdir( dir );
                }

                closedir(dir); //zamykam folder
            }

            return result;
        }

        std::vector<std::string> dirF(std::string gdzie) {
            DIR *dir;
            dirent *drnt;
            std::vector<std::string> result;

            if (gdzie.empty()) { gdzie = "."; }

            if ((dir = opendir(gdzie.c_str())) != nullptr) //otwieram folder
            {
                drnt = readdir(dir); //czytam 1

                while (drnt != nullptr) {
                    if (drnt->d_type != DT_DIR) { result.emplace_back(drnt->d_name); } //czytam nazwe
                    drnt = readdir(dir);
                }

                closedir(dir); //zamykam folder
            }

            return result;
        }

        bool mkdir(std::string jaki) {
            if (::mkdir(jaki.c_str(), 770)) {
                switch (errno) {
                    case EEXIST:
                        return true;
                    case ENOENT:
                        return false;
                    default:
                        return false;
                }
            }
            return true;
        }

#endif // __linux__
    }


}
