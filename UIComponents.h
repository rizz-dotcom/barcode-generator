// UIComponents.h
// App state, buttons, text fields and basic drawing helpers

#pragma once

#include "BarcodeScanner.h"
#include "OrderManager.h"

struct AppState
{
    string screen = "Dashboard";
    string status = "";
    bool generated = false;
    Texture barcodeTexture;
    bool showDescription = false;
    vector<OrderItem> items;
    Order order;
    string billId = "";
    bool billSaved = false;
    int selectedOrder = -1;
};

AppState app;
CameraScanner scanner;

const vector<Button> sidebarButtons =
{
    {"Dashboard", "Dashboard", 25, 140, 190, 46},
    {"Generate Barcode", "Generate Barcode", 25, 200, 190, 46},
    {"Barcode Scanner", "Barcode Scanner", 25, 260, 190, 46},
    {"New Order", "New Order", 25, 320, 190, 46},
    {"Products", "Products", 25, 380, 190, 46},
    {"Order History", "Order History", 25, 440, 190, 46},
    {"Exit", "Exit", 25, 500, 190, 46}
};

const vector<Button> quickButtons =
{
    {"Generate Barcode", "Generate Barcode", 280, 350, 230, 65},
    {"Scan Barcode", "Barcode Scanner", 540, 350, 230, 65},
    {"New Order", "New Order", 800, 350, 230, 65},
    {"View Products", "Products", 280, 445, 230, 65},
    {"Order History", "Order History", 540, 445, 230, 65},
    {"Exit", "Exit", 800, 445, 230, 65}
};

const vector<Button> paymentButtons =
{
    {"Cash", "cash", 290, 478, 100, 36},
    {"Card", "card", 400, 478, 100, 36},
    {"Wallet", "wallet", 510, 478, 100, 36}
};

const Button generateButton = { "Generate Barcode", "", 320, 255, 200, 55 };

const Button scannerStartButton = { "Start Camera", "", 270, 500, 150, 40 };
const Button scannerStopButton = { "Stop Camera", "", 435, 500, 150, 40 };
const Button scannerAgainButton = { "Scan Again", "", 600, 500, 150, 40 };

const Button orderStartButton = { "Start Camera", "", 260, 445, 120, 36 };
const Button orderStopButton = { "Stop Camera", "", 390, 445, 120, 36 };
const Button orderDescriptionButton = { "Description", "", 520, 445, 120, 36 };
const Button addButton = { "Add Product", "", 370, 590, 140, 36 };
const Button removeButton = { "Remove Last", "", 670, 600, 120, 38 };
const Button holdButton = { "Hold Order", "", 800, 600, 120, 38 };
const Button billButton = { "Generate Bill", "", 930, 600, 150, 38 };

const Button confirmButton = { "Confirm Payment", "", 290, 595, 170, 40 };
const Button billBackButton = { "Back", "", 475, 595, 120, 40 };

const Button barcodeImagesButton = { "Generate Barcode Images", "", 280, 625, 260, 42 };
const Button archiveButton = { "Archive Selected", "", 280, 610, 200, 42 };

Field numberField = { 320, 170, 400, 55, "", 6, true };
Field nameField = { 260, 95, 185, 36, "", 20, false };
Field contactField = { 460, 95, 180, 36, "", 12, true };
Field quantityField = { 260, 590, 90, 36, "1", 3, true };
Field merchantField = { 290, 555, 320, 36, "", 15, false };

Field* activeField = NULL;

bool clicked(float x, float y, float w, float h, Vector2f mouse)
{
    return mouse.x >= x &&
        mouse.x <= x + w &&
        mouse.y >= y &&
        mouse.y <= y + h;
}

bool clicked(const Button& button, Vector2f mouse)
{
    return clicked(button.x, button.y, button.w, button.h, mouse);
}

string findClickedTarget(const vector<Button>& buttons, Vector2f mouse)
{
    for (int i = 0; i < buttons.size(); i++)
    {
        if (clicked(buttons[i], mouse))
            return buttons[i].target;
    }

    return "";
}

void selectField(Field& field, Vector2f mouse)
{
    if (clicked(field.x, field.y, field.w, field.h, mouse))
        activeField = &field;
}

void typeInto(Field& field, Uint32 unicode)
{
    if (unicode == 8)
    {
        if (!field.text.empty())
            field.text.pop_back();

        return;
    }

    if ((int)field.text.length() >= field.limit)
        return;

    if (field.numbersOnly)
    {
        if (unicode >= '0' && unicode <= '9')
            field.text += static_cast<char>(unicode);
    }
    else if (unicode >= 32 && unicode <= 126 && unicode != '#')
    {
        field.text += static_cast<char>(unicode);
    }
}

void drawText(RenderWindow& window, string value,
    float x, float y, int size, Color color)
{
    Text text;

    text.setFont(font);
    text.setString(value);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);

    window.draw(text);
}

void drawLines(RenderWindow& window, const vector<string>& lines,
    float x, float y, int size, float step, Color color)
{
    for (int i = 0; i < lines.size(); i++)
        drawText(window, lines[i], x, y + i * step, size, color);
}

void drawButton(RenderWindow& window, const Button& button,
    bool highlight = false)
{
    Vector2i position = Mouse::getPosition(window);
    bool hover = clicked(button, Vector2f(position));

    RectangleShape box(Vector2f(button.w, button.h));

    box.setPosition(button.x, button.y);
    box.setOutlineThickness(1);
    box.setOutlineColor(Color(55, 75, 95));

    Color labelColor = textColor;

    if (highlight)
    {
        box.setFillColor(hover ? Color(40, 215, 195) : accentColor);
        labelColor = Color(12, 20, 30);
    }
    else
    {
        box.setFillColor(hover ? Color(55, 80, 105) : buttonColor);
    }

    window.draw(box);

    Text label;

    label.setFont(font);
    label.setString(button.title);
    label.setCharacterSize(16);
    label.setFillColor(labelColor);

    FloatRect bounds = label.getLocalBounds();

    label.setOrigin(
        bounds.left + bounds.width / 2,
        bounds.top + bounds.height / 2
    );

    label.setPosition(button.x + button.w / 2, button.y + button.h / 2);

    window.draw(label);
}

void drawPanel(RenderWindow& window, float x, float y, float w, float h)
{
    RectangleShape panel(Vector2f(w, h));

    panel.setPosition(x, y);
    panel.setFillColor(cardColor);
    panel.setOutlineThickness(1);
    panel.setOutlineColor(Color(50, 68, 88));

    window.draw(panel);
}

void drawCard(RenderWindow& window, float x, float y,
    float w, float h, string title, string value)
{
    drawPanel(window, x, y, w, h);

    drawText(window, title, x + 20, y + 18, 16, lightText);
    drawText(window, value, x + 20, y + 55, 27, accentColor);
}

void drawLine(RenderWindow& window, float x, float y, float w)
{
    RectangleShape line(Vector2f(w, 2));

    line.setPosition(x, y);
    line.setFillColor(Color(55, 75, 95));

    window.draw(line);
}

void drawField(RenderWindow& window, const Field& field, string label)
{
    bool active = (activeField == &field);

    drawText(window, label, field.x, field.y - 22, 14, lightText);

    RectangleShape box(Vector2f(field.w, field.h));

    box.setPosition(field.x, field.y);
    box.setFillColor(cardColor);
    box.setOutlineThickness(2);
    box.setOutlineColor(active ? accentColor : Color(55, 75, 95));

    window.draw(box);

    string shown = field.text;

    if (active)
        shown += "|";

    int size = (field.h > 50) ? 23 : 17;

    drawText(window, shown, field.x + 10, field.y + field.h / 2 - size * 0.7f,
        size, textColor);
}

void drawCamera(RenderWindow& window, float x, float y, float w, float h)
{
    RectangleShape box(Vector2f(w, h));

    box.setPosition(x, y);
    box.setFillColor(Color::Black);
    box.setOutlineThickness(1);
    box.setOutlineColor(Color(50, 68, 88));

    window.draw(box);

    if (scanner.ready)
    {
        Sprite sprite(scanner.texture);

        float scaleX = w / scanner.texture.getSize().x;
        float scaleY = h / scanner.texture.getSize().y;

        float scale = std::min(scaleX, scaleY);

        float drawWidth = scanner.texture.getSize().x * scale;
        float drawHeight = scanner.texture.getSize().y * scale;

        sprite.setScale(scale, scale);
        sprite.setPosition(x + (w - drawWidth) / 2, y + (h - drawHeight) / 2);

        window.draw(sprite);
    }
    else
    {
        drawText(window, "Camera is off", x + w / 2 - 55, y + h / 2 - 12,
            18, lightText);
    }
}

void drawSidebar(RenderWindow& window)
{
    RectangleShape sidebar(Vector2f(240, HEIGHT));

    sidebar.setFillColor(sidebarColor);

    window.draw(sidebar);

    drawText(window, "SMART POS", 30, 30, 27, accentColor);
    drawText(window, "BARCODE SYSTEM", 31, 67, 12, lightText);

    drawLine(window, 25, 110, 190);

    for (int i = 0; i < sidebarButtons.size(); i++)
        drawButton(window, sidebarButtons[i],
            sidebarButtons[i].target == app.screen);
}
