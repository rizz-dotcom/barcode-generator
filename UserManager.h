 //UserManager.h
// Login users stored in Users.txt
// Line format:  Username Password Role #
// Example:      manager admin123 manager #
// If Users.txt is missing, it is created with two default users.

#pragma once

#include "Models.h"
#include "Utils.h"

vector<User> users;

void loadUsers()
{
    users.clear();

    if (!fileExists(USER_FILE))
    {
        writeLines(USER_FILE, {
            "manager admin123 manager #",
            "cashier cash123 cashier #"
            });
    }

    vector<vector<string>> records = readRecords(USER_FILE);

    for (size_t i = 0; i < records.size(); i++)
    {
        if (records[i].size() < 3)
            continue;

        User u;

        u.username = records[i][0];
        u.password = records[i][1];
        u.role = records[i][2];

        if (u.role == "manager" || u.role == "cashier")
            users.push_back(u);
    }
}

User* findUser(string username, string password)
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i].username == username && users[i].password == password)
            return &users[i];
    }

    return NULL;
}

