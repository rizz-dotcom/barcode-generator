// EventHandlers.h
// Mouse click handling and actions for each screen

#pragma once

#include "Screens.h"

void clearOrder()
{
    app.items.clear();
    app.showDescription = false;
    app.billSaved = false;
    app.billId = "";

    nameField.text = "";
    contactField.text = "";
    quantityField.text = "1";
    merchantField.text = "";
}

void handleCameraButtons(const Button& startButton,
    const Button& stopButton, Vector2f mouse)
{
    if (clicked(startButton, mouse))
    {
        if (scanner.start())
            app.status = "Camera started. Place barcode in view.";
        else
            app.status = "Unable to open camera!";
    }
    else if (clicked(stopButton, mouse))
    {
        scanner.stop();
        app.status = "Camera stopped.";
    }
}

void handleDashboardClick(Vector2f mouse)
{
    string target = findClickedTarget(quickButtons, mouse);

    if (target != "")
        app.screen = target;
}

void handleGeneratorClick(Vector2f mouse)
{
    selectField(numberField, mouse);

    if (!clicked(generateButton, mouse))
        return;

    app.generated = false;

    if (!validateNumber(numberField.text))
    {
        app.status = "Enter a valid six-digit number!";
    }
    else if (generateBarcode(numberField.text))
    {
        if (app.barcodeTexture.loadFromFile("barcode.bmp"))
        {
            app.generated = true;
            app.status = "Barcode generated successfully!";
        }
        else
        {
            app.status = "Unable to load barcode image!";
        }
    }
    else
    {
        app.status = "Unable to save barcode!";
    }
}

void handleScannerClick(Vector2f mouse)
{
    handleCameraButtons(scannerStartButton, scannerStopButton, mouse);

    if (clicked(scannerAgainButton, mouse))
    {
        scanner.clearBarcode();
        app.status = "Ready for the next barcode.";
    }
}

void addScannedProduct()
{
    if (scanner.barcode == "")
    {
        app.status = "Scan a product first.";
        return;
    }

    Product* p = findProduct(scanner.barcode);

    if (p == NULL)
    {
        app.status = "Product not found in Product.txt.";
        return;
    }

    int quantity = 0;

    if (!quantityField.text.empty())
        quantity = stoi(quantityField.text);

    string message;

    if (addItem(app.items, *p, quantity, message))
    {
        scanner.clearBarcode();
        quantityField.text = "1";
        app.showDescription = false;
    }

    app.status = message;
}

bool orderIsReady()
{
    if (app.items.empty())
    {
        app.status = "Add at least one product first.";
        return false;
    }

    if (isBlank(nameField.text))
    {
        app.status = "Enter the customer name.";
        return false;
    }

    return true;
}

void holdOrder()
{
    if (!orderIsReady())
        return;

    Order order = createOrder(nameField.text, contactField.text);

    order.status = "on-hold";
    order.mode = "-";
    order.amount = calculateTotals(app.items).payable;

    if (appendOrder(order) && saveOrderDetails(order.id, app.items))
    {
        clearOrder();
        app.status = "Order " + order.id + " saved as on-hold.";
    }
    else
    {
        app.status = "Unable to save the order!";
    }
}

void openBill()
{
    if (!orderIsReady())
        return;

    app.order = createOrder(nameField.text, contactField.text);
    app.billId = createBillId();
    app.billSaved = false;
    merchantField.text = "";
    app.screen = "Bill";
}

void handleOrderClick(Vector2f mouse)
{
    selectField(nameField, mouse);
    selectField(contactField, mouse);
    selectField(quantityField, mouse);

    handleCameraButtons(orderStartButton, orderStopButton, mouse);

    if (clicked(orderDescriptionButton, mouse))
    {
        if (findProduct(scanner.barcode) == NULL)
        {
            app.status = "Scan a registered product first.";
        }
        else
        {
            app.showDescription = !app.showDescription;
            app.status = "";
        }
    }
    else if (clicked(addButton, mouse))
    {
        addScannedProduct();
    }
    else if (clicked(removeButton, mouse))
    {
        if (removeLastItem(app.items))
            app.status = "Last product removed.";
        else
            app.status = "The order is empty.";
    }
    else if (clicked(holdButton, mouse))
    {
        holdOrder();
    }
    else if (clicked(billButton, mouse))
    {
        openBill();
    }
}

void confirmPayment()
{
    if (app.billSaved)
        return;

    if (app.order.mode != "cash" && isBlank(merchantField.text))
    {
        app.status = "Enter the merchant name for card or wallet payment.";
        return;
    }

    Totals totals = calculateTotals(app.items);

    if (app.order.mode == "cash")
        app.order.merchant = "-";
    else
        app.order.merchant = spacesToUnderscore(merchantField.text);

    app.order.status = "paid";
    app.order.amount = totals.payable;

    bool saved = appendOrder(app.order) &&
        saveOrderDetails(app.order.id, app.items) &&
        saveBill(app.billId, app.order, app.items, totals) &&
        reduceStock(app.items);

    if (saved)
    {
        app.billSaved = true;
        app.status = "Payment received. Bill saved in Bill.txt.";
    }
    else
    {
        app.order.status = "in-progress";
        app.status = "Unable to save the bill files!";
    }
}

void handleBillClick(Vector2f mouse)
{
    selectField(merchantField, mouse);

    if (!app.billSaved)
    {
        string mode = findClickedTarget(paymentButtons, mouse);

        if (mode != "")
            app.order.mode = mode;
    }

    if (clicked(confirmButton, mouse))
    {
        confirmPayment();
    }
    else if (clicked(billBackButton, mouse))
    {
        if (app.billSaved)
            clearOrder();

        app.screen = "New Order";
    }
}

void handleProductsClick(Vector2f mouse)
{
    if (clicked(barcodeImagesButton, mouse))
    {
        int count = generateProductBarcodes();

        app.status = to_string(count) + " barcode images saved.";
    }
}

void handleHistoryClick(Vector2f mouse)
{
    int total = orders.size();
    int start = std::max(0, total - HISTORY_ROWS);

    for (int i = start; i < total; i++)
    {
        float y = 150 + (i - start) * 34;

        if (clicked(280, y - 4, 770, 32, mouse))
            app.selectedOrder = i;
    }

    if (!clicked(archiveButton, mouse))
        return;

    if (app.selectedOrder < 0 || app.selectedOrder >= total)
    {
        app.status = "Select an order first.";
    }
    else if (orders[app.selectedOrder].status != "paid")
    {
        app.status = "Only paid orders can be archived.";
    }
    else
    {
        orders[app.selectedOrder].status = "archived";

        if (saveOrders())
            app.status = "Order archived.";
        else
            app.status = "Unable to update Order.txt!";
    }
}

void handleClick(Vector2f mouse)
{
    activeField = NULL;

    string target = findClickedTarget(sidebarButtons, mouse);

    if (target != "")
    {
        app.screen = target;
        return;
    }

    if (app.screen == "Dashboard")
        handleDashboardClick(mouse);

    else if (app.screen == "Generate Barcode")
        handleGeneratorClick(mouse);

    else if (app.screen == "Barcode Scanner")
        handleScannerClick(mouse);

    else if (app.screen == "New Order")
        handleOrderClick(mouse);

    else if (app.screen == "Bill")
        handleBillClick(mouse);

    else if (app.screen == "Products")
        handleProductsClick(mouse);

    else if (app.screen == "Order History")
        handleHistoryClick(mouse);
}

void onScreenChange()
{
    scanner.stop();

    activeField = NULL;

    app.status = "";
    app.showDescription = false;

    loadProducts();
    loadOrders();

    if (app.screen == "Generate Barcode")
    {
        numberField.text = "";
        app.generated = false;
        activeField = &numberField;
    }

    if (app.screen == "Order History")
        app.selectedOrder = -1;
}
