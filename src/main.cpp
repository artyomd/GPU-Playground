#include <test/TestRaymarching.hpp>
#include <test/TestSeascapeShader.h>
#include <test/TestColorShader.h>
#include <test/TestShapingFunctionShader.h>
#include <test/TestSquaresShader.h>
#include <test/TestStarNestShader.h>
#include <test/TestSphere.h>
#include <test/TestTexture2D.h>
#include <test/TestObj.h>
#include <test/TestCube.h>
#include "application/OpenGlApplication.h"
#include "application/VulkanApplication.h"
#include "test/TestTriangle.h"
#include "test/TestClearColor.h"

int main() {
  try {
    application::VulkanApplication test_application{};
    test_application.InitWindow();
    test_application.InitContext();
    test_application.InitImGui();
    test_application.RegisterTest<test::TestClearColor>("Clear Color");
    test_application.RegisterTest<test::TestTriangle>("Triangle");
    test_application.RegisterTest<test::TestRaymarching>("Reflections");
    test_application.RegisterTest<test::TestSeascapeShader>("Seascape");
    test_application.RegisterTest<test::TestStarNestShader>("Star Nest");
    test_application.RegisterTest<test::TestShapingFunctionShader>("Shader Shaping Function");
    test_application.RegisterTest<test::TestColorShader>("Shader Colors");
    test_application.RegisterTest<test::TestSquaresShader>("Shader Squares");
    test_application.RegisterTest<test::TestCube>("Cube");
    test_application.RegisterTest<test::TestTexture2D>("Texture2D");
    test_application.RegisterTest<test::TestObj>("Obj");
    test_application.RegisterTest<test::TestSphere>("Sphere");
    test_application.Run();
    test_application.DestroyImGui();
    test_application.DestroyContext();
    test_application.DestroyWindow();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}