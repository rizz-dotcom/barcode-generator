// SeasonManager.h
// Seasonal discounts stored in Seasons.txt
// Line format:  StartDate EndDate DiscountPercent isActive #
// Example:      01-11-2026 30-11-2026 10.00 true #

#pragma once

#include "Models.h"
#include "Utils.h"

vector<Season> seasons;

bool loadSeasons()
{
    seasons.clear();

    if (!fileExists(SEASON_FILE))
        return false;

    vector<vector<string>> records = readRecords(SEASON_FILE);

    for (size_t i = 0; i < records.size(); i++)
    {
        if (records[i].size() < 4)
            continue;

        Season s;

        s.start = records[i][0];
        s.end = records[i][1];
        s.percent = toFloat(records[i][2]);
        s.active = (records[i][3] == "true");

        seasons.push_back(s);
    }

    return true;
}

bool saveSeasons()
{
    vector<string> lines;

    for (size_t i = 0; i < seasons.size(); i++)
    {
        lines.push_back(seasons[i].start + " " +
            seasons[i].end + " " +
            money(seasons[i].percent) + " " +
            (seasons[i].active ? "true" : "false") + " #");
    }

    return writeLines(SEASON_FILE, lines);
}

// Is this season giving a discount on this date?
bool seasonRunning(const Season& s, string date)
{
    return s.active && dateInRange(date, s.start, s.end);
}

// Discount percent for a date (the biggest running season wins)
float activeSeasonPercent(string date)
{
    float best = 0;

    for (size_t i = 0; i < seasons.size(); i++)
    {
        if (seasonRunning(seasons[i], date))
            best = std::max(best, seasons[i].percent);
    }

    return best;
}
