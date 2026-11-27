// main.cpp
// Program start: opens the window and runs the main loop

#include "Config.h"
#include "Models.h"
#include "Utils.h"
#include "ProductManager.h"
#include "BarcodeGenerator.h"
#include "BarcodeScanner.h"
#include "OrderManager.h"
#include "UIComponents.h"
#include "Screens.h"
#include "EventHandlers.h"

int main()
{
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf") &&
        !font.loadFromFile("arial.ttf"))
    {
        cout << "Unable to load font!\n";
        return 0;
    }

    if (!loadProducts())
        cout << "Unable to open Product.txt!\n";

    loadOrders();

    RenderWindow window(
        VideoMode(WIDTH, HEIGHT),
        "Smart POS - Barcode Management System",
        Style::Titlebar | Style::Close
    );

    window.setFramerateLimit(60);

    string previousScreen = "";

    while (window.isOpen())
    {
        Event event;

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::TextEntered && activeField != NULL)
                typeInto(*activeField, event.text.unicode);

            if (event.type == Event::MouseButtonPressed &&
                event.mouseButton.button == Mouse::Left)
            {
                handleClick(Vector2f(
                    (float)event.mouseButton.x,
                    (float)event.mouseButton.y));
            }
        }

        if (app.screen == "Exit")
        {
            window.close();
            break;
        }

        if (app.screen != previousScreen)
        {
            onScreenChange();
            previousScreen = app.screen;
        }

        scanner.update();

        window.clear(background);

        drawSidebar(window);
        drawScreen(window);

        window.display();
    }

    scanner.stop();

    return 0;
}
