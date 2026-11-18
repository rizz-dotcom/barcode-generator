// Models.h
// All data structures (Product, Order, Button, Field, ...)

#pragma once

#include "Config.h"

struct Product
{
    int id;
    string barcode;
    string acronym;
    string name;
    string unit;
    float price;
    float tax;
    float discount;
    int quantity;
    string imagePath;
};

struct OrderItem
{
    int productId;
    string barcode;
    string acronym;
    float price;
    float tax;
    float discount;
    int quantity;
};

struct Order
{
    string id;
    string date;
    string time;
    string customer;
    string contact;
    string status;
    float amount = 0;
    string mode;
    string merchant;
};

struct Totals
{
    float withoutTax;
    float tax;
    float discount;
    float payable;
};

struct Run
{
    bool black;
    int width;
};

struct Button
{
    string title;
    string target;
    float x;
    float y;
    float w;
    float h;
};

struct Field
{
    float x;
    float y;
    float w;
    float h;
    string text;
    int limit;
    bool numbersOnly;
};
