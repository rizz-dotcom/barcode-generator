// BarcodeScanner.h
// Barcode decoding and the camera scanner

#pragma once

#include "BarcodeGenerator.h"

vector<Run> getRuns(Mat image, int row)
{
    vector<Run> runs;

    bool currentColor = false;
    int count = 0;
    bool started = false;

    for (int x = 0; x < image.cols; x++)
    {
        bool black = image.at<uchar>(row, x) < 128;

        if (!started)
        {
            if (!black)
                continue;

            currentColor = true;
            count = 1;
            started = true;
        }
        else if (black == currentColor)
        {
            count++;
        }
        else
        {
            runs.push_back({ currentColor, count });

            currentColor = black;
            count = 1;
        }
    }

    if (started)
        runs.push_back({ currentColor, count });

    return runs;
}

string getPattern(const vector<Run>& runs, int start)
{
    vector<int> positions;

    for (int i = 0; i < 9; i++)
        positions.push_back(i);

    sort(
        positions.begin(),
        positions.end(),
        [&](int a, int b)
        {
            return runs[start + a].width >
                runs[start + b].width;
        }
    );

    int smallestWide = runs[start + positions[2]].width;
    int largestNarrow = runs[start + positions[3]].width;

    if (smallestWide < largestNarrow * 1.7)
        return "";

    string pattern = "000000000";

    for (int i = 0; i < 3; i++)
        pattern[positions[i]] = '1';

    return pattern;
}

char findCharacter(string pattern)
{
    for (auto item : code39)
    {
        if (item.second == pattern)
            return item.first;
    }

    return '?';
}

// UPDATED: tries to read 8 Code 39 characters (*123456*)
// starting from one position in the list of runs.
string decodeFrom(const vector<Run>& runs, int position)
{
    string result = "";

    for (int c = 0; c < 8; c++)
    {
        if (position + 8 >= (int)runs.size())
            return "";

        for (int j = 0; j < 9; j++)
        {
            bool expectedBlack = (j % 2 == 0);

            if (runs[position + j].black != expectedBlack)
                return "";
        }

        string pattern = getPattern(runs, position);

        if (pattern == "")
            return "";

        char character = findCharacter(pattern);

        if (character == '?')
            return "";

        result += character;
        position += 9;

        if (c < 7)
        {
            if (position >= (int)runs.size() || runs[position].black)
                return "";

            position++;
        }
    }

    if (result[0] != '*' || result[7] != '*')
        return "";

    string number = result.substr(1, 6);

    if (!validateNumber(number))
        return "";

    return number;
}

// UPDATED: searches the whole row for the barcode, so dark things
// beside the barcode (table, fingers, shadows) no longer break scanning.
string decodeRow(Mat image, int row)
{
    vector<Run> runs = getRuns(image, row);

    for (int start = 0; start + 8 < (int)runs.size(); start++)
    {
        if (!runs[start].black)
            continue;

        string result = decodeFrom(runs, start);

        if (result != "")
            return result;
    }

    return "";
}

string decodeImage(Mat image)
{
    if (image.empty())
        return "";

    Mat gray;

    if (image.channels() == 3)
        cvtColor(image, gray, COLOR_BGR2GRAY);

    else if (image.channels() == 4)
        cvtColor(image, gray, COLOR_BGRA2GRAY);

    else
        gray = image.clone();

    Mat binary;

    threshold(gray, binary, 128, 255, THRESH_BINARY);

    for (int row = binary.rows / 3;
        row <= (binary.rows * 2) / 3;
        row += 3)
    {
        string result = decodeRow(binary, row);

        if (result != "")
            return result;
    }

    return "";
}

Mat prepareFrame(Mat frame)
{
    Mat gray;
    Mat binary;

    cvtColor(frame, gray, COLOR_BGR2GRAY);

    GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);

    return binary;
}

struct CameraScanner
{
    VideoCapture camera;
    Texture texture;
    bool running = false;
    bool ready = false;
    string barcode = "";
    int frameCount = 0;

    bool start()
    {
        if (running)
            return true;

        if (!camera.open(0, CAP_DSHOW))
        {
            if (!camera.open(0))
                return false;
        }

        camera.set(CAP_PROP_FRAME_WIDTH, 640);
        camera.set(CAP_PROP_FRAME_HEIGHT, 480);

        running = true;
        ready = false;
        barcode = "";
        frameCount = 0;

        return true;
    }

    void stop()
    {
        if (camera.isOpened())
            camera.release();

        running = false;
        ready = false;
        barcode = "";
    }

    void clearBarcode()
    {
        barcode = "";
    }

    void update()
    {
        if (!running)
            return;

        Mat frame;
        Mat rgba;

        if (!camera.read(frame) || frame.empty())
            return;

        if (!ready)
        {
            texture.create(frame.cols, frame.rows);
            ready = true;
        }

        cvtColor(frame, rgba, COLOR_BGR2RGBA);

        texture.update(rgba.ptr<uchar>());

        frameCount++;

        if (barcode == "" && frameCount % 8 == 0)
            barcode = decodeImage(prepareFrame(frame));
    }
};
