#pragma once
#include "../BlackBone/Config.h"
#include "../BlackBone/Process/Process.h"
#include "../BlackBone/Process/MultPtr.hpp"
#include "../BlackBone/Process/RPC/RemoteFunction.hpp"
#include "../BlackBone/PE/PEImage.h"
#include "../BlackBone/Misc/Utils.h"
#include "../BlackBone/Misc/DynImport.h"
#include "../BlackBone/Syscalls/Syscall.h"
#include "../BlackBone/Patterns/PatternSearch.h"
#include "../BlackBone/Asm/LDasm.h"
#include "../BlackBone/localHook/VTableHook.hpp"

#include <iostream>
#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace blackbone;


/// <summary>
/// Some extension for Assert
/// </summary>
class AssertEx
{
public:
    template <typename T>
    static void IsZero( const T& actual, const wchar_t* message = nullptr, const __LineInfo* pLineInfo = nullptr )
    {
        Assert::AreEqual( T( 0 ), actual, message, pLineInfo );
    }

    template <typename T>
    static void IsNotZero( const T& actual, const wchar_t* message = nullptr, const __LineInfo* pLineInfo = nullptr )
    {
        Assert::AreNotEqual( T( 0 ), actual, message, pLineInfo );
    }

    static void NtSuccess( NTSTATUS status, const wchar_t* message = nullptr, const __LineInfo* pLineInfo = nullptr )
    {
        Assert::IsTrue( status >= 0, message, pLineInfo );
    }
};

inline std::wstring GetHelperPath()
{
#ifdef USE64
    std::wstring exeName = L"TestHelper64.exe";
#else
    std::wstring exeName = L"TestHelper32.exe";
#endif

    wchar_t buf[MAX_PATH] = { };
    GetModuleFileNameW( GetModuleHandle( L"Testing.dll" ), buf, _countof( buf ) );
    std::wstring path = buf;

    // Get project root path
    for (int i = 0; i < 4; i++)
    {
        path = Utils::GetParent( path );
    }

    path += L"\\Testing\\" + exeName;
    if (Utils::FileExists( path ))
    {
        return path;
    }

    return std::wstring();
}
