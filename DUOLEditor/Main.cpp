#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <Windows.h>
#include <cstdlib>

#include "DUOLCommon/LogHelper.h"
#include "DUOLEditor/Application.h"

#pragma region MIN_MAX_UNDEF
	#define NOMINMAX
		#ifdef max
		#undef max
	#endif
		#ifdef min
		#undef min
	#endif
#pragma endregion

//// RTTR 연습을 위함
#include <iostream>
#include <rttr/registration>
using namespace rttr;

// ImGui 연습을 위함
#pragma region TEST_DIRECTX_LIB
	#pragma comment(lib, "d3d11.lib")
	#pragma comment(lib, "d3dcompiler.lib")
	#pragma comment(lib, "dxgi.lib")
#pragma endregion

struct RTTRTest
{
    RTTRTest() {};

    void TestFunction(double f)
    {
        DUOL_ENGINE_TRACE("{0} number.");
    }

    int data;
};

RTTR_REGISTRATION
{
    rttr::registration::class_<RTTRTest>("RTTRTest")
    .constructor<>()
    .method("TestFunction", &RTTRTest::TestFunction)
    .property("data", &RTTRTest::data);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    const DUOLEditor::Application app;

    DUOL_ENGINE_TRACE("Editor application initialize.");

    DUOL_ENGINE_TRACE("START DUOL EDITOR !!");

    type t = type::get_by_name("RTTRTest");

    for (auto& prop : t.get_properties())
        std::cout << "name: " << prop.get_name() << std::endl;

    for (auto& meth : t.get_methods())
        std::cout << "name: " << meth.get_name() << std::endl;

    std::cout << t.get_name() << std::endl;

    app.Run();

    return EXIT_SUCCESS;
}