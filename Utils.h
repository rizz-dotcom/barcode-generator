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

