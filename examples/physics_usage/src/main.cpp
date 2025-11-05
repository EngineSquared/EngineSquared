/**************************************************************************
 * EngineSquared - Physics Usage Example
 *
 * Main entry point - Select and run physics examples
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to create a simple physics simulation with falling cubes.
 **************************************************************************/

#include "Engine.hpp"
#include "Physics.hpp"
#include "Object.hpp"

#include "examples/FallingObjectsExample.hpp"
#include "examples/ForcesImpulsesExample.hpp"

#include <iostream>
#include <string>

void PrintHeader()
{
    std::cout << "========================================" << std::endl;
    std::cout << "  EngineSquared - Physics Examples" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void PrintMenu()
{
    std::cout << "Available Examples:" << std::endl;
    std::cout << "  1. Falling Objects Example (Original)" << std::endl;
    std::cout << "     - Static floor, dynamic cubes, bouncy ball" << std::endl;
    std::cout << "     - Demonstrates basic physics simulation\n" << std::endl;

    std::cout << "  2. Forces & Impulses Example (Issue #001)" << std::endl;
    std::cout << "     - AddForce, AddTorque, AddImpulse, etc." << std::endl;
    std::cout << "     - Demonstrates Force Applicator API\n" << std::endl;

    std::cout << "  3. Run All Examples" << std::endl;
    std::cout << "     - Execute all examples sequentially\n" << std::endl;

    std::cout << "  0. Exit\n" << std::endl;
}

int GetUserChoice()
{
    std::cout << "Select example (0-3): ";
    std::string input;
    std::getline(std::cin, input);

    try {
        return std::stoi(input);
    } catch (...) {
        return -1;
    }
}

void RunExample(int choice)
{
    Engine::Core core;

    core.AddPlugins<Physics::Plugin>();

    switch (choice) {
    case 1:
        std::cout << "\n→ Running Falling Objects Example...\n" << std::endl;
        core.AddPlugins<Examples::FallingObjectsExample>();
        break;

    case 2:
        std::cout << "\n→ Running Forces & Impulses Example...\n" << std::endl;
        core.AddPlugins<Examples::ForcesImpulsesExample>();
        break;

    case 3:
        std::cout << "\n→ Running All Examples...\n" << std::endl;
        core.AddPlugins<Examples::FallingObjectsExample>();
        core.AddPlugins<Examples::ForcesImpulsesExample>();
        break;

    default:
        std::cout << "Invalid choice!" << std::endl;
        return;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  Starting Simulation" << std::endl;
    std::cout << "========================================" << std::endl;

    core.RunCore();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  Simulation Complete!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

int main(int argc, char** argv)
{
    PrintHeader();

    // Check for command line argument
    if (argc > 1) {
        std::string arg = argv[1];

        if (arg == "1" || arg == "falling") {
            RunExample(1);
        } else if (arg == "2" || arg == "forces") {
            RunExample(2);
        } else if (arg == "3" || arg == "all") {
            RunExample(3);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [option]" << std::endl;
            std::cout << "\nOptions:" << std::endl;
            std::cout << "  1, falling  : Run Falling Objects Example" << std::endl;
            std::cout << "  2, forces   : Run Forces & Impulses Example" << std::endl;
            std::cout << "  3, all      : Run All Examples" << std::endl;
            std::cout << "  --help, -h  : Show this help\n" << std::endl;
            std::cout << "If no option is provided, interactive menu will be shown." << std::endl;
        } else {
            std::cout << "Unknown option: " << arg << std::endl;
            std::cout << "Use --help for usage information." << std::endl;
            return 1;
        }
        return 0;
    }

    // Interactive menu
    while (true) {
        PrintMenu();
        int choice = GetUserChoice();

        if (choice == 0) {
            std::cout << "\nExiting..." << std::endl;
            break;
        }

        if (choice < 0 || choice > 3) {
            std::cout << "\nInvalid choice! Please select 0-3.\n" << std::endl;
            continue;
        }

        RunExample(choice);

        std::cout << "Press Enter to continue...";
        std::cin.get();
        std::cout << "\n";
    }

    return 0;
}
