//
// Created by user on 14.09.2020.
//

#include "Types.h"
#ifdef _WIN32
#include <Windows.h>
#endif

std::shared_ptr<FILE> OpenFile(const std::string& path, bool isRead)
{
    FILE* f = nullptr;
#ifndef _WIN32
    f = std::fopen(path.c_str(), (isRead) ? "rb" : "wb");
#else
    std::wstring pathW;
    pathW.resize(path.size());

    int newSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), nullptr, 0);
    pathW.resize(newSize);
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), pathW.data(), path.size());
    _wfopen_s(&f, pathW.c_str(), (isRead) ? L"rb" : L"wb");
#endif
    if(f != nullptr)
        return std::shared_ptr<FILE>(f, [](FILE* file){ if(file != nullptr) std::fclose(file); });
    else
        return std::shared_ptr<FILE>();
}

std::shared_ptr<FILE> OpenFile(const std::string& path, TOpenFileMode mode)
{
    FILE* f = nullptr;
#ifndef _WIN32
    f = std::fopen(path.c_str(), mode == TOpenFileMode::Read ? "rb" :( mode == TOpenFileMode::Write ? "wb" : "ab" ));
#else
    std::wstring pathW;
    pathW.resize(path.size());

    int newSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), nullptr, 0);
    pathW.resize(newSize);
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), pathW.data(), path.size());
    _wfopen_s(&f, pathW.c_str(), mode == TOpenFileMode::Read ? L"rb" :( mode == TOpenFileMode::Write ? L"wb" : L"ab" ));
#endif
    if(f != nullptr)
        return std::shared_ptr<FILE>(f, [](FILE* file)
            {
                if(file != nullptr)
                    std::fclose(file);
            });
    else
        return std::shared_ptr<FILE>();
}

void TSimpleLog::Clear()
{
    auto file = OpenFile(DefaultPath(), TOpenFileMode::Write);
    std::fwrite("Cleared", sizeof(char), 7, file.get());
    std::fwrite("\n", sizeof(char), 1, file.get());
}

void TSimpleLog::Log(const TString &text)
{
    auto file = OpenFile(DefaultPath(), TOpenFileMode::Append);
    std::fseek(file.get(), 0, SEEK_END);
    std::fwrite(text.c_str(), sizeof(char), text.length(), file.get());
    std::fwrite("\n", sizeof(char), 1, file.get());
}



static const char* trans[] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "I", "K", "L", "M", "N", "O",
                              "P", "R", "S", "T", "U", "F", "H", "TS", "CH", "SH", "SCH", "", "Y", "", "E", "YU", "YA",
                              "a", "b", "v", "g", "d", "e", "zh", "z", "i", "i", "k", "l", "m", "n", "o",
                              "p", "r", "s", "t", "u", "f", "h", "ts", "ch", "sh", "sch", "", "y", "", "e", "yu", "ya",
};

TString Transliteration(TString utf8text)
{
    TString res;
    for(auto it = utf8text.begin(); it != utf8text.end(); it++)
    {
        unsigned char a = *it;
        if (a < 0xC0)//символ не русский
        {
            res.push_back(a);
        }
        else
        {
            unsigned short unic = (a & 0x3F) << 6;
            it++;
            a = *it;
            unic = unic | (0x7F & a);//получили символ в юникоде
            if(unic >= 0x410 && unic <= 0x044F)//А = 0x0410 - Я = 0x042F, а = 0x0430 - я = 0x044F
                res.insert(res.length(), trans[unic - 0x410]);
            else if(unic == 0x401) res.insert(res.length(), "EO");//Ё буква отдельно
            else if(unic == 0x451) res.insert(res.length(), "eo");//ё
        }
    }
    return res;
}

TString Merge(const TVecString& values, typename TString::value_type delim)
{
    TString res;
    for(const auto& r : values)
    {
        if(res.empty() == false) res.push_back(delim);
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}