// OrderManager.h
// Order cart, totals, order IDs, bills and Order/Bill files

#pragma once

#include "ProductManager.h"

vector<Order> orders;

Totals calculateTotals(const vector<OrderItem>& items)
{
    Totals totals;

    totals.withoutTax = 0;
    totals.tax = 0;
    totals.discount = 0;

    for (int i = 0; i < items.size(); i++)
    {
        totals.withoutTax += items[i].price * items[i].quantity;
        totals.tax += items[i].tax * items[i].quantity;
        totals.discount += items[i].discount * items[i].quantity;
    }

    totals.payable = totals.withoutTax + totals.tax - totals.discount;

    return totals;
}

int quantityInOrder(const vector<OrderItem>& items, int productId)
{
    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].productId == productId)
            return items[i].quantity;
    }

    return 0;
}

bool addItem(vector<OrderItem>& items, const Product& p,
    int quantity, string& message)
{
    if (quantity <= 0)
    {
        message = "Enter a valid quantity.";
        return false;
    }

    if (quantityInOrder(items, p.id) + quantity > p.quantity)
    {
        message = "Not enough stock. Available: " + to_string(p.quantity);
        return false;
    }

    for (int i = 0; i < items.size(); i++)
    {
        if (items[i].productId == p.id)
        {
            items[i].quantity += quantity;
            message = p.name + " quantity updated.";
            return true;
        }
    }

    if (items.size() >= MAX_ITEMS)
    {
        message = "An order can hold only 8 different products.";
        return false;
    }

    OrderItem item;

    item.productId = p.id;
    item.barcode = p.barcode;
    item.acronym = p.acronym;
    item.price = p.price;
    item.tax = p.tax;
    item.discount = p.discount;
    item.quantity = quantity;

    items.push_back(item);

    message = p.name + " added to the order.";

    return true;
}

bool removeLastItem(vector<OrderItem>& items)
{
    if (items.empty())
        return false;

    items.pop_back();

    return true;
}

string createOrderId()
{
    return padNumber(countLines("Order.txt") + 1, 6) + currentDay();
}

string createBillId()
{
    return padNumber(countLines("Bill.txt") + 1, 9);
}

Order createOrder(string customer, string contact)
{
    Order order;

    order.id = createOrderId();
    order.date = currentDate();
    order.time = currentTime();
    order.customer = spacesToUnderscore(customer);

    if (contact.empty())
        order.contact = "-";
    else
        order.contact = contact;

    order.status = "in-progress";
    order.amount = 0;
    order.mode = "cash";
    order.merchant = "-";

    return order;
}

string orderLine(const Order& o)
{
    return o.id + " " + o.date + " " + o.time + " " + o.customer + " " +
        o.contact + " " + o.status + " " + money(o.amount) + " " +
        o.mode + " " + o.merchant + " #\n";
}

bool appendOrder(const Order& order)
{
    ofstream file("Order.txt", ios::app);

    if (!file.is_open())
        return false;

    file << orderLine(order);

    file.close();

    return true;
}

bool saveOrderDetails(string orderId, const vector<OrderItem>& items)
{
    ofstream file("OrderDetail.txt", ios::app);

    if (!file.is_open())
        return false;

    for (int i = 0; i < items.size(); i++)
    {
        file << orderId << " "
            << items[i].productId << " "
            << items[i].barcode << " "
            << items[i].quantity << " #\n";
    }

    file.close();

    return true;
}

bool saveBill(string billId, const Order& order,
    const vector<OrderItem>& items, const Totals& totals)
{
    ofstream file("Bill.txt", ios::app);

    if (!file.is_open())
        return false;

    file << billId << " " << order.customer << " " << order.id;

    for (int i = 0; i < items.size(); i++)
    {
        file << " " << items[i].acronym
            << " " << money(items[i].price)
            << " " << money(items[i].discount)
            << " " << items[i].quantity;
    }

    file << " " << money(totals.withoutTax)
        << " " << money(totals.tax)
        << " " << money(totals.discount)
        << " " << money(totals.payable)
        << " " << order.status << " #\n";

    file.close();

    return true;
}

bool reduceStock(const vector<OrderItem>& items)
{
    for (int i = 0; i < items.size(); i++)
    {
        for (int j = 0; j < products.size(); j++)
        {
            if (products[j].id == items[i].productId)
                products[j].quantity -= items[i].quantity;
        }
    }

    return saveProducts();
}

bool loadOrders()
{
    ifstream file("Order.txt");

    orders.clear();

    if (!file.is_open())
        return false;

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        Order o;
        string marker;

        if (ss >> o.id
            >> o.date
            >> o.time
            >> o.customer
            >> o.contact
            >> o.status
            >> o.amount
            >> o.mode
            >> o.merchant
            >> marker)
        {
            if (marker == "#")
                orders.push_back(o);
        }
    }

    file.close();

    return true;
}

bool saveOrders()
{
    ofstream file("Order.txt");

    if (!file.is_open())
        return false;

    for (int i = 0; i < orders.size(); i++)
        file << orderLine(orders[i]);

    file.close();

    return true;
}

float totalSales()
{
    float total = 0;

    for (int i = 0; i < orders.size(); i++)
    {
        if (orders[i].status == "paid" || orders[i].status == "archived")
            total += orders[i].amount;
    }

    return total;
}
