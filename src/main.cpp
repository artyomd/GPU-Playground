#include "application/application.hpp"
#include "renderables/menu/default_menu.hpp"
#include "renderables/menu/menu_manager.hpp"
#include "renderables/model/cube.hpp"
#include "renderables/model/gltf_model.hpp"
#include "renderables/model/lighting.hpp"
#include "renderables/model/obj_model.hpp"
#include "renderables/model/spiral_sphere.hpp"
#include "renderables/model/stacked_sphere.hpp"
#include "renderables/model/texture_2d.hpp"
#include "renderables/model/triangle.hpp"
#include "renderables/shader.hpp"

#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>

#ifdef TRACY_ENABLE
void *operator new(std::size_t count) {
  auto ptr = malloc(count);
  TracyAlloc(ptr, count);
  return ptr;
}

void operator delete(void *ptr) noexcept {
  TracyFree(ptr);
  free(ptr);
}
#endif

int main() {
  try {
    spdlog::set_level(spdlog::level::debug);
    application::Application test_application([&](auto context) {
      auto menu = renderable::MenuManager::Create(context, [](auto context, auto parent) {
        return renderable::DefaultMenu::Create(context, parent);
      }, [&]() {
        test_application.RequestExit();
      });

      menu->RegisterMenuItem([](auto context, auto parent) {
        auto models_menu = renderable::MenuManager::Create(context, [](auto context, auto parent) {
          return renderable::DefaultMenu::Create(context, parent);
        }, parent);
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Triangle::Create(context, parent);
        }, "Triangle");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Cube::Create(context, parent);
        }, "Cube");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Texture2d::Create(context, parent);
        }, "Texture2d");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::SpiralSphere::Create(context, parent);
        }, "Spiral Sphere");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::StackedSphere::Create(context, parent);
        }, "Stacked Sphere");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::ObjModel::Create(context, parent);
        }, "Obj");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Lighting::Create(context, parent);
        }, "Lighting");
        models_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::GltfModel::Create(context, parent);
        }, "GLTF");
        return models_menu;
      }, "Models");

      menu->RegisterMenuItem([](auto context, auto parent) {
        auto shader_menu = renderable::MenuManager::Create(context, [](auto context, auto parent) {
          return renderable::DefaultMenu::Create(context, parent);
        }, parent);
        shader_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Shader::Create(context,
                                            parent,
                                            {
#include "shaping_function_fragment_shader.spv"
              });
        }, "Shader Shaping Function");
        shader_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Shader::Create(context,
                                            parent,
                                            {
#include "color_fragment_shader.spv"
              });
        }, "Shader Colors");
        shader_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Shader::Create(context,
                                            parent,
                                            {
#include "squares_fragment_shader.spv"
              });
        }, "Shader Squares");
        shader_menu->RegisterMenuItem([](auto context, auto parent) {
          return renderable::Shader::Create(context,
                                            parent,
                                            {
#include "hsb_fragment.spv"
              });
        }, "HSB");
        return shader_menu;
      }, "Shaders");
      return menu;
    });
    test_application.Run();
  } catch (const std::exception &e) {
    spdlog::error(e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
