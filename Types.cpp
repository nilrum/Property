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
    std::fopen_s(&f, path.c_str(), (isRead) ? "rb" : "wb");
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