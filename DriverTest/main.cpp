/*
 * This example was used for putting items into the action in Lineage 2 MMORPG.
 */

#include <iostream>
#include <string>

#include "mouse.h"
#include "keyboard.h"

int main()
{
    Mouse mouse;
    try {
        mouse.initialize();
    } catch (const std::runtime_error &e) {
        std::cout << std::string("Mouse initialization: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Keyboard keyboard;
    try {
        keyboard.initialize();
    } catch (const std::runtime_error &e) {
        std::cout << std::string("Keyboard initialization: ") + e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Sleep(5000);

    //mouse.moveCursor(136, 271);
    mouse.moveCursorEx(50,0);
    mouse.leftButtonClick();
    Sleep(500);
    //mouse.moveCursor(56, 315);
    mouse.moveCursorEx(50, 0);
    mouse.leftButtonClick();
    Sleep(500);
    //mouse.moveCursor(283, 350);
    mouse.moveCursorEx(50, 0);
    mouse.leftButtonClick();
    Sleep(500);
    //mouse.moveCursor(275, 271);
    mouse.moveCursorEx(50, 0);
    mouse.leftButtonClick();
    Sleep(500);

    return EXIT_SUCCESS;
}
