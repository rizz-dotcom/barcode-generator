// Config.h
// Libraries, namespaces, window size, colours and the font

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
#include <limits>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;
using namespace cv;
using namespace sf;

const int WIDTH = 1100;
const int HEIGHT = 700;
const int MAX_ITEMS = 8;
const int HISTORY_ROWS = 12;

Color background(18, 25, 38);
Color sidebarColor(24, 34, 50);
Color cardColor(32, 44, 62);
Color buttonColor(40, 58, 78);
Color accentColor(0, 190, 170);
Color textColor(235, 240, 245);
Color lightText(155, 170, 190);
Color errorColor(235, 95, 95);

Font font;
