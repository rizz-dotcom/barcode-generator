// Screens.h
// Drawing of each screen (Dashboard, Scanner, New Order, Bill, ...)

#pragma once

#include "UIComponents.h"

void drawDashboard(RenderWindow& window)
{
    drawText(window, "Dashboard", 280, 35, 32, textColor);

    drawText(window,
        "Welcome to your Point of Sale System",
        282, 82, 15, lightText);

    drawCard(window, 280, 140, 235, 115,
        "Total Products", to_string(products.size()));

    drawCard(window, 540, 140, 235, 115,
        "Total Orders", to_string(orders.size()));

    drawCard(window, 800, 140, 235, 115,
        "Total Sales", "PKR " + money(totalSales()));

    drawText(window, "Quick Actions", 280, 300, 23, textColor);

    for (int i = 0; i < quickButtons.size(); i++)
        drawButton(window, quickButtons[i]);
}

void drawGenerator(RenderWindow& window)
{
    drawText(window, "Generate Barcode", 280, 35, 32, textColor);

    drawText(window,
        "Enter a six-digit product number",
        282, 90, 16, lightText);

    drawField(window, numberField, "Six-digit number");

    drawButton(window, generateButton, true);

    drawText(window, app.status, 320, 335, 17, accentColor);

    if (app.generated)
    {
        RectangleShape preview(Vector2f(480, 230));

        preview.setPosition(550, 400);
        preview.setFillColor(Color::White);

        window.draw(preview);

        Sprite barcode(app.barcodeTexture);

        float scaleX = 440.0f / app.barcodeTexture.getSize().x;
        float scaleY = 190.0f / app.barcodeTexture.getSize().y;

        float scale = std::min(scaleX, scaleY);

        barcode.setScale(scale, scale);
        barcode.setPosition(570, 420);

        window.draw(barcode);

        drawText(window,
            "Saved as barcode.bmp",
            550, 640, 16, accentColor);
    }
}

void drawScanner(RenderWindow& window)
{
    drawText(window, "Barcode Scanner", 270, 25, 32, textColor);

    drawText(window,
        "Place the barcode in front of the camera to view product details",
        272, 75, 16, lightText);

    drawCamera(window, 270, 110, 500, 375);

    drawButton(window, scannerStartButton, true);
    drawButton(window, scannerStopButton);
    drawButton(window, scannerAgainButton);

    drawPanel(window, 790, 110, 290, 375);

    drawText(window, "Product Description", 810, 125, 19, accentColor);

    Product* p = findProduct(scanner.barcode);

    if (scanner.barcode == "")
    {
        drawText(window, "Waiting for a barcode...", 810, 175, 16, lightText);
    }
    else if (p == NULL)
    {
        drawText(window, "Barcode: " + scanner.barcode, 810, 175, 16, textColor);
        drawText(window, "Product not found!", 810, 205, 18, errorColor);
    }
    else
    {
        drawLines(window, describeProduct(*p), 810, 170, 15, 30, textColor);
    }

    drawText(window, app.status, 270, 560, 16, accentColor);
}

void drawOrderTable(RenderWindow& window)
{
    drawPanel(window, 670, 70, 410, 520);

    drawText(window, "Current Order", 685, 80, 20, accentColor);

    drawText(window, "Item", 685, 115, 14, lightText);
    drawText(window, "Qty", 790, 115, 14, lightText);
    drawText(window, "Price", 850, 115, 14, lightText);
    drawText(window, "Amount", 960, 115, 14, lightText);

    drawLine(window, 685, 140, 380);

    for (int i = 0; i < app.items.size(); i++)
    {
        float y = 150 + i * 30;

        drawText(window, app.items[i].acronym, 685, y, 15, textColor);
        drawText(window, to_string(app.items[i].quantity), 790, y, 15, textColor);
        drawText(window, money(app.items[i].price), 850, y, 15, textColor);
        drawText(window,
            money(app.items[i].price * app.items[i].quantity),
            960, y, 15, textColor);
    }

    Totals totals = calculateTotals(app.items);

    drawLine(window, 685, 395, 380);

    drawText(window, "Total Without Tax", 685, 405, 15, lightText);
    drawText(window, money(totals.withoutTax), 960, 405, 15, textColor);

    drawText(window, "Total Tax", 685, 432, 15, lightText);
    drawText(window, money(totals.tax), 960, 432, 15, textColor);

    drawText(window, "Total Discount", 685, 459, 15, lightText);
    drawText(window, money(totals.discount), 960, 459, 15, textColor);

    drawText(window, "Total Payable", 685, 500, 20, textColor);
    drawText(window, money(totals.payable), 940, 500, 20, accentColor);
}

void drawOrderScreen(RenderWindow& window)
{
    drawText(window, "New Order", 260, 15, 28, textColor);

    drawField(window, nameField, "Customer Name");
    drawField(window, contactField, "Contact (optional)");

    drawCamera(window, 260, 150, 380, 285);

    Product* p = findProduct(scanner.barcode);

    if (app.showDescription && p != NULL)
    {
        drawPanel(window, 260, 150, 380, 285);

        drawText(window, "Product Description", 280, 162, 18, accentColor);

        drawLines(window, describeProduct(*p), 280, 200, 15, 24, textColor);
    }

    drawButton(window, orderStartButton);
    drawButton(window, orderStopButton);
    drawButton(window, orderDescriptionButton, app.showDescription);

    if (scanner.barcode == "")
    {
        drawText(window, "Scan a product barcode using the camera.",
            260, 490, 15, lightText);
    }
    else if (p == NULL)
    {
        drawText(window, "Barcode " + scanner.barcode + " is not registered.",
            260, 490, 15, errorColor);
    }
    else
    {
        drawText(window, "Barcode: " + p->barcode, 260, 490, 15, textColor);
        drawText(window, "Product: " + p->name, 260, 512, 15, accentColor);
        drawText(window,
            "Price: PKR " + money(p->price) +
            "   Available: " + to_string(p->quantity),
            260, 534, 15, textColor);
    }

    drawField(window, quantityField, "Quantity");

    drawButton(window, addButton, true);

    drawOrderTable(window);

    drawButton(window, removeButton);
    drawButton(window, holdButton);
    drawButton(window, billButton, true);

    drawText(window, app.status, 260, 655, 16, accentColor);
}

void drawBill(RenderWindow& window)
{
    Totals totals = calculateTotals(app.items);

    drawPanel(window, 270, 60, 810, 590);

    drawText(window, "BILL", 290, 70, 26, accentColor);
    drawText(window, "SMART POS", 940, 78, 16, lightText);

    drawText(window, "Bill ID: " + app.billId, 290, 110, 16, textColor);
    drawText(window, "Order ID: " + app.order.id, 290, 135, 16, textColor);
    drawText(window, "Customer: " + app.order.customer, 290, 160, 16, textColor);

    drawText(window, "Date: " + app.order.date, 700, 110, 16, textColor);
    drawText(window, "Time: " + app.order.time, 700, 135, 16, textColor);
    drawText(window, "Contact: " + app.order.contact, 700, 160, 16, textColor);

    drawText(window, "Product", 290, 195, 14, lightText);
    drawText(window, "Unit Price", 420, 195, 14, lightText);
    drawText(window, "Discount", 540, 195, 14, lightText);
    drawText(window, "Qty", 640, 195, 14, lightText);
    drawText(window, "Amount", 740, 195, 14, lightText);

    drawLine(window, 290, 218, 770);

    for (int i = 0; i < app.items.size(); i++)
    {
        float y = 228 + i * 26;

        drawText(window, app.items[i].acronym, 290, y, 15, textColor);
        drawText(window, money(app.items[i].price), 420, y, 15, textColor);
        drawText(window, money(app.items[i].discount), 540, y, 15, textColor);
        drawText(window, to_string(app.items[i].quantity), 640, y, 15, textColor);
        drawText(window,
            money(app.items[i].price * app.items[i].quantity),
            740, y, 15, textColor);
    }

    drawText(window, "Payment Mode", 290, 452, 14, lightText);

    for (int i = 0; i < paymentButtons.size(); i++)
        drawButton(window, paymentButtons[i],
            paymentButtons[i].target == app.order.mode);

    drawField(window, merchantField, "Merchant (e.g. ABL, JazzCash)");

    drawText(window, "Total Without Tax", 700, 452, 15, lightText);
    drawText(window, money(totals.withoutTax), 930, 452, 15, textColor);

    drawText(window, "Total Tax", 700, 480, 15, lightText);
    drawText(window, money(totals.tax), 930, 480, 15, textColor);

    drawText(window, "Total Discount", 700, 508, 15, lightText);
    drawText(window, money(totals.discount), 930, 508, 15, textColor);

    drawText(window, "Total Payable", 700, 545, 20, textColor);
    drawText(window, money(totals.payable), 920, 545, 20, accentColor);

    drawText(window, "Payment Status: " + app.order.status, 700, 595, 17,
        app.billSaved ? accentColor : textColor);

    drawButton(window, confirmButton, !app.billSaved);

    Button backButton = billBackButton;

    if (app.billSaved)
        backButton.title = "New Order";

    drawButton(window, backButton);

    drawText(window, app.status, 270, 660, 16, accentColor);
}

void drawProducts(RenderWindow& window)
{
    drawText(window, "Product Management", 280, 35, 32, textColor);

    drawText(window,
        "Products loaded from Product.txt",
        282, 85, 16, lightText);

    drawText(window,
        "Total Products: " + to_string(products.size()),
        280, 125, 19, accentColor);

    drawText(window, "ID", 290, 175, 16, accentColor);
    drawText(window, "Barcode", 350, 175, 16, accentColor);
    drawText(window, "Acronym", 450, 175, 16, accentColor);
    drawText(window, "Name", 560, 175, 16, accentColor);
    drawText(window, "Unit", 720, 175, 16, accentColor);
    drawText(window, "Price", 810, 175, 16, accentColor);
    drawText(window, "Qty", 920, 175, 16, accentColor);

    drawLine(window, 280, 205, 730);

    for (int i = 0; i < products.size() && i < 10; i++)
    {
        float y = 215 + i * 36;

        drawText(window, to_string(products[i].id), 290, y, 15, textColor);
        drawText(window, products[i].barcode, 350, y, 15, textColor);
        drawText(window, products[i].acronym, 450, y, 15, textColor);
        drawText(window, products[i].name, 560, y, 15, textColor);
        drawText(window, products[i].unit, 720, y, 15, textColor);
        drawText(window, money(products[i].price), 810, y, 15, textColor);
        drawText(window, to_string(products[i].quantity), 920, y, 15, textColor);
    }

    if (products.empty())
    {
        drawText(window,
            "No products found. Check Product.txt.",
            300, 250, 20, errorColor);
    }

    drawButton(window, barcodeImagesButton, true);

    drawText(window, app.status, 560, 637, 16, accentColor);
}

void drawHistory(RenderWindow& window)
{
    drawText(window, "Order History", 280, 35, 32, textColor);

    drawText(window,
        "Orders loaded from Order.txt. Click an order to select it.",
        282, 85, 16, lightText);

    drawText(window, "Order ID", 290, 115, 15, accentColor);
    drawText(window, "Date", 400, 115, 15, accentColor);
    drawText(window, "Customer", 510, 115, 15, accentColor);
    drawText(window, "Status", 670, 115, 15, accentColor);
    drawText(window, "Amount", 780, 115, 15, accentColor);
    drawText(window, "Mode", 900, 115, 15, accentColor);

    drawLine(window, 280, 140, 770);

    int total = orders.size();
    int start = std::max(0, total - HISTORY_ROWS);

    for (int i = start; i < total; i++)
    {
        float y = 150 + (i - start) * 34;

        if (i == app.selectedOrder)
        {
            RectangleShape row(Vector2f(770, 32));

            row.setPosition(280, y - 4);
            row.setFillColor(buttonColor);

            window.draw(row);
        }

        drawText(window, orders[i].id, 290, y, 15, textColor);
        drawText(window, orders[i].date, 400, y, 15, textColor);
        drawText(window, orders[i].customer, 510, y, 15, textColor);
        drawText(window, orders[i].status, 670, y, 15, textColor);
        drawText(window, money(orders[i].amount), 780, y, 15, textColor);
        drawText(window, orders[i].mode, 900, y, 15, textColor);
    }

    if (orders.empty())
        drawText(window, "No orders recorded yet.", 300, 200, 20, lightText);

    drawButton(window, archiveButton, true);

    drawText(window, app.status, 500, 620, 16, accentColor);
}

void drawScreen(RenderWindow& window)
{
    if (app.screen == "Dashboard")
        drawDashboard(window);

    else if (app.screen == "Generate Barcode")
        drawGenerator(window);

    else if (app.screen == "Barcode Scanner")
        drawScanner(window);

    else if (app.screen == "New Order")
        drawOrderScreen(window);

    else if (app.screen == "Bill")
        drawBill(window);

    else if (app.screen == "Products")
        drawProducts(window);

    else if (app.screen == "Order History")
        drawHistory(window);
}
