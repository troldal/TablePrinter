/*
    MIT License

    Copyright (c) 2017 Dat Chu
    Copyright (c) 2019 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

 */

// Adapted from TablePrinter by (https://github.com/dattanchu/bprinter)


#ifndef TABLEPRINTER_HPP
#define TABLEPRINTER_HPP

#include "rang.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cmath>

namespace trl
{

    /**
     * @brief
     */
    class endl
    {
    };

    /**
     * @brief Print a pretty table into your output of choice.
     *
     * Usage:
     *   TablePrinter tp(&std::cout);
     *   tp.AddColumn("Name", 25);
     *   tp.AddColumn("Age", 3);
     *   tp.AddColumn("Position", 30);
     *
     *   tp.PrintHeader();
     *   tp << "Dat Chu" << 25 << "Research Assistant";
     *   tp << "John Doe" << 26 << "Professional Anonymity";
     *   tp << "Jane Doe" << tp.SkipToNextLine();
     *   tp << "Tom Doe" << 7 << "Student";
     *   tp.PrintFooter();
     *
     * @todo Add support for padding in each table cell
     **/
    class TablePrinter {
    public:

        /**
         * @brief
         * @param output
         * @param separator
         */
        explicit TablePrinter(std::ostream& output = std::cout, const std::string& separator = "|")
                : m_outStream(output),
                  m_columnSeparator(separator) {

        }

        /**
         * @brief
         */
        TablePrinter(const TablePrinter& other) = delete;

        /**
         * @brief
         * @param other
         */
        TablePrinter(TablePrinter&& other) = delete;

        /**
         * @brief
         */
        ~TablePrinter() = default;

        /**
         * @brief
         * @param other
         * @return
         */
        TablePrinter& operator=(const TablePrinter& other) = delete;

        /**
         * @brief
         * @param other
         * @return
         */
        TablePrinter& operator=(TablePrinter&& other) = delete;

        /**
         * @brief
         * @return
         */
        int GetColumnCount() const {

            return m_columnTitles.size();
        }

        /**
         * @brief
         * @return
         */
        int GetTableWidth() const {

            return m_tableWidth;
        }

        /**
         * @brief
         * @param separator
         */
        void SetSeparator(const std::string& separator) {

            m_columnSeparator = separator;
        }

        /**
         * @brief
         */
        void SetFlushLeft() {

            m_flushLeft = true;
        }

        /**
         * @brief
         */
        void SetFlushRight() {

            m_flushLeft = false;
        }

        /**
         * @brief
         * @param columnTitle
         * @param columnWidth
         */
        void AddColumn(const std::string& columnTitle, int columnWidth) {

            if (columnWidth < 4) {
                throw std::invalid_argument("Column width has to be >= 4");
            }

            m_columnTitles.emplace_back(columnTitle);
            m_columnWidths.emplace_back(columnWidth);
            m_tableWidth += columnWidth + m_columnSeparator.size(); // for the separator
        }

        /**
         * @brief
         * @param title
         */
        void PrintTitle(const std::string& title) {

            auto totalWidth = 0;
            for (auto& it : m_columnWidths) totalWidth += it;
            totalWidth += m_columnWidths.size() - 1;

            auto tit        = title;
            auto titleWidth = tit.length();
            if (titleWidth > totalWidth) tit = tit.substr(0, totalWidth);

            auto pre  = (totalWidth - tit.length()) / 2;
            auto post = (totalWidth - tit.length() - pre);

            PrintHorizontalLine('=');
            m_outStream << "|";
            for (int i = 0; i < pre; ++i) m_outStream << " ";
            m_outStream << tit;
            for (int i = 0; i < post; ++i) m_outStream << " ";
            m_outStream << "|\n";

        }

        /**
         * @brief
         */
        void PrintHeader() {

            PrintHorizontalLine('=');
            m_outStream << "|";

            for (int i = 0; i < GetColumnCount(); ++i) {

                if (m_flushLeft)
                    m_outStream << std::left;
                else
                    m_outStream << std::right;

                m_outStream << std::setw(m_columnWidths.at(i)) << m_columnTitles.at(i).substr(0, m_columnWidths.at(i));
                if (i != GetColumnCount() - 1) {
                    m_outStream << m_columnSeparator;
                }
            }

            m_outStream << "|\n";
            PrintHorizontalLine('=');
        }

        /**
         * @brief
         */
        void PrintFooter() {

            PrintHorizontalLine();
        }

        /**
         *
         */
        TablePrinter& operator<<(endl input) {

            while (m_columnIndex != 0) {
                *this << "";
            }
            return *this;
        }

        /**
         * @brief
         * @tparam T
         * @param input
         * @return
         */
        template<typename T>
        TablePrinter& operator<<(T input) {

            if constexpr(std::is_floating_point<T>::value) {
                OutputDecimalNumber<T>(input);
            }
            else {

                if (m_columnIndex == 0)
                    m_outStream << "|";

                if (m_flushLeft)
                    m_outStream << std::left;
                else
                    m_outStream << std::right;

                // Leave 3 extra space: One for negative sign, one for zero, one for decimal
                m_outStream << std::setw(m_columnWidths.at(m_columnIndex));
                m_outStream << input;

                if (m_columnIndex == GetColumnCount() - 1) {
                    m_outStream << "|\n";
                    m_rowIndex    = m_rowIndex + 1;
                    m_columnIndex = 0;
                }
                else {
                    m_outStream << m_columnSeparator;
                    ++m_columnIndex;
                }
            }
            return *this;
        }

    private:

        /**
         * @brief
         * @param character
         */
        void PrintHorizontalLine(char character = '-') {

            m_outStream << "+"; // the left bar

            for (int i = 0; i < m_tableWidth - 1; ++i)
                m_outStream << character;

            m_outStream << "+"; // the right bar
            m_outStream << "\n";
        }

        /**
         * @brief
         * @tparam T
         * @param input
         */
        template<typename T>
        void OutputDecimalNumber(T input) {
            // If we cannot handle this number, indicate so
            if (input < 10 * (m_columnWidths.at(m_columnIndex) - 1) || input > 10 * m_columnWidths.at(m_columnIndex)) {
                std::stringstream string_out;
                string_out << std::setiosflags(std::ios::fixed) << std::setprecision(m_columnWidths.at(m_columnIndex))
                           << std::setw(m_columnWidths.at(m_columnIndex)) << input;

                std::string string_rep_of_number = string_out.str();

                string_rep_of_number[m_columnWidths.at(m_columnIndex) - 1] = '*';
                std::string string_to_print = string_rep_of_number.substr(0, m_columnWidths.at(m_columnIndex));
                m_outStream << string_to_print;
            }
            else {

                // determine what precision we need
                int precision = m_columnWidths.at(m_columnIndex) - 1; // leave room for the decimal point
                if (input < 0)
                    --precision; // leave room for the minus sign

                // leave room for digits before the decimal?
                if (input < -1 || input > 1) {
                    int num_digits_before_decimal = 1 + static_cast<int>(log10(std::abs(input)));
                    precision -= num_digits_before_decimal;
                }
                else
                    precision--; // e.g. 0.12345 or -0.1234

                if (precision < 0)
                    precision = 0; // don't go negative with precision

                m_outStream << std::setiosflags(std::ios::fixed) << std::setprecision(precision)
                            << std::setw(m_columnWidths.at(m_columnIndex)) << input;
            }

            if (m_columnIndex == GetColumnCount() - 1) {
                m_outStream << "|\n";
                m_rowIndex    = m_rowIndex + 1;
                m_columnIndex = 0;
            }
            else {
                m_outStream << m_columnSeparator;
                m_columnIndex = m_columnIndex + 1;
            }
        }

        std::ostream& m_outStream; /**< */
        std::vector<std::string> m_columnTitles; /**< */
        std::vector<int>         m_columnWidths; /**< */
        std::string              m_columnSeparator; /**< */

        int m_rowIndex{0}; /**< index of current row */
        int m_columnIndex{0}; /**< index of current column */

        int  m_tableWidth{0}; /**< */
        bool m_flushLeft{false}; /**< */
    };
} // namespace trl

#endif //TABLEPRINTER_HPP
