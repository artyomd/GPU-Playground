#include <spdlog/spdlog.h>

#ifndef __APPLE__
#include "src/application/opengl_application.hpp"
#endif
#include "src/application/vulkan_application.hpp"
#include "src/shaders/shaders.hpp"
#include "src/test/test_color_shader.hpp"
#include "src/test/test_cube.hpp"
#include "src/test/test_gltf.hpp"
#include "src/test/test_light.hpp"
#include "src/test/test_obj.hpp"
#include "src/test/test_raymarching.hpp"
#include "src/test/test_seascape_shader.hpp"
#include "src/test/test_shaping_function_shader.hpp"
#include "src/test/test_sphere.hpp"
#include "src/test/test_squares_shader.hpp"
#include "src/test/test_star_nest_shader.hpp"
#include "src/test/test_texture_2d.hpp"
#include "src/test/test_triangle.hpp"

int main() {
  try {
    spdlog::set_level(spdlog::level::debug);
    LoadShaders();
    application::OpenGlApplication test_application;
    test_application.RegisterTest<test::TestTriangle>("Triangle");
    test_application.RegisterTest<test::TestRaymarching>("Reflections");
    test_application.RegisterTest<test::TestSeascapeShader>("Seascape");
    test_application.RegisterTest<test::TestStarNestShader>("Star Nest");
    test_application.RegisterTest<test::TestShapingFunctionShader>(
        "Shader Shaping Function");
    test_application.RegisterTest<test::TestColorShader>("Shader Colors");
    test_application.RegisterTest<test::TestSquaresShader>("Shader Squares");
    test_application.RegisterTest<test::TestCube>("Cube");
    test_application.RegisterTest<test::TestTexture2D>("Texture2D");
    test_application.RegisterTest<test::TestObj>("Obj");
    test_application.RegisterTest<test::TestLight>("Light");
    test_application.RegisterTest<test::TestSphere>("Sphere");
    test_application.RegisterTest<test::TestGltf>("gltf");
    test_application.Run();
  } catch (const std::exception &e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
