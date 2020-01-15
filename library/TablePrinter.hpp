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
#include <numeric>

namespace trl
{
    enum class LineStyle
    {
        Single, Double, Block
    };
    enum class TextStyle
    {
        Bold = 1, Dim = 2, Italic = 4, Underline = 8, Blink = 16, Reversed = 32, Default = 0
    };
    enum class TextColor
    {
        Black, Red, Green, Yellow, Blue, Magenta, Cyan, Gray, Default
    };
    enum class TextAlignment
    {
        Left, Right, Center, Decimal
    };
    enum class TableMode
    {
        Setup, Printing
    };
    enum class DecimalFormat
    {
        Fixed, Scientific
    };

    inline TextStyle operator|(TextStyle lhs,
                               TextStyle rhs) {
        return static_cast<TextStyle> (
                static_cast<std::underlying_type<TextStyle>::type>(lhs) |
                        static_cast<std::underlying_type<TextStyle>::type>(rhs)
        );
    }

    inline TextStyle operator&(TextStyle lhs,
                               TextStyle rhs) {
        return static_cast<TextStyle> (
                static_cast<std::underlying_type<TextStyle>::type>(lhs) &
                        static_cast<std::underlying_type<TextStyle>::type>(rhs)
        );
    }

    /**
     * @brief The TitleProperties struct is used to configure the visual output of the table title.
     */
    struct TitleProperties
    {
        std::string titleText  = "Title";
        TextStyle   titleStyle = TextStyle::Italic | TextStyle::Bold | TextStyle::Reversed;
        TextColor   titleColor = TextColor::Green;

        LineStyle lineStyle  = LineStyle::Double;
        LineStyle frameStyle = LineStyle::Single;

        bool printUpperLine = false;
        bool printLowerLine = false;
        bool printFrame     = true;
    };

    /**
     * @brief The HeaderProperties struct is used to configure the visual output of the table headers.
     */
    struct HeaderProperties
    {
        TextStyle headerTextStyle = TextStyle::Bold | TextStyle::Reversed;
        TextColor headerTextColor = TextColor::Yellow;

        LineStyle lineStyle  = LineStyle::Double;
        LineStyle frameStyle = LineStyle::Single;

        bool printUpperLine = false;
        bool printLowerLine = true;
        bool printFrame     = true;
    };

    struct ColumnProperties
    {
        std::string headerText;

        int columnWidth   = 10;
        int columnMargins = 1;

        TextAlignment textAlignment = TextAlignment::Left;
        TextStyle     textStyle     = TextStyle::Bold | TextStyle::Italic;
        TextColor     textColor     = TextColor::Red;

        LineStyle frameStyle = LineStyle::Single;

        bool printFrame = true;

        DecimalFormat decimalFormat = DecimalFormat::Fixed;
        int           decimalPlaces = 0;
    };

    /**
     * @brief
     */
    struct TableProperties
    {
        TextColor tableElementsColor = TextColor::Default;

        char lineStyle       = '=';
        char columnSeparator = '|';

        bool printTableTitle       = true;
        bool printTableHeader      = true;
        bool printTableFrame       = false;
        bool printColumnSeparators = false;

        int columnMargin = 1;
    };

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
    class TablePrinter
    {
    public:

        /**
         * @brief
         * @param output
         * @param separator
         */
//        explicit TablePrinter(std::ostream& output = std::cout, const std::string& separator = "|")
//                : m_outStream(output),
//                  m_columnSeparator(separator) {
//
//        }

        explicit TablePrinter(std::ostream& output = std::cout,
                              const TableProperties& tableProps = TableProperties(),
                              const TitleProperties& titleProps = TitleProperties(),
                              const HeaderProperties& headerProps = HeaderProperties())
                : m_title(*this, titleProps),
                  m_header(*this, headerProps),
                  m_tableProperties(tableProps),
                  m_outStream(output) {

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
         * @param columnTitle
         * @param columnWidth
         */
        void AddColumn(const std::string& columnTitle,
                       int columnWidth) {

            if (GetTableMode() == TableMode::Printing)
                throw std::runtime_error("Cannot add columns while table is in printing mode.");

            if (columnWidth < 4)
                throw std::invalid_argument("Column width has to be >= 4");

            ColumnProperties colProps;
            colProps.headerText  = columnTitle;
            colProps.columnWidth = columnWidth;
            m_columns.emplace_back(Column(*this, colProps));
            m_currentColumn = m_columns.begin();
        }

        void Begin() {

            SetTableMode(TableMode::Printing);

            m_outStream << "\r";
            if (m_tableProperties.printTableTitle)
                m_title.Print(m_outStream);
            if (m_tableProperties.printTableHeader)
                m_header.Print(m_outStream);

        }

        void End() {

            SetTableMode(TableMode::Setup);
        }

        int TableWidth() const {

            int result = 0;
            for (auto& col : m_columns)
                result += col.ColumnWidth();

            return result;

//            int result = std::accumulate(m_columnWidths.begin(), m_columnWidths.end(), 0);
//            result += m_columnWidths.size() * (2 * m_tableProperties.columnMargin);
//
//            if (m_tableProperties.printColumnSeparators) result += m_columnWidths.size() - 1;
//
//            return result;
        }


//        void PrintTitle(const std::string& title) {

        //auto totalWidth = 0;
        //for (auto& it : m_columnWidths) totalWidth += it;
        //totalWidth += m_columnWidths.size() - 1;

//            auto tit        = title;
//            auto titleWidth = tit.length();
//            if (titleWidth > TableWidth()) tit = tit.substr(0, TableWidth());
//
//            auto pre  = (TableWidth() - tit.length()) / 2;
//            auto post = (TableWidth() - tit.length() - pre);
//
//            PrintHorizontalLine();
//
//            if (m_tableProperties.printTableFrame) m_outStream << "|";
//            SetStreamFormat(m_outStream, m_tableProperties.titleStyle, m_tableProperties.titleColor);
//            for (int i = 0; i < pre; ++i) m_outStream << " ";
//            m_outStream << tit;
//            for (int i = 0; i < post; ++i) m_outStream << " ";
//            ResetStreamFormat(m_outStream);
//            if (m_tableProperties.printTableFrame) m_outStream << "|";
//            m_outStream << "\n";

//        }

        /**
         * @brief
         */
/*        void PrintHeader() {

            PrintHorizontalLine();
            if (m_tableProperties.printTableFrame) m_outStream << "|";

            for (int i = 0; i < GetColumnCount(); ++i) {

                //SetStreamFormat(m_outStream, m_tableProperties.headerTextStyle, m_tableProperties.headerTextColor);
                for (int k = 0; k < m_tableProperties.columnMargin; ++k) m_outStream << " ";

                if (m_flushLeft)
                    m_outStream << std::left;
                else
                    m_outStream << std::right;


                m_outStream << std::setw(m_columnWidths.at(i));
                m_outStream << m_columnTitles.at(i).substr(0, m_columnWidths.at(i));


                for (int k = 0; k < m_tableProperties.columnMargin; ++k) m_outStream << " ";

                if (m_tableProperties.printColumnSeparators && i < (GetColumnCount() - 1)) m_outStream << " ";
                //ResetStreamFormat(<#initializer#>);
            }

            if (m_tableProperties.printTableFrame) m_outStream << "|";
            m_outStream << "\n";
            PrintHorizontalLine();
        }*/

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

//            while (m_columnIndex != 0) {
//                *this << "";
//            }
//            return *this;
        }

        /**
         * @brief
         * @tparam T
         * @param input
         * @return
         */
        template<typename T>
        TablePrinter& operator<<(T input) {

            if (GetTableMode() != TableMode::Printing)
                throw std::runtime_error("Table must be in Printing mode.");

            m_currentColumn->PrintData(input);
            NextColumn();


            //if (m_columnIndex == 0 && m_tableProperties.printTableFrame)
            //    m_outStream << "|";

//            if constexpr(std::is_floating_point<T>::value) {
//                OutputDecimalNumber<T>(input);
//            }
//            else {

            //for (int k = 0; k < m_tableProperties.columnMargin; ++k) m_outStream << " ";

            //if (m_flushLeft)
            //    m_outStream << std::left;
            //else
            //    m_outStream << std::right;

            // Leave 3 extra space: One for negative sign, one for zero, one for decimal
            //m_outStream << std::setw(m_columnWidths.at(m_columnIndex));
            //m_outStream << input;

            //for (int k = 0; k < m_tableProperties.columnMargin; ++k) m_outStream << " ";

            //if (m_columnIndex == GetColumnCount() - 1) {
            //    if (m_tableProperties.printTableFrame) m_outStream << "|";
            //    m_outStream << "\n";
            //    m_rowIndex    = m_rowIndex + 1;
            //    m_columnIndex = 0;
            //}
            //else {
            //    m_outStream << m_columnSeparator;
            //    ++m_columnIndex;
            //}
            //}
            return *this;
        }

    private: // ===== Private Member Functions

        void SetTableMode(TableMode mode) {
            m_mode = mode;
        }

        TableMode GetTableMode() const {
            return m_mode;
        }

        static std::string FormatText(const std::string& text,
                                      int width,
                                      TextAlignment alignment) {

            if (text.size() == width)
                return text;
            if (text.size() > width)
                return text.substr(0, width);

            int paddingLeft;
            int paddingRight;

            switch (alignment) {
                case TextAlignment::Right : {
                    paddingLeft  = width - text.size();
                    paddingRight = 0;
                    break;
                }

                case TextAlignment::Left : {
                    paddingLeft  = 0;
                    paddingRight = width - text.size();
                    break;
                }

                case TextAlignment::Decimal : {
                    paddingLeft  = width - text.size();
                    paddingRight = 0;
                    break;
                }

                case TextAlignment::Center : {
                    paddingLeft  = (width - text.size()) / 2;
                    paddingRight = width - text.size() - paddingLeft;
                }
            }

            std::stringstream result;

            for (auto i = 0; i < paddingLeft; ++i)
                result << " ";
            result << text;
            for (auto i = 0; i < paddingRight; ++i)
                result << " ";

            return result.str();
        }

        template<typename Number>
        static std::string FormatNumber(Number number,
                                        int width,
                                        TextAlignment alignment) {

            std::string result;

            static_assert(!std::is_same_v<Number, bool>, "Type 'bool' cannot be formatted!");

            if constexpr (std::is_integral_v<Number>) {
                result = std::to_string(number);

                if (result.size() > width) {
                    result = result.substr(0, width);
                    for (auto& ch : result)
                        ch = '#';
                }
                else {
                    result = FormatText(result, width, alignment);
                }
            }

            return result;
        }

        /**
         * @brief
         */
        class Title
        {
        public:
            Title(const TablePrinter& table,
                  const TitleProperties& properties)
                    : m_table(table),
                      m_properties(properties) {}

            void Print(std::ostream& stream) {

                if (m_properties.printUpperLine)
                    m_table.PrintHorizontalLine();

                auto title      = m_properties.titleText;
                auto titleWidth = title.length();
                if (titleWidth > m_table.TableWidth())
                    title = title.substr(0, m_table.TableWidth());

                auto pre  = (m_table.TableWidth() - title.length()) / 2;
                auto post = (m_table.TableWidth() - title.length() - pre);

                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                SetStreamFormat(stream, m_properties.titleStyle, m_properties.titleColor);
                for (int i = 0; i < pre; ++i)
                    stream << " ";
                stream << title;
                for (int i = 0; i < post; ++i)
                    stream << " ";
                ResetStreamFormat(stream);
                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                stream << "\n";

                if (m_properties.printLowerLine)
                    m_table.PrintHorizontalLine();

            }

        private:
            TitleProperties m_properties;
            const TablePrinter& m_table;
        }; // class Title

        /**
         * @brief
         */
        class Header
        {
        public:
            Header(const TablePrinter& table,
                   const HeaderProperties& properties)
                    : m_table(table),
                      m_properties(properties) {}

            void Print(std::ostream& stream) {

                if (m_properties.printUpperLine)
                    m_table.PrintHorizontalLine();

                //auto title      = m_properties.titleText;
                //auto titleWidth = title.length();
                //if (titleWidth > m_table.TableWidth()) title = title.substr(0, m_table.TableWidth());

                //auto pre  = (m_table. TableWidth() - title.length()) / 2;
                //auto post = (m_table.TableWidth() - title.length() - pre);

                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                SetStreamFormat(stream, m_properties.headerTextStyle, m_properties.headerTextColor);

                for (auto& col : m_table.m_columns)
                    col.PrintHeader();

                //for (int i = 0; i < pre; ++i) stream << " ";
                //stream << title;
                //for (int i = 0; i < post; ++i) stream << " ";
                ResetStreamFormat(stream);
                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                stream << "\n";

                if (m_properties.printLowerLine)
                    m_table.PrintHorizontalLine();

            }

        private:
            HeaderProperties m_properties;
            const TablePrinter& m_table;
        };

        /**
         * @brief
         */
        class Column
        {
        public:
            Column(const TablePrinter& table,
                   const ColumnProperties& properties)
                    : m_table(table),
                      m_properties(properties) {}

            int ColumnWidth() const {
                return (2 * m_properties.columnMargins + m_properties.columnWidth);
            }

            template<typename DataType>
            void PrintData(DataType data) const {

                SetStreamFormat(m_table.m_outStream, m_properties.textStyle, m_properties.textColor);
                PrintMargin();

                if constexpr (std::is_same_v<DataType, bool>)
                    PrintBool(data);
                else if constexpr (std::is_integral_v<DataType>)
                    PrintInteger(data);

                else if constexpr (std::is_floating_point_v<DataType>)
                    PrintFloat(data);

                else
                    m_table.m_outStream << ResizeAndAlign(data);

                PrintMargin();
                ResetStreamFormat(m_table.m_outStream);
            }

            void PrintHeader() const {

                PrintMargin();
                m_table.m_outStream << ResizeAndAlign(m_properties.headerText);
                PrintMargin();
            }

        private: // Private Member Functions

            void PrintBool(bool data) const {
                if (data)
                    m_table.m_outStream << ResizeAndAlign("TRUE");
                else
                    m_table.m_outStream << ResizeAndAlign("FALSE");
            }

            template<typename DataType>
            void PrintInteger(DataType data) const {
                std::stringstream sstr;
                sstr << data;
                auto str = sstr.str();

                if (str.size() > m_properties.columnWidth)
                    for (auto& ch : str)
                        ch = '#';

                m_table.m_outStream << ResizeAndAlign(str);
            }

            template<typename DataType>
            void PrintFloat(DataType data) const {

                if (m_properties.textAlignment == TextAlignment::Decimal) {

                    std::stringstream sstr;
                    m_properties.decimalFormat == DecimalFormat::Fixed ?
                    sstr << std::fixed :
                    sstr << std::scientific;
                    m_properties.decimalPlaces == 0 ?
                    sstr << std::setprecision((m_properties.columnWidth - 1) / 2) :
                    sstr << std::setprecision(m_properties.decimalPlaces);

                    sstr << data;

                    if (sstr.str().size() > m_properties.columnWidth) {
                        sstr.str("");
                        for (int i = 0; i < m_properties.columnWidth; ++i)
                            sstr << '#';
                    }

                    m_table.m_outStream << ResizeAndAlign(sstr.str());
                }
                else {
                    std::stringstream sstr;
                    m_properties.decimalFormat == DecimalFormat::Fixed ?
                    sstr << std::fixed :
                    sstr << std::scientific;
                    m_properties.decimalPlaces == 0 ?
                    sstr << std::setprecision((m_properties.columnWidth - 1) / 2) :
                    sstr << std::setprecision(m_properties.decimalPlaces);

                    sstr << data;

                    if (sstr.str().size() > m_properties.columnWidth) {
                        sstr.str("");
                        for (int i = 0; i < m_properties.columnWidth; ++i)
                            sstr << '#';
                    }

                    m_table.m_outStream << ResizeAndAlign(sstr.str());
                }
            }

            void PrintMargin() const {
                for (int i = 0; i < m_properties.columnMargins; ++i)
                    m_table.m_outStream << " ";
            }

            std::string ResizeAndAlign(const std::string& str) const {

                std::string result;

                if (str.size() > m_properties.columnWidth)
                    result = str.substr(0, m_properties.columnWidth);
                else if (str.size() < m_properties.columnWidth) {
                    int diff       = m_properties.columnWidth - str.size();
                    int leftSpace  = 0;
                    int rightSpace = 0;

                    switch (m_properties.textAlignment) {
                        case TextAlignment::Left : {
                            leftSpace  = 0;
                            rightSpace = diff;
                            break;
                        }

                        case TextAlignment::Right : {
                            leftSpace  = diff;
                            rightSpace = 0;
                            break;
                        }

                        case TextAlignment::Center : {
                            leftSpace  = diff / 2;
                            rightSpace = diff - leftSpace;
                            break;
                        }

                        case TextAlignment::Decimal : {
                            leftSpace  = diff;
                            rightSpace = 0;
                            break;
                        }
                    }

                    for (int i = 0; i < leftSpace; ++i)
                        result += " ";
                    result += str;
                    for (int i = 0; i < rightSpace; ++i)
                        result += " ";

                }
                else
                    result = str;

                return result;
            }

        private:
            ColumnProperties m_properties;
            const TablePrinter& m_table;
        };

        using Columns = std::vector<Column>;

        /**
         * @brief
         * @param character
         */
        void PrintHorizontalLine() const {

            if (m_tableProperties.printTableFrame)
                m_outStream << m_tableProperties.columnSeparator; // the left bar

            for (int i = 0; i < TableWidth(); ++i)
                m_outStream << m_tableProperties.lineStyle;

            if (m_tableProperties.printTableFrame)
                m_outStream << m_tableProperties.columnSeparator; // the right bar

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
//            if (input < 10 * (m_columnWidths.at(m_columnIndex) - 1) || input > 10 * m_columnWidths.at(m_columnIndex)) {
//                std::stringstream string_out;
//                string_out << std::setiosflags(std::ios::fixed) << std::setprecision(m_columnWidths.at(m_columnIndex))
//                           << std::setw(m_columnWidths.at(m_columnIndex)) << input;
//
//                std::string string_rep_of_number = string_out.str();
//
//                string_rep_of_number[m_columnWidths.at(m_columnIndex) - 1] = '*';
//                std::string string_to_print = string_rep_of_number.substr(0, m_columnWidths.at(m_columnIndex));
//                m_outStream << string_to_print;
//            }
//            else {
//
//                // determine what precision we need
//                int precision = m_columnWidths.at(m_columnIndex) - 1; // leave room for the decimal point
//                if (input < 0)
//                    --precision; // leave room for the minus sign
//
//                // leave room for digits before the decimal?
//                if (input < -1 || input > 1) {
//                    int num_digits_before_decimal = 1 + static_cast<int>(log10(std::abs(input)));
//                    precision -= num_digits_before_decimal;
//                }
//                else
//                    precision--; // e.g. 0.12345 or -0.1234
//
//                if (precision < 0)
//                    precision = 0; // don't go negative with precision
//
//                m_outStream << std::setiosflags(std::ios::fixed) << std::setprecision(precision)
//                            << std::setw(m_columnWidths.at(m_columnIndex)) << input;
//            }
//
//            if (m_columnIndex == GetColumnCount() - 1) {
//                m_outStream << "|\n";
//                m_rowIndex    = m_rowIndex + 1;
//                m_columnIndex = 0;
//            }
//            else {
//                m_outStream << m_columnSeparator;
//                m_columnIndex = m_columnIndex + 1;
//            }
        }

        Columns::iterator NextColumn() {

            ++m_currentColumn;

            if (m_currentColumn == m_columns.end()) {
                m_outStream << "\n";
                m_currentColumn = m_columns.begin();
            }

            return m_currentColumn;
        }

    private: //===== Private Static Functions

        static void SetStreamFormat(std::ostream& stream,
                                    TextStyle style,
                                    TextColor color) {

            if ((style & TextStyle::Bold) == TextStyle::Bold)
                stream << rang::style::bold;
            if ((style & TextStyle::Dim) == TextStyle::Dim)
                stream << rang::style::dim;
            if ((style & TextStyle::Italic) == TextStyle::Italic)
                stream << rang::style::italic;
            if ((style & TextStyle::Underline) == TextStyle::Underline)
                stream << rang::style::underline;
            if ((style & TextStyle::Blink) == TextStyle::Blink)
                stream << rang::style::blink;
            if ((style & TextStyle::Reversed) == TextStyle::Reversed)
                stream << rang::style::reversed;

            switch (color) {
                case TextColor::Red :
                    stream << rang::fg::red;
                    break;

                case TextColor::Black :
                    stream << rang::fg::black;
                    break;

                case TextColor::Green :
                    stream << rang::fg::green;
                    break;

                case TextColor::Yellow :
                    stream << rang::fg::yellow;
                    break;

                case TextColor::Blue :
                    stream << rang::fg::blue;
                    break;

                case TextColor::Magenta :
                    stream << rang::fg::magenta;
                    break;

                case TextColor::Cyan :
                    stream << rang::fg::cyan;
                    break;

                case TextColor::Gray :
                    stream << rang::fg::gray;
                    break;

                case TextColor::Default :
                    stream << rang::fg::reset;
                    break;
            }
        }

        static void ResetStreamFormat(std::ostream& stream) {
            stream << rang::style::reset;
            stream << rang::fg::reset;
        }

    private: // ===== Private Member Variables
        Title   m_title;
        Header  m_header;
        Columns m_columns;

        TableProperties m_tableProperties;

        // ===== Internal State Variables
        std::ostream& m_outStream = std::cout; /**< */
        TableMode         m_mode          = TableMode::Setup; /**< */
        Columns::iterator m_currentColumn = m_columns.begin(); /**< */

        //std::vector<std::string> m_columnTitles; /**< */
        //std::vector<int>         m_columnWidths; /**< */
        //std::string              m_columnSeparator; /**< */

        //int m_rowIndex{0}; /**< index of current row */
        //int m_columnIndex{0}; /**< index of current column */

        //int  m_tableWidth{0}; /**< */
        //bool m_flushLeft{false}; /**< */
    };
} // namespace trl

#endif //TABLEPRINTER_HPP
