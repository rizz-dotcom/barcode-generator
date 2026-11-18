// Utils.h
// Small helpers: money format, dates/times, padding, line counting

#pragma once

#include "Config.h"

string money(float value)
{
    stringstream ss;

    ss << fixed << setprecision(2) << value;

    return ss.str();
}

string padNumber(int value, int width)
{
    string text = to_string(value);

    while ((int)text.length() < width)
        text = "0" + text;

    return text;
}

string spacesToUnderscore(string text)
{
    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] == ' ')
            text[i] = '_';
    }

    return text;
}

bool isBlank(string text)
{
    return text.find_first_not_of(' ') == string::npos;
}

string formatTime(const char* format)
{
    time_t now = time(0);
    struct tm* local = localtime(&now);

    char buffer[30];

    strftime(buffer, 30, format, local);

    return string(buffer);
}

string currentDate()
{
    return formatTime("%d-%m-%Y");
}

string currentTime()
{
    return formatTime("%H:%M:%S");
}

string currentDay()
{
    return formatTime("%d");
}

int countLines(string fileName)
{
    ifstream file(fileName);

    string line;
    int count = 0;

    while (getline(file, line))
    {
        if (!line.empty())
            count++;
    }

    return count;
}



