//
// Created by Troldal on 2019-03-14.
//

#include <TablePrinter.hpp>
#include <cmath>

int main() {

    trl::TablePrinter tp;
    tp.AddColumn("Name", 25);
    tp.AddColumn("Age", 5);
    tp.AddColumn("Position", 30);
    tp.AddColumn("Allowance", 10);

    //tp.PrintTitle("Hello");
    tp.Begin();
    //tp.PrintHeader();
    tp << "Dat Chu" << 1 << "Research Assistant" << -0.00000000001337;
    tp << "John Doe" << 12 << "Too much float" << 125456789.123456789;
    tp << "John Doe" << 123 << "Typical Int" << 1254567894;
    tp << "John Doe" << 1234 << "Typical float" << 1254.36;
    tp << "John Doe" << 12345 << "Too much negative" << -125456789.123456789;
    tp << "John Doe" << 26 << "Exact size int" << 125456789;
    tp << "John Doe" << 26 << "Exact size int" << -12545678;
    tp << "John Doe" << 26 << "Exact size float" << -1254567.8;
    tp << "John Doe" << 26 << "Negative Int" << -1254;
    // tp << "Jane Doe" << trl::endl();
    tp << "Tom Doe" << 123456 << "Student" << -M_PI;
    //tp.PrintFooter();
    tp.End();

    return 0;
}