//
// Created by artyomd on 12/7/19.
//

#include <vendor/imgui/imgui.h>
#include "TestApplication.h"
#include "test/TestMenu.h"

namespace application {
    TestApplication::TestApplication() {
        testMenu = new test::TestMenu(currentTest);
        currentTest = testMenu;
    }

    void TestApplication::renderMenu() {
        if (currentTest != testMenu && ImGui::Button("<--")) {
            delete currentTest;
            currentTest = testMenu;
        }
    }

    TestApplication::~TestApplication() {
        delete currentTest;
        if (currentTest != testMenu) {
            delete testMenu;
        }
    }
}