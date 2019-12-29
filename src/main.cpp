#include <GL/glew.h>
#include "application/TestApplication.h"
#include "application/OpenGLApplication.h"
#include "test/TestSeascapeShader.hpp"
#include "test/TestRaymarching.hpp"
#include "test/TestStarNestShader.hpp"
#include "TestShapingFunctionShader.hpp"
#include "TestClearColor.h"
#include "TestColorShader.hpp"
#include "TestSquaresShader.h"
#include "TestTriangle.h"
#include "TestSphere.h"
#include "TestTexture2d.h"
#include "VulkanApplication.h"

class Main {
public:
    void run() {
        application::TestApplication *testApplication = new application::VulkanApplication();

        testApplication->initWindow();
        testApplication->initContext();
        testApplication->initImGui();
        testApplication->registerTest<test::TestClearColor>("Clear Color");
//        testApplication->registerTest<test::TestSeascapeShader>("Seascape");
//        testApplication->registerTest<test::TestRaymarching>("Raymarching");
//        testApplication->registerTest<test::TestStarNestShader>("Star Nest");
//        testApplication->registerTest<test::TestShapingFunctionShader>("Shader Shaping Function");
//        testApplication->registerTest<test::TestColorShader>("Shader Colors");
//        testApplication->registerTest<test::TestSquaresShader>("Shader Squares");
//        testApplication->registerTest<test::TestTriangle>("Triangle");
//        testApplication->registerTest<test::TestTexture2d>("Texture2D");
//        testApplication->registerTest<test::TestSphere>("Sphere");
        testApplication->run();
        testApplication->destroyImGui();
        testApplication->destroyContext();
        testApplication->destroyWindow();

        delete testApplication;
    }
};

int main() {
    Main main;
    try {
        main.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}