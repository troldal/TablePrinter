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
#include <functional>

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
     * @brief
     */
    struct TableFormat
    {
        TextColor frameColor = TextColor::Default;

        char lineStyle       = '-';
        char columnSeparator = '|';

        bool printTitle  = false;
        bool printHeader = true;
        bool printFrame  = false;
    };

    /**
     * @brief The TitleProperties struct is used to configure the visual output of the table title.
     */
    struct TitleFormat
    {
        std::string text  = "";
        TextStyle   style = TextStyle::Default;
        TextColor   color = TextColor::Default;

        bool printFrame = true;
    };

    /**
     * @brief The HeaderProperties struct is used to configure the visual output of the table headers.
     */
    struct HeaderFormat
    {
        TextStyle style = TextStyle::Default;
        TextColor color = TextColor::Default;

        bool printFrame = true;
    };

    struct ColumnFormat
    {
        std::string headerText;

        int width   = 10;
        int margins = 1;

        TextAlignment alignment = TextAlignment::Left;
        TextStyle     style     = TextStyle::Default;
        TextColor     color     = TextColor::Default;

        bool printFrame = true;

        DecimalFormat decimalFormat    = DecimalFormat::Fixed;
        int           decimalPlaces    = 0;
        bool          showIntsAsFloats = false;
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
     **/
    class TablePrinter
    {
    public:

        /**
         * @brief
         * @param output
         * @param separator
         */
        explicit TablePrinter(std::ostream& output = std::cout,
                              const TableFormat& tableProps = TableFormat(),
                              const TitleFormat& titleProps = TitleFormat(),
                              const HeaderFormat& headerProps = HeaderFormat())
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

        void AddColumn(const ColumnFormat& colProps) {

            if (GetTableMode() == TableMode::Printing)
                throw std::runtime_error("Cannot add columns while table is in printing mode.");

            if (colProps.width < 4)
                throw std::invalid_argument("Column width has to be >= 4");

            m_columns.emplace_back(Column(*this, colProps));
            m_currentColumn = m_columns.begin();

        }

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

            ColumnFormat colProps;
            colProps.headerText = columnTitle;
            colProps.width      = columnWidth;
            m_columns.emplace_back(Column(*this, colProps));
            m_currentColumn = m_columns.begin();
        }

        void Begin() {

            SetTableMode(TableMode::Printing);

            m_outStream << "\r";
            if (m_tableProperties.printTitle)
                m_title.Print(m_outStream);
            if (m_tableProperties.printHeader)
                m_header.Print(m_outStream);

            PrintHorizontalLine();
        }

        void End() {

            PrintHorizontalLine();
            SetTableMode(TableMode::Setup);
        }

        int TableWidth() const {

            int result = 0;
            for (auto& col : m_columns)
                result += col.ColumnWidth();

            return result;
        }

        /**
         *
         */
        TablePrinter& operator<<(endl input) {

            *this << "";
            while (m_currentColumn != m_columns.begin())
                *this << "";
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

            if (GetTableMode() != TableMode::Printing)
                throw std::runtime_error("Table must be in Printing mode.");

            m_currentColumn->PrintData(input);
            NextColumn();

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

        /**
         * @brief
         */
        class Title
        {
        public:
            Title(const TablePrinter& table,
                  const TitleFormat& properties)
                    : m_table(table),
                      m_properties(properties) {}

            void Print(std::ostream& stream) {

                auto title      = m_properties.text;
                auto titleWidth = title.length();
                if (titleWidth > m_table.TableWidth())
                    title = title.substr(0, m_table.TableWidth());

                auto pre  = (m_table.TableWidth() - title.length()) / 2;
                auto post = (m_table.TableWidth() - title.length() - pre);

                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                SetStreamFormat(stream, m_properties.style, m_properties.color);
                for (int i = 0; i < pre; ++i)
                    stream << " ";
                stream << title;
                for (int i = 0; i < post; ++i)
                    stream << " ";
                ResetStreamFormat(stream);
                //if (m_tableProperties.printTableFrame) m_outStream << "|";
                stream << "\n";
            }

        private:
            TitleFormat m_properties;
            const TablePrinter& m_table;
        }; // class Title

        /**
         * @brief
         */
        class Header
        {
        public:
            Header(const TablePrinter& table,
                   const HeaderFormat& properties)
                    : m_table(table),
                      m_properties(properties) {}

            void Print(std::ostream& stream) {

                SetStreamFormat(stream, m_properties.style, m_properties.color);

                for (auto& col : m_table.m_columns)
                    col.PrintHeader();

                ResetStreamFormat(stream);

                stream << "\n";
            }

        private:
            HeaderFormat m_properties;
            const TablePrinter& m_table;
        };

        /**
         * @brief
         */
        class Column
        {
        public:
            Column(const TablePrinter& table,
                   const ColumnFormat& properties)
                    : m_table(table),
                      m_properties(properties) {}

            int ColumnWidth() const {
                return (2 * m_properties.margins + m_properties.width);
            }

            template<typename DataType>
            void PrintData(DataType data) const {

                SetStreamFormat(m_table.m_outStream, m_properties.style, m_properties.color);
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
                if (m_properties.showIntsAsFloats) {
                    PrintFloat(static_cast<double>(data));
                    return;
                }

                std::stringstream sstr;
                sstr << data;
                auto str = sstr.str();

                if (str.size() > m_properties.width)
                    for (auto& ch : str)
                        ch = '#';

                m_table.m_outStream << ResizeAndAlign(str);
            }

            template<typename DataType>
            void PrintFloat(DataType data) const {

                if (m_properties.alignment == TextAlignment::Decimal) {

                    std::stringstream sstr;
                    m_properties.decimalFormat == DecimalFormat::Fixed ?
                    sstr << std::fixed :
                    sstr << std::scientific;
                    m_properties.decimalPlaces == 0 ?
                    sstr << std::setprecision((m_properties.width - 1) / 2) :
                    sstr << std::setprecision(m_properties.decimalPlaces);

                    sstr << data;

                    if (sstr.str().size() > m_properties.width) {
                        sstr.str("");
                        for (int i = 0; i < m_properties.width; ++i)
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
                    sstr << std::setprecision((m_properties.width - 1) / 2) :
                    sstr << std::setprecision(m_properties.decimalPlaces);

                    sstr << data;

                    if (sstr.str().size() > m_properties.width) {
                        sstr.str("");
                        for (int i = 0; i < m_properties.width; ++i)
                            sstr << '#';
                    }

                    m_table.m_outStream << ResizeAndAlign(sstr.str());
                }
            }

            void PrintMargin() const {
                for (int i = 0; i < m_properties.margins; ++i)
                    m_table.m_outStream << " ";
            }

            std::string ResizeAndAlign(const std::string& str) const {

                std::string result;

                if (str.size() > m_properties.width)
                    result = str.substr(0, m_properties.width);
                else if (str.size() < m_properties.width) {
                    int diff       = m_properties.width - str.size();
                    int leftSpace  = 0;
                    int rightSpace = 0;

                    switch (m_properties.alignment) {
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
            ColumnFormat m_properties;
            const TablePrinter& m_table;
        };

        using Columns = std::vector<Column>;

        /**
         * @brief
         * @param character
         */
        void PrintHorizontalLine() const {

            if (m_tableProperties.printFrame)
                m_outStream << m_tableProperties.columnSeparator; // the left bar

            for (int i = 0; i < TableWidth(); ++i)
                m_outStream << m_tableProperties.lineStyle;

            if (m_tableProperties.printFrame)
                m_outStream << m_tableProperties.columnSeparator; // the right bar

            m_outStream << "\n";
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

        TableFormat m_tableProperties;

        // ===== Internal State Variables
        std::ostream& m_outStream = std::cout; /**< */
        TableMode         m_mode          = TableMode::Setup; /**< */
        Columns::iterator m_currentColumn = m_columns.begin(); /**< */
    };
} // namespace trl

#endif //TABLEPRINTER_HPP
