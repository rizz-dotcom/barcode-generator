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


