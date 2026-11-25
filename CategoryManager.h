// CategoryManager.h
// Product categories stored in PCategory.txt
// Line format:  CategoryId CategoryName CategoryAcronym #
// Example:      15 Personal_Care PRC #

#pragma once

#include "Models.h"
#include "Utils.h"

vector<Category> categories;

bool loadCategories()
{
    categories.clear();

    if (!fileExists(CATEGORY_FILE))
        return false;

    vector<vector<string>> records = readRecords(CATEGORY_FILE);

    for (size_t i = 0; i < records.size(); i++)
    {
        if (records[i].size() < 3)
            continue;

        Category c;

        c.id = records[i][0];
        c.name = records[i][1];
        c.acronym = records[i][2];

        categories.push_back(c);
    }

    std::sort(categories.begin(), categories.end(),
        [](const Category& a, const Category& b)
        {
            return a.id < b.id;
        });

    return true;
}

bool saveCategories()
{
    vector<string> lines;

    for (size_t i = 0; i < categories.size(); i++)
    {
        lines.push_back(categories[i].id + " " +
            categories[i].name + " " +
            categories[i].acronym + " #");
    }

    return writeLines(CATEGORY_FILE, lines);
}

int categoryIndex(string id)
{
    for (size_t i = 0; i < categories.size(); i++)
    {
        if (categories[i].id == id)
            return (int)i;
    }

    return -1;
}

Category* findCategory(string id)
{
    int index = categoryIndex(id);

    if (index < 0)
        return NULL;

    return &categories[index];
}

string categoryName(string id)
{
    Category* c = findCategory(id);

    if (c == NULL)
        return "Unknown";

    return underscoreToSpaces(c->name);
}

// A category id is exactly 2 digits and not 00
bool isValidCategoryId(string id)
{
    return id.length() == 2 && isDigits(id) && id != "00";
}
