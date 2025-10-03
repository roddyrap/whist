#include "AIPlayer.h"
#include <map>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace Whist::Logic
{
    constexpr float AIPlayer::Sigmoid(float x)
    {
        return 1.0f / (1.0f + expf(-x));
    }

    constexpr float AIPlayer::RulingCardSigmoid(const Card& card)
    {
        return (Sigmoid(card.m_number - 11.5f) + 0.1f) / (Sigmoid(2.5f) + 0.1f);
    }

    constexpr float AIPlayer::NonRulingCardSigmoid(const Card& card)
    {
        return Sigmoid(card.m_number - 11.5f) / Sigmoid(3);
    }

    constexpr float AIPlayer::GeneralCardSigmoid(const Card& card)
    {
        return Sigmoid(card.m_number - 11.5f);
    }

    Card AIPlayer::CalculateInitialBet() const
    {
        // Pre-populate the results map with the bets for each suit.
        std::map<eCardSuit, int8_t> results;
        for (eCardSuit suit : BETTING_SUITS)
        {
            results[suit] = 0;
        }

        // Calculate the betting power of each card individually.
        for (const auto& card : m_hand.GetCards())
        {
            // Calculate the relative power of a card for each of the available game states:
            // The card is of the ruling type, it isn't and there's no ruler.
            float cardRulingPower{RulingCardSigmoid(card)};
            float cardNoRulerPower{NonRulingCardSigmoid(card)};
            float cardGeneralPower{GeneralCardSigmoid(card)};

            // How certain the computer is it will be able to utilize this card.
            // TODO: Make not uniform distribution, as the AI player should generally be
            // relatively sure of their cards, so probabilities should center around a half.
            float cardTakeProbability{static_cast <float> (rand()) / static_cast <float> (RAND_MAX)};

            // Add the power of the card to the fitting total bet counters.
            for (auto& betState : results)
            {
                if (betState.first == eCardSuit::NO_TYPE)
                {
                    betState.second += cardTakeProbability <= cardNoRulerPower;
                }
                else if (betState.first == card.m_suit)
                {
                    betState.second += cardTakeProbability <= cardRulingPower;
                }
                else
                {
                    betState.second += cardTakeProbability <= cardGeneralPower;
                }
            }
        }

        // Return the suit with the highest bet.
        auto highest_bet = std::max_element(results.begin(), results.end());
        return Card{highest_bet->first, highest_bet->second};
    }

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

        // Calculate and place most optimal bet.
        Card playerBet{CalculateInitialBet()};
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
            eCardSuit cardSuit{currentCard.m_suit};
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