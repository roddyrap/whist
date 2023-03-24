#include "AIPlayer.h"
#include <map>
#include <algorithm>

namespace Whist::Logic
{
    bool AIPlayer::PlaceInitialBet()
    {
        // If already skipping don't bet again.
        Card previousBet{m_game.GetBets()[m_playerIndex]};
        if (previousBet.m_number == SKIP_BET)
        {
            return true;
        }
        // If calculated bet before try to remain on it.
        else if (previousBet.m_number != 0)
        {
            if (m_game.PlaceBet(m_playerIndex, previousBet) != 0)
            {
                previousBet.m_number = SKIP_BET;
                return m_game.PlaceBet(m_playerIndex, previousBet) == 0;
            }
        }

        // For now, bet the number of cards in a suit you have the most of.
        std::map<eCardSuit, int8_t> countSuits{this->CountSuits()};
        eCardSuit mostNumerousCardSuit{std::max_element(countSuits.begin(), countSuits.end())->first};

        Card playerBet{mostNumerousCardSuit, countSuits[mostNumerousCardSuit]};
        if (m_game.PlaceBet(m_playerIndex, playerBet) != 0)
        {
            playerBet.m_number = SKIP_BET;
            return m_game.PlaceBet(m_playerIndex, playerBet) == 0;
        }

        return true;
    }

    bool AIPlayer::PlaceSecondaryBet()
    {
        eCardSuit rulingType{m_game.GetRulingType()};
        if (rulingType == eCardSuit::NO_TYPE)
        {
            rulingType = eCardSuit::SPADES;
        }

        std::map<eCardSuit, int8_t> countSuits{this->CountSuits()};

        Card playerBet{rulingType, countSuits[rulingType]};

        if (m_game.PlaceBet(m_playerIndex, playerBet) != 0)
        {
            // Try to bet with +1 so sum will not be 13.
            playerBet.m_number++;
            return m_game.PlaceBet(m_playerIndex, playerBet) == 0;
        }

        return true;
    }

    bool AIPlayer::PlaceCard()
    {
        // For now just place the first valid card.
        for (const auto& currentCard : m_hand.GetCards())
        {
            if (m_game.PlaceCard(m_playerIndex, currentCard))
            {
                m_hand.RemoveCard(currentCard);
                return true;
            }
        }

        return false;
    }

    std::map<eCardSuit, int8_t> AIPlayer::CountSuits() const
    {
        std::map<eCardSuit, int8_t> cardCounts{};
        for (const auto& currentCard : m_hand.GetCards())
        {
            eCardSuit cardSuit{currentCard.m_type};
            if (cardCounts.contains(cardSuit))
            {
                cardCounts[cardSuit] += 1;
            }
            else
            {
                cardCounts.emplace(cardSuit, 1);
            }
        }

        return cardCounts;
    }
}