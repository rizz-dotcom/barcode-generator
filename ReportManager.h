// ReportManager.h
// Manager reports shown as tables

#pragma once

#include "BillManager.h"

struct Report
{
    string title;
    vector<string> headers;
    vector<float> columns;          // x position of each column
    vector<vector<string>> rows;
    string summary;
};

// Report 1: orders that are not paid or archived
Report reportIncomplete()
{
    loadOrders();

    Report r;

    r.title = "Incomplete Orders (not paid or archived)";
    r.headers = { "Order ID", "Date", "Time", "Customer", "Status", "Amount" };
    r.columns = { 270, 380, 480, 570, 730, 850 };

    float total = 0;

    for (int i = (int)orders.size() - 1; i >= 0; i--)
    {
        Order& o = orders[i];

        if (isCompleted(o))
            continue;

        r.rows.push_back({ o.id, o.date, o.time, underscoreToSpaces(o.customer),
            o.status, money(o.amount) });

        total += o.amount;
    }

    r.summary = "Incomplete orders: " + to_string(r.rows.size()) +
        "      Total amount: PKR " + money(total);

    return r;
}

// Report 2: orders that are paid or archived
Report reportCompleted()
{
    loadOrders();
    loadBills();

    Report r;

    r.title = "Completed Orders (paid or archived)";
    r.headers = { "Order ID", "Date", "Customer", "Status", "Mode", "Bill ID", "Amount" };
    r.columns = { 270, 370, 470, 630, 730, 810, 940 };

    float total = 0;

    for (int i = (int)orders.size() - 1; i >= 0; i--)
    {
        Order& o = orders[i];

        if (!isCompleted(o))
            continue;

        int b = findBillIndexByOrder(o.id);

        r.rows.push_back({ o.id, o.date, underscoreToSpaces(o.customer), o.status,
            o.mode, b >= 0 ? bills[b].id : "-", money(o.amount) });

        total += o.amount;
    }

    r.summary = "Completed orders: " + to_string(r.rows.size()) +
        "      Total amount: PKR " + money(total);

    return r;
}

// Gives the date range of Daily / Weekly / Monthly / Yearly
void periodRange(string period, string& from, string& to)
{
    to = currentDate();

    if (period == "Weekly")
        from = addDays(to, -6);
    else if (period == "Monthly")
        from = "01" + to.substr(2);
    else if (period == "Yearly")
        from = "01-01" + to.substr(5);
    else
        from = to;
}

// Report 3: cash sales in a date range
Report reportCashSales(string from, string to, string label)
{
    loadOrders();
    loadBills();

    Report r;

    r.title = "Cash Sales - " + label + " (" + from + " to " + to + ")";
    r.headers = { "Bill ID", "Date", "Time", "Customer", "Order ID", "Amount" };
    r.columns = { 270, 380, 480, 570, 730, 850 };

    float total = 0;

    for (int i = (int)bills.size() - 1; i >= 0; i--)
    {
        Bill& b = bills[i];
        int o = findOrderIndex(b.orderId);

        if (o < 0 || orders[o].mode != "cash" || !isCompleted(orders[o]))
            continue;

        if (!dateInRange(b.date, from, to))
            continue;

        r.rows.push_back({ b.id, b.date, b.time, underscoreToSpaces(b.customer),
            b.orderId, money(b.payable) });

        total += b.payable;
    }

    r.summary = "Cash bills: " + to_string(r.rows.size()) +
        "      Total cash sales: PKR " + money(total);

    return r;
}

// Report 4: all sales with amounts and discounts
Report reportAllSales()
{
    loadBills();

    Report r;

    r.title = "All Sales";
    r.headers = { "Bill ID", "Date", "Customer", "Subtotal", "Tax",
        "Item Disc", "Seasonal", "Special", "Payable" };
    r.columns = { 270, 365, 460, 575, 665, 740, 825, 905, 985 };

    float sales = 0, itemDisc = 0, seasonal = 0, special = 0;

    for (int i = (int)bills.size() - 1; i >= 0; i--)
    {
        Bill& b = bills[i];

        if (b.status != "paid" && b.status != "archived")
            continue;

        r.rows.push_back({ b.id, b.date, underscoreToSpaces(b.customer),
            money(b.withoutTax), money(b.tax), money(b.discount),
            money(b.seasonal), money(b.special), money(b.payable) });

        sales += b.payable;
        itemDisc += b.discount;
        seasonal += b.seasonal;
        special += b.special;
    }

    r.summary = "Sales: PKR " + money(sales) +
        "  |  Item disc: " + money(itemDisc) +
        "  |  Seasonal: " + money(seasonal) +
        "  |  Special: " + money(special) +
        "  |  All discounts: " + money(itemDisc + seasonal + special);

    return r;
}

// Report 5: top 10 most sold products
Report reportTopSold()
{
    loadOrders();
    loadOrderDetails();
    loadProducts();

    map<int, int> sold;

    for (size_t i = 0; i < orderDetails.size(); i++)
    {
        int o = findOrderIndex(orderDetails[i].orderId);

        if (o >= 0 && isCompleted(orders[o]))
            sold[orderDetails[i].productId] += orderDetails[i].quantity;
    }

    vector<pair<int, int>> list(sold.begin(), sold.end());

    std::sort(list.begin(), list.end(),
        [](const pair<int, int>& a, const pair<int, int>& b)
        {
            return a.second > b.second;
        });

    Report r;

    r.title = "Top 10 Most Sold Products";
    r.headers = { "Rank", "Barcode", "Name", "Category", "Sold Qty", "Remaining" };
    r.columns = { 270, 330, 430, 640, 800, 920 };

    for (size_t i = 0; i < list.size() && i < 10; i++)
    {
        Product* p = findProductById(list[i].first);

        if (p == NULL)
            continue;

        r.rows.push_back({ to_string(r.rows.size() + 1), p->barcode,
            underscoreToSpaces(p->name), categoryName(p->categoryId),
            to_string(list[i].second), to_string(p->quantity) });
    }

    r.summary = "Based on paid and archived orders.";

    return r;
}

// Report 6: top 10 products with the most stock left
Report reportTopStock()
{
    loadProducts();

    vector<Product> list = products;

    std::sort(list.begin(), list.end(),
        [](const Product& a, const Product& b)
        {
            return a.quantity > b.quantity;
        });

    Report r;

    r.title = "Top 10 Products With Most Stock (no need to purchase)";
    r.headers = { "Rank", "Barcode", "Name", "Category", "Remaining", "Unit" };
    r.columns = { 270, 330, 430, 640, 800, 920 };

    for (size_t i = 0; i < list.size() && i < 10; i++)
    {
        r.rows.push_back({ to_string(i + 1), list[i].barcode,
            underscoreToSpaces(list[i].name), categoryName(list[i].categoryId),
            to_string(list[i].quantity), list[i].unit });
    }

    r.summary = "Purchase of these products does not need to be started.";

    return r;
}
