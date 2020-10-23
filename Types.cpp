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
        return std::shared_ptr<FILE>(f, [](FILE* file){ if(file != nullptr) std::fclose(file); });
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
