// ProductManager.h
// Load/save/find products in Product.txt

#pragma once

#include "Models.h"
#include "Utils.h"

vector<Product> products;

bool loadProducts()
{
    ifstream file("Product.txt");

    if (!file.is_open())
        return false;

    products.clear();

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        Product p;
        string marker;

        if (ss >> p.id
            >> p.barcode
            >> p.acronym
            >> p.name
            >> p.unit
            >> p.price
            >> p.tax
            >> p.discount
            >> p.quantity
            >> p.imagePath
            >> marker)
        {
            if (marker == "#")
                products.push_back(p);
        }
    }

    file.close();

    return true;
}

bool saveProducts()
{
    ofstream file("Product.txt");

    if (!file.is_open())
        return false;

    for (int i = 0; i < products.size(); i++)
    {
        file << products[i].id << " "
            << products[i].barcode << " "
            << products[i].acronym << " "
            << products[i].name << " "
            << products[i].unit << " "
            << money(products[i].price) << " "
            << money(products[i].tax) << " "
            << money(products[i].discount) << " "
            << products[i].quantity << " "
            << products[i].imagePath << " #\n";
    }

    file.close();

    return true;
}

Product* findProduct(string barcode)
{
    for (int i = 0; i < products.size(); i++)
    {
        if (products[i].barcode == barcode)
            return &products[i];
    }

    return NULL;
}

vector<string> describeProduct(const Product& p)
{
    vector<string> lines;

    lines.push_back("Product ID: " + to_string(p.id));
    lines.push_back("Barcode: " + p.barcode);
    lines.push_back("Acronym: " + p.acronym);
    lines.push_back("Name: " + p.name);
    lines.push_back("Unit: " + p.unit);
    lines.push_back("Unit Price: PKR " + money(p.price));
    lines.push_back("Tax: PKR " + money(p.tax));
    lines.push_back("Discount: PKR " + money(p.discount));
    lines.push_back("Available: " + to_string(p.quantity));
    lines.push_back("Barcode File: " + p.imagePath);

    return lines;
}
