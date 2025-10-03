#include "Card.h"
#include "Hand.h"

namespace Whist::Logic
{
    Card::Card(eCardSuit type, card_number_t number) : m_suit{type}, m_number{number}
    {}

    int Card::operator<=>(const Card& other) const
    {
        if (this->m_number == other.m_number)
        {
            return this->m_suit == other.m_suit ? 0 : (this->m_suit > other.m_suit ? 1 : -1);
        }

        return this->m_number > other.m_number ? 1 : -1;
    }

    bool Card::operator==(const Card& other) const
    {
        return this->m_number == other.m_number && this->m_suit == other.m_suit;
    }
}