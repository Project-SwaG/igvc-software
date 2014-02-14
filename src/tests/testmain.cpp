#include <QTest>
#include "testangleutils.h"
#include "testgpsreader.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include "teststringutils.hpp"
#include "testpositiontracker.hpp"
#include "testgpsutils.h"
#include "capturegpsandpostracker.hpp"

void RunTestCase(QObject* testCase, std::vector<std::string> args, bool runByDefault = true)
{
    if(runByDefault && ( args.size() == 0 || std::find(args.begin(), args.end(), testCase->metaObject()->className()) != args.end() ))
    {
        QTest::qExec(testCase);
    }
    else if(std::find(args.begin(), args.end(), testCase->metaObject()->className()) != args.end())
    {
        QTest::qExec(testCase);
    }
}

int main(int argc, const char* argv[])
{
    // Setup arguments vector
    std::vector<std::string> args;
    for(int i = 0; i < argc; i++)
        if(argv[i][0] != '/')
            args.push_back(argv[i]);
    if(std::find(args.begin(), args.end(), "all") != args.end())
        args.clear();
    if(args.size() == 0)
    {
        std::cout << "Running all tests." << std::endl;
    }
    else
    {
        std::cout << "Running ";
        for(uint i = 0; i < args.size() - 1; i++)
            std::cout << args[i] << ", ";
        std::cout << (args.size() > 1 ? "and " : "") << args[args.size()-1] << "." << std::endl;
    }

    // Execute selected test cases
    RunTestCase(new TestStringUtils(), args);
    RunTestCase(new TestPositionTracker(), args);
    RunTestCase(new TestGPSUtils(), args);
    RunTestCase(new TestAngleUtils(), args);
    RunTestCase(new TestGPSReader(), args);
    RunTestCase(new CaptureGPSAndPosTracker(), args, false);
}
