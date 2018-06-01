#include "Common.h"

namespace Testing
{
    TEST_CLASS( RemoteCall )
    {
    public:
        TEST_METHOD( NtQueryVirtualMemory )
        {
            Process process;

            auto path = GetHelperPath();
            Assert::IsFalse( path.empty() );

            // Give process some time to initialize
            AssertEx::NtSuccess( process.CreateAndAttach( path ) );
            Sleep( 100 );

            auto hMainMod = process.modules().GetMainModule();
            auto pRemote = process.modules().GetNtdllExport( "NtQueryVirtualMemory" );

            Assert::IsNotNull( hMainMod.get() );
            Assert::IsTrue( pRemote.success() );

            uint8_t buf[1024] = { 0 };

            RemoteFunction<fnNtQueryVirtualMemory> pFN( process, pRemote->procAddress );
            decltype(pFN)::CallArguments args(
                INVALID_HANDLE_VALUE,
                reinterpret_cast<PVOID>(hMainMod->baseAddress),
                MemorySectionName,
                reinterpret_cast<PVOID>(buf),
                static_cast<SIZE_T>(sizeof( buf )),
                nullptr
            );

            args.set( 3, AsmVariant( buf, sizeof( buf ) ) );
            auto result = pFN.Call( args );

            process.Terminate();

            AssertEx::NtSuccess( result.status );
            AssertEx::NtSuccess( result.result() );

            std::wstring name( reinterpret_cast<wchar_t*>(buf + sizeof( UNICODE_STRING )) );
            Assert::AreNotEqual( name.npos, name.rfind( Utils::StripPath( path ) ) );
        }

    private:
    };

}