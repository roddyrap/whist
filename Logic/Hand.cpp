#include "Hand.h"
#include <algorithm>
#include <map>

namespace Whist::Logic
{
    void Hand::AddCard(Card newCard)
    {
        m_hand.push_back(newCard);
    }

    void Hand::RemoveCard(Card removedCard)
    {
        m_hand.erase(std::find(std::begin(m_hand), std::end(m_hand), removedCard));
    }

    bool Hand::HasCard(const Card& card) const
    {
        return std::find(std::begin(m_hand), std::end(m_hand), card) != std::end(m_hand);
    }

    bool Hand::HasType(eCardSuit cardSuit) const
    {
        return std::any_of(std::begin(m_hand), std::end(m_hand), [cardSuit](Card card){ return card.m_suit == cardSuit; });
    }

    uint8_t Hand::CountCards() const
    {
        return m_hand.size();
    }

    const std::vector<Card> Hand::GetCards() const
    {
        return m_hand;
    }

    bool Hand::CardSortCompare(const Card& firstCard, const Card& secondCard)
    {
        // Custom type sort order, Red Black Red Black.
        std::map<eCardSuit, uint8_t> typePrecedence{};

        typePrecedence[eCardSuit::SPADES] = 3;
        typePrecedence[eCardSuit::HEARTS] = 2;
        typePrecedence[eCardSuit::CLUBS] = 1;
        typePrecedence[eCardSuit::DIAMONDS] = 0;

        if (firstCard.m_suit != secondCard.m_suit)
        {
            return typePrecedence[firstCard.m_suit] < typePrecedence[secondCard.m_suit];
        }

        return secondCard.m_number > firstCard.m_number;
    }

    void Hand::SortCards(bool inverted)
    {
        auto augmentedSortingFunction = [inverted](const Card& first, const Card& second){ return CardSortCompare(first, second) ^ inverted; };
        std::sort(m_hand.begin(), m_hand.end(), augmentedSortingFunction);
    }
}
