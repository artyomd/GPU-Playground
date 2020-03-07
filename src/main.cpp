#include "application/TestApplication.h"
#include "application/OpenGLApplication.h"
#include "application/VulkanApplication.h"
#include "test/TestTriangle.h"
#include "test/TestClearColor.h"

int main() {
  try {
    application::TestApplication *test_application = new application::VulkanApplication();
    test_application->InitWindow();
    test_application->InitContext();
    test_application->InitImGui();
    test_application->RegisterTest<test::TestClearColor>("Clear Color");
    test_application->RegisterTest<test::TestTriangle>("Triangle");
//        testApplication->registerTest<test::TestSeascapeShader>("Seascape");
//        testApplication->registerTest<test::TestRaymarching>("Raymarching");
//        testApplication->registerTest<test::TestStarNestShader>("Star Nest");
//        testApplication->registerTest<test::TestShapingFunctionShader>("Shader Shaping Function");
//        testApplication->registerTest<test::TestColorShader>("Shader Colors");
//        testApplication->registerTest<test::TestSquaresShader>("Shader Squares");
//        testApplication->registerTest<test::TestTexture2d>("Texture2D");
//        testApplication->registerTest<test::TestSphere>("Sphere");
    test_application->Run();
    test_application->DestroyImGui();
    test_application->DestroyContext();
    test_application->DestroyWindow();

    delete test_application;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}