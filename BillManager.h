// Config.h
// Libraries, namespaces, window size, colours, file names and settings

#pragma once

// Must come before the includes (stops Visual Studio warnings about localtime)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <SFML/Graphics.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include <filesystem>

using namespace std;
using namespace cv;
using namespace sf;

namespace fs = std::filesystem;

// Layout size (the app is scaled to full screen from this size)
const int WIDTH = 1100;
const int HEIGHT = 700;

// Business rules
const int MAX_ITEMS = 8;                 // different products in one order
const int LOW_STOCK_LIMIT = 5;           // stock at or below this is "LOW"
const int EDIT_DAYS_LIMIT = 7;           // paid orders editable for 7 days
const float MAX_SPECIAL_DISCOUNT = 8.0f; // special discount limit (percent)

// Data files (kept in the Working Directory)
const string PRODUCT_FILE = "Product.txt";
const string CATEGORY_FILE = "PCategory.txt";
const string ORDER_FILE = "Order.txt";
const string ORDER_DETAIL_FILE = "OrderDetail.txt";
const string BILL_FILE = "Bill.txt";
const string BILL_DETAIL_FILE = "BillDetail.txt";
const string SEASON_FILE = "Seasons.txt";
const string USER_FILE = "Users.txt";

// Folders
const string BARCODE_FOLDER = "Barcodes";
const string BILL_FOLDER = "Bills";

// Colours
Color background(18, 25, 38);
Color sidebarColor(24, 34, 50);
Color cardColor(32, 44, 62);
Color buttonColor(40, 58, 78);
Color accentColor(0, 190, 170);
Color textColor(235, 240, 245);
Color lightText(155, 170, 190);
Color errorColor(235, 95, 95);
Color warningColor(240, 180, 80);

Font font;
