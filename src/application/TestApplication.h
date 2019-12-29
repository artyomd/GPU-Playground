//
// Created by artyomd on 12/7/19.
//
#pragma once

#include "test/Test.h"
#include <iostream>
#include <test/TestMenu.h>
#include "Application.h"

namespace application {
    class TestApplication : public Application {
    private:
        test::TestMenu *testMenu = nullptr;
    protected:
        test::Test *currentTest = nullptr;

        virtual void renderMenu() final;

    public:
        TestApplication();

        virtual ~TestApplication();

        template<typename T>
        void registerTest(const std::string &name) {
            testMenu->RegisterTest<T>(name);
        }
    };
}