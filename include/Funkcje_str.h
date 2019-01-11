#ifndef STRING_H
#define STRING_H

#ifdef _WIN32

#include <io.h>
#include <dir.h>
#include <direct.h>

#endif // _WIN32
#ifdef __linux__
#include <sys/dir.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#endif // __linux__

#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <SFML/System/Clock.hpp>
#include <vector>

namespace Game_api {

    inline std::string wypisz(std::string &str, const char &znak = ' ') {
        std::string result = "";

        std::size_t found = str.find(znak); //pierwszy znak liczac od 0

        if (found != std::string::npos) {
            result.insert(0, str, 0, found); //na pozycji 0 umieszcza str od 0(liczy od 0) do found(liczy od 1)
            str.erase(0, found + 1);
        } else {
            result = str;
            str.clear();
        }

        return result;
    }

    inline std::string rwypisz(std::string &str, const char &znak = ' ') {
        std::string result = "";

        std::size_t found = str.rfind(znak); //pierwszy znak liczac od 0

        if (found != std::string::npos) {
            result.insert(0, str, found + 1,
                          str.length() - found); //na pozycji 0 umieszcza str od found(liczy od 1) do konca
            str.erase(found, str.length() - found);

        } else {
            result = str;
            str.clear();
        }

        return result;
    }

    inline std::string swypisz(std::string str, const char &znak = ' ') {
        return wypisz(str, znak);
    }

    inline std::string rswypisz(std::string str, const char &znak = ' ') {
        return rwypisz(str, znak);
    }

    std::string delstr(std::string str, const char &znak = ' ', const std::string &del = ".");

    bool has(const std::string &str, const char &znak = ' ');

    std::string convertInt(int32_t number);

    std::string convertFloat(float number);

    std::string shortenFloat(float number, uint32_t ile = 1);

    int32_t convertString(std::string str);

    float convertString_float(std::string str);

    bool isInt(std::string str);

    std::string smallString(const std::string &str);

    namespace plik {
        //std::string dir(std::string gdzie);
        std::vector<std::string> dirD(std::string gdzie);

        std::vector<std::string> dirF(std::string gdzie);

        bool mkdir(std::string jaki);
    }

    namespace binary {
        template<typename T>
        inline T &shift_bit_lewo(T &co, const int8_t &ile) {
            static_assert(std::is_unsigned<T>{}, "T should be unsigned");
            asm volatile( "rol %%cl, %0" : "+r"( co ) : "ecx"( ile ));
            return co;
        }

        template<typename T>
        inline T &shift_bit_prawo(T &co, const int8_t &ile) {
            return shift_bit_lewo(co, -ile);
        }
    }


    template<typename Tag, typename Tag::type Member>
    struct break_in_ {
        friend constexpr typename Tag::type break_in(Tag) {
            return Member;
        }
    };

#define DEFINE_BREAK_IN(name, t, member)\
           struct name {\
             using type = t;\
             friend constexpr t break_in(name);\
           };\
           template struct break_in_<name, member>

    //DEFINE_BREAK_IN(nazwa_odwolamnia, int Klasa::*, &Klasa::x);
    // std::cout << x. *break_in(nazwa_odwolamnia());
}
#endif // STRING_H
