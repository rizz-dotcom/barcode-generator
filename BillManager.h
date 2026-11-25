// BillManager.h
// Bills (Bill.txt), bill lines (BillDetail.txt) and printing bills as PDF
// Bill.txt line:
// BillId Customer OrderId WithoutTax Tax Discount Payable Received Balance
// Status Seasonal Special Date Time #
// BillDetail.txt line:
// BillId Acronym UnitPrice Quantity Discount #

#pragma once

#include "OrderManager.h"

vector<Bill> bills;
vector<BillDetail> billDetails;

// ---------- Bill.txt ----------

string billLine(const Bill& b)
{
    return b.id + " " + b.customer + " " + b.orderId + " " +
        money(b.withoutTax) + " " + money(b.tax) + " " +
        money(b.discount) + " " + money(b.payable) + " " +
        money(b.received) + " " + money(b.balance) + " " +
        b.status + " " + money(b.seasonal) + " " + money(b.special) + " " +
        b.date + " " + b.time + " #";
}

bool loadBills()
{
    bills.clear();

    if (!fileExists(BILL_FILE))
        return false;

    vector<vector<string>> records = readRecords(BILL_FILE);

    for (size_t i = 0; i < records.size(); i++)
    {
        vector<string>& r = records[i];

        // Old (increment 2) bill lines have a different layout: skip them
        if (r.size() != 14)
            continue;

        Bill b;

        b.id = r[0];
        b.customer = r[1];
        b.orderId = r[2];
        b.withoutTax = toFloat(r[3]);
        b.tax = toFloat(r[4]);
        b.discount = toFloat(r[5]);
        b.payable = toFloat(r[6]);
        b.received = toFloat(r[7]);
        b.balance = toFloat(r[8]);
        b.status = r[9];
        b.seasonal = toFloat(r[10]);
        b.special = toFloat(r[11]);
        b.date = r[12];
        b.time = r[13];

        bills.push_back(b);
    }

    return true;
}

bool saveBills()
{
    vector<string> lines;

    for (size_t i = 0; i < bills.size(); i++)
        lines.push_back(billLine(bills[i]));

    return writeLines(BILL_FILE, lines);
}

int findBillIndex(string billId)
{
    for (size_t i = 0; i < bills.size(); i++)
    {
        if (bills[i].id == billId)
            return (int)i;
    }

    return -1;
}

int findBillIndexByOrder(string orderId)
{
    for (size_t i = 0; i < bills.size(); i++)
    {
        if (bills[i].orderId == orderId)
            return (int)i;
    }

    return -1;
}

// Next unique 9-digit bill id (largest id in Bill.txt + 1)
string nextBillId()
{
    loadBills();

    long long maxId = 0;

    for (size_t i = 0; i < bills.size(); i++)
        maxId = std::max(maxId, toLong(bills[i].id));

    return padNumber(maxId + 1, 9);
}

// Adds a new bill, or replaces the bill with the same id
bool saveOrReplaceBill(const Bill& bill)
{
    loadBills();

    int index = findBillIndex(bill.id);

    if (index >= 0)
        bills[index] = bill;
    else
        bills.push_back(bill);

    return saveBills();
}

// When an order is archived, its bill status changes too
bool updateBillStatus(string orderId, string status)
{
    loadBills();

    int index = findBillIndexByOrder(orderId);

    if (index < 0)
        return true;

    bills[index].status = status;

    return saveBills();
}

// ---------- BillDetail.txt ----------

bool loadBillDetails()
{
    billDetails.clear();

    if (!fileExists(BILL_DETAIL_FILE))
        return false;

    vector<vector<string>> records = readRecords(BILL_DETAIL_FILE);

    for (size_t i = 0; i < records.size(); i++)
    {
        vector<string>& r = records[i];

        if (r.size() < 5)
            continue;

        BillDetail d;

        d.billId = r[0];
        d.acronym = r[1];
        d.unitPrice = toFloat(r[2]);
        d.quantity = toInt(r[3]);
        d.discount = toFloat(r[4]);

        billDetails.push_back(d);
    }

    return true;
}

bool replaceBillDetails(string billId, const vector<OrderItem>& items)
{
    loadBillDetails();

    vector<string> lines;

    for (size_t i = 0; i < billDetails.size(); i++)
    {
        BillDetail& d = billDetails[i];

        if (d.billId != billId)
        {
            lines.push_back(d.billId + " " + d.acronym + " " +
                money(d.unitPrice) + " " + to_string(d.quantity) + " " +
                money(d.discount) + " #");
        }
    }

    for (size_t i = 0; i < items.size(); i++)
    {
        lines.push_back(billId + " " + items[i].acronym + " " +
            money(items[i].price) + " " + to_string(items[i].quantity) + " " +
            money(items[i].discount * items[i].quantity) + " #");
    }

    bool ok = writeLines(BILL_DETAIL_FILE, lines);

    loadBillDetails();

    return ok;
}

// ---------- PDF ----------

string pdfEscape(const string& text)
{
    string result;

    for (size_t i = 0; i < text.length(); i++)
    {
        char c = text[i];

        if (c == '(' || c == ')' || c == '\\')
            result += '\\';

        if (c >= 32 && c <= 126)
            result += c;
    }

    return result;
}

// Writes a simple one-page PDF (Courier font, one string per line)
bool writeSimplePdf(string path, const vector<string>& lines)
{
    ensureFolder(fs::path(path).parent_path().string());

    ostringstream content;

    content << "BT\n/F1 11 Tf\n14 TL\n50 790 Td\n";

    for (size_t i = 0; i < lines.size() && i < 54; i++)
        content << "(" << pdfEscape(lines[i]) << ") Tj T*\n";

    content << "ET\n";

    string stream = content.str();

    vector<string> objects =
    {
        "<< /Type /Catalog /Pages 2 0 R >>",
        "<< /Type /Pages /Kids [3 0 R] /Count 1 >>",
        "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595 842] "
        "/Resources << /Font << /F1 4 0 R >> >> /Contents 5 0 R >>",
        "<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>",
        "<< /Length " + to_string(stream.size()) + " >>\nstream\n" +
        stream + "endstream"
    };

    string pdf = "%PDF-1.4\n";
    vector<size_t> offsets;

    for (size_t i = 0; i < objects.size(); i++)
    {
        offsets.push_back(pdf.size());
        pdf += to_string(i + 1) + " 0 obj\n" + objects[i] + "\nendobj\n";
    }

    size_t xref = pdf.size();

    pdf += "xref\n0 " + to_string(objects.size() + 1) + "\n";
    pdf += "0000000000 65535 f \n";

    for (size_t i = 0; i < offsets.size(); i++)
        pdf += padNumber((long long)offsets[i], 10) + " 00000 n \n";

    pdf += "trailer\n<< /Size " + to_string(objects.size() + 1) +
        " /Root 1 0 R >>\nstartxref\n" + to_string(xref) + "\n%%EOF\n";

    // binary mode so Windows does not change \n to \r\n
    ofstream file(path, ios::binary);

    if (!file.is_open())
        return false;

    file << pdf;

    return true;
}

string billPdfPath(string billId)
{
    return BILL_FOLDER + "/Bill_" + billId + ".pdf";
}

// Builds the PDF of a saved bill. Returns the file path ("" on error).
string createBillPdf(string billId)
{
    loadBills();
    loadBillDetails();
    loadOrders();

    int index = findBillIndex(billId);

    if (index < 0)
        return "";

    Bill& b = bills[index];

    Order order;
    int orderIndex = findOrderIndex(b.orderId);

    if (orderIndex >= 0)
        order = orders[orderIndex];

    string line(64, '-');
    vector<string> lines;

    lines.push_back("                         SMART POS");
    lines.push_back("                Barcode Point of Sale System");
    lines.push_back(line);
    lines.push_back(padRight("Bill ID  : " + b.id, 34) + "Date : " + b.date);
    lines.push_back(padRight("Order ID : " + b.orderId, 34) + "Time : " + b.time);
    lines.push_back(padRight("Customer : " + underscoreToSpaces(b.customer), 34) +
        "Contact : " + order.contact);
    lines.push_back(padRight("Payment  : " + order.mode, 34) +
        "Merchant : " + underscoreToSpaces(order.merchant));
    lines.push_back(line);
    lines.push_back(padRight("Item", 12) + padLeft("Unit Price", 12) +
        padLeft("Qty", 7) + padLeft("Discount", 13) + padLeft("Amount", 14));
    lines.push_back(line);

    for (size_t i = 0; i < billDetails.size(); i++)
    {
        BillDetail& d = billDetails[i];

        if (d.billId != b.id)
            continue;

        lines.push_back(padRight(d.acronym, 12) +
            padLeft(money(d.unitPrice), 12) +
            padLeft(to_string(d.quantity), 7) +
            padLeft(money(d.discount), 13) +
            padLeft(money(d.unitPrice * d.quantity), 14));
    }

    lines.push_back(line);
    lines.push_back(padRight("Total without tax", 44) + padLeft(money(b.withoutTax), 14));
    lines.push_back(padRight("Total tax", 44) + padLeft(money(b.tax), 14));
    lines.push_back(padRight("Product discounts", 44) + padLeft("-" + money(b.discount), 14));
    lines.push_back(padRight("Seasonal discount", 44) + padLeft("-" + money(b.seasonal), 14));
    lines.push_back(padRight("Special discount", 44) + padLeft("-" + money(b.special), 14));
    lines.push_back(line);
    lines.push_back(padRight("TOTAL PAYABLE (PKR)", 44) + padLeft(money(b.payable), 14));
    lines.push_back(padRight("Amount received", 44) + padLeft(money(b.received), 14));
    lines.push_back(padRight("Change returned", 44) + padLeft(money(b.balance), 14));
    lines.push_back(line);
    lines.push_back("Status: " + b.status);
    lines.push_back("");
    lines.push_back("                 Thank you for shopping with us!");

    string path = billPdfPath(b.id);

    if (!writeSimplePdf(path, lines))
        return "";

    return path;
}

// Opens a file with its normal program (Windows only)
void openFile(string path)
{
#ifdef _WIN32
    string fullPath = fs::absolute(path).make_preferred().string();
    string command = "start \"\" \"" + fullPath + "\"";
    system(command.c_str());
#else
    cout << "Saved: " << path << "\n";
#endif
}
