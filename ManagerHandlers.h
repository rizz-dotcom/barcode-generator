// ManagerHandlers.h
// Clicks and actions on the manager screens:
// Products, Inventory, Categories, Seasons, Reports

#pragma once

#include "ManagerScreens.h"

// ---------- Products ----------

void loadPreview(string path)
{
    app.previewReady = !path.empty() && fileExists(path) &&
        app.previewTexture.loadFromFile(path);
}

void resetProductForm()
{
    app.formNew = true;
    app.selected = -1;
    app.previewReady = false;

    pAcronymField.text = "";
    pNameField.text = "";
    pUnitField.text = "pcs";
    pPriceField.text = "";
    pTaxField.text = "0";
    pDiscountField.text = "0";
    pQtyField.text = "0";
}

void selectProduct(int index)
{
    Product& p = products[index];

    app.selected = index;
    app.formNew = false;

    int c = categoryIndex(p.categoryId);
    app.categoryChoice = c >= 0 ? c : 0;

    pAcronymField.text = p.acronym;
    pNameField.text = underscoreToSpaces(p.name);
    pUnitField.text = p.unit;
    pPriceField.text = money(p.price);
    pTaxField.text = money(p.tax);
    pDiscountField.text = money(p.discount);
    pQtyField.text = to_string(p.quantity);

    loadPreview(p.imagePath);

    setStatus("Editing " + underscoreToSpaces(p.name) + ". Change the fields and press Save Product.");
}

void saveProductForm()
{
    if (categories.empty())
    {
        setStatus("Add a product category first (Categories screen).", true);
        return;
    }

    string acronym = upperCase(spacesToUnderscore(trim(pAcronymField.text)));
    string name = spacesToUnderscore(trim(pNameField.text));
    string unit = spacesToUnderscore(trim(pUnitField.text));
    float price = toFloat(pPriceField.text, -1);
    float tax = toFloat(pTaxField.text, 0);
    float discount = toFloat(pDiscountField.text, 0);
    int quantity = toInt(pQtyField.text, -1);

    if (acronym.empty() || name.empty() || unit.empty())
    {
        setStatus("Acronym, name and unit are required.", true);
        return;
    }

    if (price <= 0)
    {
        setStatus("Enter a price greater than 0.", true);
        return;
    }

    if (tax < 0 || discount < 0 || discount > price)
    {
        setStatus("Tax must be 0 or more, and discount between 0 and the price.", true);
        return;
    }

    if (quantity < 0)
    {
        setStatus("Enter a quantity of 0 or more.", true);
        return;
    }

    bool editing = !app.formNew && app.selected >= 0 && app.selected < (int)products.size();
    int productId = editing ? products[app.selected].id : -1;

    if (acronymTaken(acronym, productId))
    {
        setStatus("Another product already uses the acronym " + acronym + ".", true);
        return;
    }

    string categoryId = categories[app.categoryChoice].id;
    Product p;

    if (editing)
        p = products[app.selected];
    else
        p.id = nextProductId();

    // New product, or category changed: make a new barcode
    if (!editing || p.categoryId != categoryId)
    {
        string barcode = nextBarcodeForCategory(categoryId);

        if (barcode.empty())
        {
            setStatus("This category is full (9999 products).", true);
            return;
        }

        p.barcode = barcode;
        p.categoryId = categoryId;
        p.imagePath = productImagePath(barcode);
    }

    p.acronym = acronym;
    p.name = name;
    p.unit = unit;
    p.price = round2(price);
    p.tax = round2(tax);
    p.discount = round2(discount);
    p.quantity = quantity;

    if (!generateBarcode(p.barcode, p.imagePath))
    {
        setStatus("Unable to save the barcode image.", true);
        return;
    }

    int index;

    if (editing)
    {
        products[app.selected] = p;
        index = app.selected;
    }
    else
    {
        products.push_back(p);
        index = (int)products.size() - 1;
    }

    if (!saveProducts())
    {
        setStatus("Unable to save Product.txt!", true);
        return;
    }

    selectProduct(index);

    setStatus(string(editing ? "Product updated. " : "Product added. ") +
        "Barcode " + p.barcode + " saved as " + p.imagePath);
}

void handleProductsClick(Vector2f mouse)
{
    int row = tableRowAt(productsTable, (int)products.size(), mouse);

    if (row >= 0)
    {
        selectProduct(row);
        return;
    }

    selectField(pAcronymField, mouse);
    selectField(pNameField, mouse);
    selectField(pUnitField, mouse);
    selectField(pPriceField, mouse);
    selectField(pTaxField, mouse);
    selectField(pDiscountField, mouse);
    selectField(pQtyField, mouse);

    if (!categories.empty())
    {
        int count = (int)categories.size();

        if (clicked(prevCategoryButton, mouse))
            app.categoryChoice = (app.categoryChoice - 1 + count) % count;

        if (clicked(nextCategoryButton, mouse))
            app.categoryChoice = (app.categoryChoice + 1) % count;
    }

    if (clicked(newProductButton, mouse))
    {
        resetProductForm();
        activeField = &pAcronymField;
        setStatus("Choose a category and fill in the new product.");
    }
    else if (clicked(saveProductButton, mouse))
    {
        saveProductForm();
    }
    else if (clicked(imagesButton, mouse))
    {
        int count = generateProductBarcodes();
        setStatus(to_string(count) + " barcode images saved in the " + BARCODE_FOLDER + " folder.");
    }
}

// ---------- Inventory ----------

void addStock()
{
    if (app.selected < 0 || app.selected >= (int)products.size())
    {
        setStatus("Select a product first.", true);
        return;
    }

    int amount = toInt(invAddField.text, 0);

    if (amount <= 0)
    {
        setStatus("Enter how many items to add.", true);
        return;
    }

    products[app.selected].quantity += amount;

    if (saveProducts())
    {
        invAddField.text = "";
        setStatus(to_string(amount) + " added. " + underscoreToSpaces(products[app.selected].name) +
            " now has " + to_string(products[app.selected].quantity) + ".");
    }
    else
    {
        setStatus("Unable to save Product.txt!", true);
    }
}

void setStock()
{
    if (app.selected < 0 || app.selected >= (int)products.size())
    {
        setStatus("Select a product first.", true);
        return;
    }

    int amount = toInt(invSetField.text, -1);

    if (amount < 0)
    {
        setStatus("Enter the new quantity (0 or more).", true);
        return;
    }

    products[app.selected].quantity = amount;

    if (saveProducts())
    {
        invSetField.text = "";
        setStatus(underscoreToSpaces(products[app.selected].name) + " quantity set to " +
            to_string(amount) + ".");
    }
    else
    {
        setStatus("Unable to save Product.txt!", true);
    }
}

void handleInventoryClick(Vector2f mouse)
{
    int row = tableRowAt(inventoryTable, (int)products.size(), mouse);

    if (row >= 0)
    {
        app.selected = row;
        activeField = &invAddField;
        setStatus("");
        return;
    }

    selectField(invAddField, mouse);
    selectField(invSetField, mouse);

    if (clicked(addStockButton, mouse))
        addStock();

    else if (clicked(setStockButton, mouse))
        setStock();
}

// ---------- Categories ----------

void resetCategoryForm()
{
    app.formNew = true;
    app.selected = -1;

    cIdField.text = "";
    cNameField.text = "";
    cAcronymField.text = "";
}

void saveCategoryForm()
{
    string id = trim(cIdField.text);
    string name = spacesToUnderscore(trim(cNameField.text));
    string acronym = upperCase(spacesToUnderscore(trim(cAcronymField.text)));

    if (name.empty() || acronym.empty())
    {
        setStatus("Name and acronym are required.", true);
        return;
    }

    bool editing = !app.formNew && app.selected >= 0 && app.selected < (int)categories.size();

    if (editing)
    {
        categories[app.selected].name = name;
        categories[app.selected].acronym = acronym;
    }
    else
    {
        if (!isValidCategoryId(id))
        {
            setStatus("Category ID must be 2 digits (01 to 99).", true);
            return;
        }

        if (findCategory(id) != NULL)
        {
            setStatus("Category ID " + id + " already exists.", true);
            return;
        }

        categories.push_back({ id, name, acronym });

        std::sort(categories.begin(), categories.end(),
            [](const Category& a, const Category& b)
            {
                return a.id < b.id;
            });
    }

    if (saveCategories())
    {
        setStatus(editing ? "Category updated." : "Category " + id + " added.");
        resetCategoryForm();
    }
    else
    {
        setStatus("Unable to save PCategory.txt!", true);
    }
}

void handleCategoriesClick(Vector2f mouse)
{
    int row = tableRowAt(categoriesTable, (int)categories.size(), mouse);

    if (row >= 0)
    {
        app.selected = row;
        app.formNew = false;

        cIdField.text = categories[row].id;
        cNameField.text = underscoreToSpaces(categories[row].name);
        cAcronymField.text = categories[row].acronym;

        activeField = &cNameField;
        setStatus("");
        return;
    }

    if (app.formNew)
        selectField(cIdField, mouse);

    selectField(cNameField, mouse);
    selectField(cAcronymField, mouse);

    if (clicked(newCategoryButton, mouse))
    {
        resetCategoryForm();
        activeField = &cIdField;
        setStatus("");
    }
    else if (clicked(saveCategoryButton, mouse))
    {
        saveCategoryForm();
    }
}

// ---------- Seasons ----------

void resetSeasonForm()
{
    app.formNew = true;
    app.selected = -1;

    sStartField.text = "";
    sEndField.text = "";
    sPercentField.text = "";
}

void saveSeasonForm()
{
    string start = trim(sStartField.text);
    string end = trim(sEndField.text);
    float percent = toFloat(sPercentField.text, -1);

    if (!isValidDate(start) || !isValidDate(end))
    {
        setStatus("Enter dates as dd-mm-yyyy, for example 01-12-2026.", true);
        return;
    }

    if (dateKey(start) > dateKey(end))
    {
        setStatus("The start date must be before the end date.", true);
        return;
    }

    if (percent <= 0 || percent > 100)
    {
        setStatus("Discount must be more than 0 and at most 100 percent.", true);
        return;
    }

    bool editing = !app.formNew && app.selected >= 0 && app.selected < (int)seasons.size();

    if (editing)
    {
        seasons[app.selected].start = start;
        seasons[app.selected].end = end;
        seasons[app.selected].percent = round2(percent);
    }
    else
    {
        seasons.push_back({ start, end, round2(percent), true });
    }

    if (saveSeasons())
    {
        setStatus(editing ? "Season updated." : "Season added and set active.");
        resetSeasonForm();
    }
    else
    {
        setStatus("Unable to save Seasons.txt!", true);
    }
}

void toggleSeason()
{
    if (app.selected < 0 || app.selected >= (int)seasons.size())
    {
        setStatus("Select a season first.", true);
        return;
    }

    seasons[app.selected].active = !seasons[app.selected].active;

    if (saveSeasons())
        setStatus(string("Season is now ") + (seasons[app.selected].active ? "ACTIVE." : "INACTIVE."));
    else
        setStatus("Unable to save Seasons.txt!", true);
}

void handleSeasonsClick(Vector2f mouse)
{
    int row = tableRowAt(seasonsTable, (int)seasons.size(), mouse);

    if (row >= 0)
    {
        app.selected = row;
        app.formNew = false;

        sStartField.text = seasons[row].start;
        sEndField.text = seasons[row].end;
        sPercentField.text = money(seasons[row].percent);

        setStatus("");
        return;
    }

    selectField(sStartField, mouse);
    selectField(sEndField, mouse);
    selectField(sPercentField, mouse);

    if (clicked(newSeasonButton, mouse))
    {
        resetSeasonForm();
        activeField = &sStartField;
        setStatus("");
    }
    else if (clicked(saveSeasonButton, mouse))
    {
        saveSeasonForm();
    }
    else if (clicked(toggleSeasonButton, mouse))
    {
        toggleSeason();
    }
}

// ---------- Reports ----------

void buildCurrentReport()
{
    app.scroll = 0;

    switch (app.reportTab)
    {
    case 0:
        app.report = reportIncomplete();
        break;

    case 1:
        app.report = reportCompleted();
        break;

    case 2:
    {
        string from, to;

        if (app.cashPeriod == "Custom")
        {
            from = trim(rFromField.text);
            to = trim(rToField.text);

            if (!isValidDate(from) || !isValidDate(to) || dateKey(from) > dateKey(to))
            {
                setStatus("Enter From and To dates as dd-mm-yyyy (From before To).", true);
                return;
            }
        }
        else
        {
            periodRange(app.cashPeriod, from, to);
        }

        app.report = reportCashSales(from, to, app.cashPeriod);
        break;
    }

    case 3:
        app.report = reportAllSales();
        break;

    case 4:
        app.report = reportTopSold();
        break;

    default:
        app.report = reportTopStock();
        break;
    }

    setStatus("");
}

void handleReportsClick(Vector2f mouse)
{
    for (int i = 0; i < (int)reportNames.size(); i++)
    {
        if (clicked(reportTabButton(i), mouse))
        {
            app.reportTab = i;
            buildCurrentReport();
            return;
        }
    }

    if (app.reportTab != 2)
        return;

    for (int i = 0; i < (int)periodNames.size(); i++)
    {
        if (clicked(periodButton(i), mouse))
        {
            app.cashPeriod = periodNames[i];

            if (app.cashPeriod == "Custom")
            {
                if (rFromField.text.empty())
                    rFromField.text = "01" + currentDate().substr(2);

                if (rToField.text.empty())
                    rToField.text = currentDate();

                activeField = &rFromField;
            }

            buildCurrentReport();
            return;
        }
    }

    if (app.cashPeriod == "Custom")
    {
        selectField(rFromField, mouse);
        selectField(rToField, mouse);

        if (clicked(applyButton, mouse))
            buildCurrentReport();
    }
}
