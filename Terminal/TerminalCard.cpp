#include "TerminalCard.h"

#include "TerminalUtils.h"

#include <wchar.h>
#include <sstream>

namespace Whist::Terminal
{
    TerminalCard::operator std::wstring() const
    {
        // Invalid card.
        if (m_number == 0)
        {
            return L"N/A";
        }

        std::wstring type_symbol = CardSuitToWString(m_type);

        std::wstringstream strStream{};
        strStream << type_symbol;

        if (m_number == 11)
        {
            strStream << L'J';
        }
        else if (m_number == 12)
        {
            strStream << L'Q';
        }
        else if (m_number == 13)
        {
            strStream << L'K';
        }
        else if (m_number == 14)
        {
            strStream << L'A';
        }
        else
        {
            strStream << std::to_wstring(m_number);
        }

        return strStream.str();
    }

    std::wostream& operator<<(std::wostream& os, const TerminalCard& card)
    {
        os << static_cast<std::wstring>(card);
        return os;
    }
}