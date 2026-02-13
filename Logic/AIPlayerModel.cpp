#include "AIPlayerModel.h"
#include <map>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

namespace Whist::Logic
{
    AIPlayerModel::AIPlayerModel(Game& game, uint8_t playerIndex) :
        m_game{game}, m_hand{game.GetMutableHand(playerIndex)}, m_playerIndex{playerIndex}
    {}

    Card AIPlayerModel::GetAction()
    {
        if (m_game.GetGameState() == eGameState::INITIAL_BETTING)
        {
            Card playerPreviousBet = m_game.GetBets()[m_playerIndex];
            if (playerPreviousBet.m_number != 0 && m_game.IsBetValid(m_playerIndex, playerPreviousBet) == Game::ACTION_SUCCESS) return playerPreviousBet;

            Card highestBet = this->CalculateInitialBet();
            if (m_game.IsBetValid(m_playerIndex, highestBet) == Game::ACTION_SUCCESS) return highestBet;

            return Card{eCardSuit::SPADES, SKIP_BET};
        }
        else if (m_game.GetGameState() == eGameState::SECONDARY_BETTING)
        {
            // TODO: Dogshit logic.
            eCardSuit rulingType{*m_game.GetRulingSuit()};
            std::map<eCardSuit, int8_t> countSuits{this->CountSuits()};
            Card playerBet{rulingType, countSuits[rulingType]};

            if (m_game.IsBetValid(m_playerIndex, playerBet) == Game::BET_WILL_COMPLETE_TO_HAND_SIZE)
            {
                playerBet.m_number++;
            }

            return playerBet;
        }
        else if (m_game.GetGameState() == eGameState::ROUNDS)
        {
            // TODO: Also dogshit logic.
            //       For now just place the first valid card.
            for (const auto& currentCard : m_hand.GetCards())
            {
                if (m_game.IsPlayValid(m_playerIndex, currentCard) == Game::ACTION_SUCCESS)
                {
                    return currentCard;
                }
            }
        }

        // Should never happen, every player has at least one valid card per round and all states
        // have been accounted for.
        return Card{eCardSuit::NO_TYPE, 0};
    }

    constexpr float AIPlayerModel::Sigmoid(float x)
    {
        return 1.0f / (1.0f + expf(-x));
    }

    constexpr float AIPlayerModel::RulingCardSigmoid(const Card& card)
    {
        return (Sigmoid(card.m_number - 11.5f) + 0.1f) / (Sigmoid(2.5f) + 0.1f);
    }

    constexpr float AIPlayerModel::NonRulingCardSigmoid(const Card& card)
    {
        return Sigmoid(card.m_number - 11.5f) / Sigmoid(3);
    }

    constexpr float AIPlayerModel::GeneralCardSigmoid(const Card& card)
    {
        return Sigmoid(card.m_number - 11.5f);
    }

    Card AIPlayerModel::CalculateInitialBet() const
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

    std::map<eCardSuit, int8_t> AIPlayerModel::CountSuits() const
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