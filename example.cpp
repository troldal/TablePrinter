//
// Created by Troldal on 2019-03-14.
//

#include "tableprinter.h"

int main() {
    TablePrinter::TablePrinter tp(&std::cout);
    tp.AddColumn("Name", 25);
    tp.AddColumn("Age", 5);
    tp.AddColumn("Position", 30);
    tp.AddColumn("Allowance", 9);

    tp.PrintHeader();
    tp << "Dat Chu" << 25 << "Research Assistant" << -0.00000000001337;
    tp << "John Doe" << 26 << "Too much float" << 125456789.123456789;
    tp << "John Doe" << 26 << "Typical Int" << 1254;
    tp << "John Doe" << 26 << "Typical float" << 1254.36;
    tp << "John Doe" << 26 << "Too much negative" << -125456789.123456789;
    tp << "John Doe" << 26 << "Exact size int" << 125456789;
    tp << "John Doe" << 26 << "Exact size int" << -12545678;
    tp << "John Doe" << 26 << "Exact size float" << -1254567.8;
    tp << "John Doe" << 26 << "Negative Int" << -1254;
    tp << "Jane Doe" << TablePrinter::endl();
    tp << "Tom Doe" << 7 << "Student" << -M_PI;
    tp.PrintFooter();

    return 0;
}