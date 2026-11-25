// ManagerScreens.h
// Manager-only screens: Products, Inventory, Categories, Seasons, Reports
// and drawScreen(), which picks the screen to draw

#pragma once

#include "Screens.h"

// ---------- Products ----------

const TableLayout productsTable =
{
    260, 55, 820, 28, 11,
    { 270, 315, 395, 445, 530, 720, 780, 850, 910, 980 },
    { "ID", "Barcode", "Cat", "Acronym", "Name", "Unit", "Price", "Tax", "Disc", "Qty" }
};

void drawProducts(RenderWindow& window)
{
    drawText(window, "Products", 260, 15, 26, textColor);
    drawText(window, "Click a product to edit it, or press New Product to add one.",
        390, 24, 14, lightText);

    vector<vector<string>> rows;

    for (size_t i = 0; i < products.size(); i++)
    {
        Product& p = products[i];

        rows.push_back({ to_string(p.id), p.barcode, p.categoryId, p.acronym,
            underscoreToSpaces(p.name), p.unit, money(p.price), money(p.tax),
            money(p.discount), to_string(p.quantity) });
    }

    drawTable(window, productsTable, rows, app.selected);

    bool editing = !app.formNew && app.selected >= 0 && app.selected < (int)products.size();

    string formTitle = editing ? "Edit Product " + products[app.selected].barcode :
        "Add New Product (barcode is made automatically)";

    drawText(window, formTitle, 260, 412, 17, accentColor);

    // Category chooser
    drawText(window, "Category", 260, 438, 14, lightText);
    drawButton(window, prevCategoryButton);
    drawButton(window, nextCategoryButton);

    RectangleShape box(Vector2f(200, 34));
    box.setPosition(298, 460);
    box.setFillColor(cardColor);
    box.setOutlineThickness(2);
    box.setOutlineColor(Color(55, 75, 95));
    window.draw(box);

    string categoryText = "No categories yet";

    if (app.categoryChoice < 0 || app.categoryChoice >= (int)categories.size())
        app.categoryChoice = 0;

    if (!categories.empty())
    {
        Category& c = categories[app.categoryChoice];
        categoryText = c.id + "  " + fitText(underscoreToSpaces(c.name), 16);
    }

    drawText(window, categoryText, 306, 467, 16, textColor);

    drawField(window, pAcronymField, "Acronym");
    drawField(window, pNameField, "Name");
    drawField(window, pUnitField, "Unit");
    drawField(window, pPriceField, "Price");
    drawField(window, pTaxField, "Tax");
    drawField(window, pDiscountField, "Discount");
    drawField(window, pQtyField, "Quantity");

    drawButton(window, newProductButton);
    drawButton(window, saveProductButton, true);
    drawButton(window, imagesButton);

    // Barcode picture of the selected product
    RectangleShape preview(Vector2f(280, 110));
    preview.setPosition(795, 520);
    preview.setFillColor(Color::White);
    window.draw(preview);

    if (app.previewReady)
    {
        Sprite sprite(app.previewTexture);

        float scale = std::min(260.0f / app.previewTexture.getSize().x,
            100.0f / app.previewTexture.getSize().y);

        sprite.setScale(scale, scale);
        sprite.setPosition(805, 525);

        window.draw(sprite);
    }
    else
    {
        drawText(window, "Barcode preview", 875, 565, 15, Color(120, 120, 120));
    }

    drawStatus(window, 260, 650);
}

// ---------- Inventory ----------

const TableLayout inventoryTable =
{
    260, 85, 820, 30, 14,
    { 270, 360, 580, 740, 820, 920 },
    { "Barcode", "Name", "Category", "Unit", "Available", "Status" }
};

void drawInventory(RenderWindow& window)
{
    drawText(window, "Inventory", 260, 15, 26, textColor);
    drawText(window, "Products: " + to_string(products.size()) +
        "     Low stock (" + to_string(LOW_STOCK_LIMIT) + " or less): " +
        to_string(lowStockCount()), 262, 52, 15, lightText);

    vector<vector<string>> rows;

    for (size_t i = 0; i < products.size(); i++)
    {
        Product& p = products[i];

        rows.push_back({ p.barcode, underscoreToSpaces(p.name), categoryName(p.categoryId),
            p.unit, to_string(p.quantity),
            p.quantity <= LOW_STOCK_LIMIT ? "LOW - reorder" : "OK" });
    }

    drawTable(window, inventoryTable, rows, app.selected);

    if (app.selected >= 0 && app.selected < (int)products.size())
    {
        Product& p = products[app.selected];

        drawText(window, "Selected: " + underscoreToSpaces(p.name) +
            "   (available " + to_string(p.quantity) + ")", 260, 552, 16, accentColor);
    }
    else
    {
        drawText(window, "Click a product to update its stock.", 260, 552, 16, lightText);
    }

    drawField(window, invAddField, "Add stock");
    drawButton(window, addStockButton, true);

    drawField(window, invSetField, "New quantity");
    drawButton(window, setStockButton);

    drawStatus(window, 260, 655);
}

// ---------- Categories ----------

const TableLayout categoriesTable =
{
    260, 75, 820, 30, 11,
    { 270, 340, 600, 740 },
    { "ID", "Name", "Acronym", "Products" }
};

void drawCategories(RenderWindow& window)
{
    drawText(window, "Product Categories", 260, 15, 26, textColor);
    drawText(window, "The category ID is the first 2 digits of every barcode in it.",
        262, 50, 14, lightText);

    vector<vector<string>> rows;

    for (size_t i = 0; i < categories.size(); i++)
    {
        Category& c = categories[i];

        rows.push_back({ c.id, underscoreToSpaces(c.name), c.acronym,
            to_string(productsInCategory(c.id)) });
    }

    drawTable(window, categoriesTable, rows, app.selected);

    bool editing = !app.formNew && app.selected >= 0 && app.selected < (int)categories.size();

    drawText(window, editing ? "Edit Category " + categories[app.selected].id +
        " (ID cannot change)" : "Add New Category",
        260, 462, 17, accentColor);

    drawField(window, cIdField, "ID (2 digits)");
    drawField(window, cNameField, "Name");
    drawField(window, cAcronymField, "Acronym");

    drawButton(window, newCategoryButton);
    drawButton(window, saveCategoryButton, true);

    drawStatus(window, 260, 650);
}

// ---------- Seasons ----------

const TableLayout seasonsTable =
{
    260, 75, 820, 30, 11,
    { 270, 320, 450, 580, 700, 800 },
    { "#", "Start", "End", "Discount", "Active", "Running Today" }
};

void drawSeasons(RenderWindow& window)
{
    drawText(window, "Seasonal Discounts", 260, 15, 26, textColor);
    drawText(window, "Discount today: " + money(activeSeasonPercent(currentDate())) +
        " %   (dates as dd-mm-yyyy)", 262, 50, 14, lightText);

    vector<vector<string>> rows;

    for (size_t i = 0; i < seasons.size(); i++)
    {
        Season& s = seasons[i];

        rows.push_back({ to_string(i + 1), s.start, s.end, money(s.percent) + " %",
            s.active ? "Yes" : "No", seasonRunning(s, currentDate()) ? "YES" : "-" });
    }

    drawTable(window, seasonsTable, rows, app.selected);

    drawText(window, app.formNew ? "Add New Season" :
        "Edit Season " + to_string(app.selected + 1), 260, 462, 17, accentColor);

    drawField(window, sStartField, "Start date");
    drawField(window, sEndField, "End date");
    drawField(window, sPercentField, "Discount %");

    drawButton(window, newSeasonButton);
    drawButton(window, saveSeasonButton, true);
    drawButton(window, toggleSeasonButton);

    drawStatus(window, 260, 650);
}

// ---------- Reports ----------

void drawReports(RenderWindow& window)
{
    drawText(window, app.report.title, 260, 15, 22, textColor);

    for (int i = 0; i < (int)reportNames.size(); i++)
        drawButton(window, reportTabButton(i), i == app.reportTab);

    if (app.reportTab == 2)
    {
        for (int i = 0; i < (int)periodNames.size(); i++)
            drawButton(window, periodButton(i), periodNames[i] == app.cashPeriod);

        if (app.cashPeriod == "Custom")
        {
            drawText(window, "From", rFromField.x, 97, 13, lightText);
            drawText(window, "To", rToField.x, 97, 13, lightText);
            drawField(window, rFromField, "");
            drawField(window, rToField, "");
            drawButton(window, applyButton, true);
        }
    }

    TableLayout table =
    {
        260, 165, 820, 28, 14, app.report.columns, app.report.headers
    };

    drawTable(window, table, app.report.rows, -1);

    drawText(window, app.report.summary, 260, 612, 16, accentColor);

    drawStatus(window, 260, 650);
}

// ---------- Pick the screen ----------

void drawScreen(RenderWindow& window)
{
    if (app.screen == "Login")
        drawLogin(window);

    else if (app.screen == "Dashboard")
        drawDashboard(window);

    else if (app.screen == "New Order")
        drawOrderScreen(window);

    else if (app.screen == "Bill")
        drawBill(window);

    else if (app.screen == "Orders")
        drawOrdersScreen(window);

    else if (app.screen == "Approval")
        drawApproval(window);

    else if (app.screen == "Scanner")
        drawScanner(window);

    else if (app.screen == "Products")
        drawProducts(window);

    else if (app.screen == "Inventory")
        drawInventory(window);

    else if (app.screen == "Categories")
        drawCategories(window);

    else if (app.screen == "Seasons")
        drawSeasons(window);

    else if (app.screen == "Reports")
        drawReports(window);
}
