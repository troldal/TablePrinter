//
// Created by Troldal on 2019-03-14.
//

#include <TablePrinter.hpp>
#include <cmath>
#include <sstream>

void SimpleTable();
void IntermediateTable();
void AdvancedTable();

template<typename Data>
std::string AsMoney(Data data) {

    std::stringstream buffer;

    buffer.imbue(std::locale("en_us"));
    buffer << std::showbase << std::put_money(data);
    return buffer.str();
}

int main() {

    SimpleTable();
    std::cout << std::endl;
    IntermediateTable();

    return 0;
}

void SimpleTable() {

    trl::TablePrinter tp;

    tp.AddColumn("Name", 25);
    tp.AddColumn("Age", 5);
    tp.AddColumn("Position", 30);
    tp.AddColumn("Allowance", 20);

    tp.Begin();
    tp << "Dat Chu" << 1 << "Research Assistant" << -0.00000000001337;
    tp << "John Doe" << 12 << "Too much float" << 125456789.123456789;
    tp << "John Doe" << 123 << "Typical Int" << 1254567894;
    tp << "John Doe" << 1234 << "Typical float" << 1254.36;
    tp << "John Doe" << 12345 << "Too much negative" << -125456789.123456789;
    tp << "John Doe" << 26 << "Exact size int" << 125456789;
    tp << "John Doe" << 26 << "Exact size int" << -12545678;
    tp << "John Doe" << 26 << "Exact size float" << -1254567.8;
    tp << "John Doe" << 26 << "Negative Int" << -1254;
    tp << "Jane Doe" << trl::endl();
    tp << "Tom Doe" << 123456 << "Student" << -M_PI;
    tp.End();
}

void IntermediateTable() {

    trl::TableFormat table;
    table.printTitle = true;

    trl::TitleFormat title;
    title.text  = "Intermediate Table";
    title.style = trl::TextStyle::Bold | trl::TextStyle::Reversed;
    title.color = trl::TextColor::Blue;

    trl::HeaderFormat header;
    header.style = trl::TextStyle::Bold;
    header.color = trl::TextColor::Yellow;

    trl::ColumnFormat colName;
    colName.style      = trl::TextStyle::Bold;
    colName.color      = trl::TextColor::Yellow;
    colName.headerText = "Name";
    colName.width      = 25;

    trl::ColumnFormat colAge;
    colAge.alignment  = trl::TextAlignment::Right;
    colAge.color      = trl::TextColor::Red;
    colAge.headerText = "Age";
    colAge.width      = 5;

    trl::ColumnFormat colPosition;
    colPosition.color      = trl::TextColor::Cyan;
    colPosition.headerText = "Position";
    colPosition.width      = 30;

    trl::ColumnFormat colAllowance;
    colAllowance.alignment        = trl::TextAlignment::Decimal;
    colAllowance.decimalFormat    = trl::DecimalFormat::Fixed;
    colAllowance.decimalPlaces    = 4;
    colAllowance.showIntsAsFloats = true;
    colAllowance.color            = trl::TextColor::Green;
    colAllowance.headerText       = "Allowance";
    colAllowance.width            = 10;

    trl::TablePrinter tp(std::cout, table, title, header);

    tp.AddColumn(colName);
    tp.AddColumn(colAge);
    tp.AddColumn(colPosition);
    tp.AddColumn(colAllowance);

    tp.Begin();
    tp << "Dat Chu" << 1 << "Research Assistant" << -0.00000000001337;
    tp << "John Doe" << 12 << "Too much float" << 125456789.123456789;
    tp << "John Doe" << 123 << "Typical Int" << 1254567894;
    tp << "John Doe" << 1234 << "Typical float" << 1254.36;
    tp << "John Doe" << 12345 << "Too much negative" << -125456789.123456789;
    tp << "John Doe" << 26 << "Exact size int" << 125456789;
    tp << "John Doe" << 26 << "Exact size int" << -12545678;
    tp << "John Doe" << 26 << "Exact size float" << -1254567.8;
    tp << "John Doe" << 26 << "Negative Int" << -1254;
    tp << "John Doe" << 26 << "Negative Int" << 1254;
    tp << "John Doe" << 26 << "Negative Int" << AsMoney(-1254);
    tp << "John Doe" << 26 << "Negative Int" << AsMoney(9876543210);
    tp << "Jane Doe" << trl::endl();
    tp << "Tom Doe" << 123456 << "Student" << -M_PI;
    tp.End();
}

void AdvancedTable() {

}