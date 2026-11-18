// BarcodeGenerator.h
// Code 39 barcode creation and saving as .bmp images

#pragma once

#include "ProductManager.h"

map<char, string> code39 =
{
    {'0', "000110100"},
    {'1', "100100001"},
    {'2', "001100001"},
    {'3', "101100000"},
    {'4', "000110001"},
    {'5', "100110000"},
    {'6', "001110000"},
    {'7', "000100101"},
    {'8', "100100100"},
    {'9', "001100100"},
    {'*', "010010100"}
};

const int NARROW = 3;
const int WIDE = 9;

const int BAR_HEIGHT = 125;
const int QUIET_ZONE = 60;
const int IMAGE_HEIGHT = 190;

bool validateNumber(string number)
{
    if (number.length() != 6 || number[0] == '0')
        return false;

    for (int i = 0; i < number.length(); i++)
    {
        if (number[i] < '0' || number[i] > '9')
            return false;
    }

    return true;
}

int getWidth(char element)
{
    if (element == '0')
        return NARROW;

    return WIDE;
}

int calculateWidth(string data)
{
    int totalWidth = QUIET_ZONE * 2;

    for (int i = 0; i < data.length(); i++)
    {
        string pattern = code39.at(data[i]);

        for (int j = 0; j < pattern.length(); j++)
            totalWidth += getWidth(pattern[j]);

        if (i != data.length() - 1)
            totalWidth += NARROW;
    }

    return totalWidth;
}

Mat drawBarcode(string data)
{
    int width = calculateWidth(data);

    Mat image(
        IMAGE_HEIGHT,
        width,
        CV_8UC3,
        Scalar(255, 255, 255)
    );

    int x = QUIET_ZONE;

    for (int i = 0; i < data.length(); i++)
    {
        string pattern = code39.at(data[i]);

        for (int j = 0; j < pattern.length(); j++)
        {
            int elementWidth = getWidth(pattern[j]);

            if (j % 2 == 0)
            {
                rectangle(
                    image,
                    cv::Rect(x, 10, elementWidth, BAR_HEIGHT),
                    Scalar(0, 0, 0),
                    FILLED
                );
            }

            x += elementWidth;
        }

        if (i != data.length() - 1)
            x += NARROW;
    }

    return image;
}

void addNumber(Mat& image, string number)
{
    int fontType = FONT_HERSHEY_SIMPLEX;
    double scale = 0.8;
    int thickness = 2;

    int baseline;

    cv::Size textSize = getTextSize(
        number,
        fontType,
        scale,
        thickness,
        &baseline
    );

    int x = (image.cols - textSize.width) / 2;

    putText(
        image,
        number,
        cv::Point(x, 175),
        fontType,
        scale,
        Scalar(0, 0, 0),
        thickness
    );
}

bool generateBarcode(string number, string fileName = "barcode.bmp")
{
    if (!validateNumber(number))
        return false;

    string data = "*" + number + "*";

    Mat barcode = drawBarcode(data);

    addNumber(barcode, number);

    if (imwrite(fileName, barcode))
        return true;

    return false;
}

int generateProductBarcodes()
{
    int count = 0;

    for (int i = 0; i < products.size(); i++)
    {
        if (generateBarcode(products[i].barcode, products[i].imagePath))
            count++;
    }

    return count;
}
