#ifndef WHIST_TERMINAL_CARD_H
#define WHIST_TERMINAL_CARD_H

#include <Logic/Card.h>

namespace Whist::Terminal
{
    struct TerminalCard : public Logic::Card
    {
        TerminalCard(Logic::eCardSuit suit, card_number_t number) : Card{suit, number}
        {}

        TerminalCard(const Card& card) : Card{card}
        {}

        TerminalCard(Card&& card) : Card{card}
        {}

        operator std::wstring() const;
        friend std::wostream& operator<<(std::wostream& os, const TerminalCard& card);
    };
}

#endif // WHIST_TERMINAL_CARD_H