#ifndef WHIST_LOGIC_HAND_H
#define WHIST_LOGIC_HAND_H

#include "Card.h"
#include "eCardSuit.h"
#include <stdint.h>

#include <vector>

namespace Whist::Logic
{
    constexpr int8_t PLAYER_HAND_SIZE{13};

    // An interface that allows for I/O polymorphism.
    class Hand
    {
    public:
        /**
         * A Comparator for sort operations on cards meant to provide a good
         * hand order.
         *
         * @param firstCard  The first Card to compare.
         * @param secondCard The second Card to compare.
         * @return           True if first is less than second, else false.
         */
        static bool CardSortCompare(const Card& firstCard, const Card& secondCard);

        Hand() = default;
        ~Hand() = default;

        void AddCard(Card newCard);
        void RemoveCard(Card removedCard);
        bool HasCard(const Card& card) const;
        bool HasType(eCardSuit cardSuit) const;
        uint8_t CountCards() const;

        void SortCards(bool inverted = false);
        const std::vector<Card> GetCards() const;

    protected:
        std::vector<Card> m_hand{};
    };
} // namespace Whist::Logic

#endif // WHIST_LOGIC_HAND_H
