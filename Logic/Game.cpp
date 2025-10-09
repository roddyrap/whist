#include "Game.h"

#include <random>
#include <algorithm>
#include <vector>

namespace Whist::Logic
{
    void Game::DealCards()
    {
        // Create card deck.
        std::vector<Card> cardDeck{};
        for (card_number_t cardNumber = 2; cardNumber < 2 + PLAYER_HAND_SIZE; ++cardNumber)
        {
            cardDeck.emplace_back(eCardSuit::SPADES, cardNumber);
            cardDeck.emplace_back(eCardSuit::HEARTS, cardNumber);
            cardDeck.emplace_back(eCardSuit::DIAMONDS, cardNumber);
            cardDeck.emplace_back(eCardSuit::CLUBS, cardNumber);
        }

        // Shuffle card deck.
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(std::begin(cardDeck), std::end(cardDeck), g);

        // Deal card to players.
        for (int8_t playerIndex = 0; playerIndex < NUM_PLAYERS; ++playerIndex)
        {
            for (int8_t cardIndex = 0; cardIndex < PLAYER_HAND_SIZE; ++cardIndex)
            {
                m_playerHands[playerIndex].AddCard(cardDeck.back());
                cardDeck.pop_back();
            }
        }

        for (auto& hand : m_playerHands)
        {
            hand.SortCards();
        }
    }

    int8_t Game::PlaceBet(uint8_t playerIndex, Card playerBet)
    {
        // Validate game state.
        if (m_gameState != eGameState::SECONDARY_BETTING && m_gameState != eGameState::INITIAL_BETTING)
        {
            return 1;
        }

        if (m_gameState == eGameState::SECONDARY_BETTING && playerBet.m_number >= 0)
        {
            if (m_playedPlayers[playerIndex])
            {
                return 0;
            }

            m_playedPlayers[playerIndex] = true;
            m_playerBets[playerIndex] = playerBet;

            // Validate that if this is the last player to secondary bet the sum of bets is valid.
            if (this->GetBetsSum() == PLAYER_HAND_SIZE && m_playedPlayers.all())
            {
                // Reset bet.
                m_playedPlayers[playerIndex] = false;
                m_playerBets[playerIndex] = Card{eCardSuit::NO_TYPE, SKIP_BET};
                return 3;
            }

            if (!this->ArePlayersSkipping())
            {
                m_playedPlayers.reset();
                m_gameState = eGameState::ROUNDS;
            }
        }
        else if (m_gameState == eGameState::INITIAL_BETTING && playerBet.m_number >= SKIP_BET)
        {
            // Check that new bet is the highest bet or player is skipping.
            if (playerBet.m_number == SKIP_BET)
            {
                m_playerBets[playerIndex] = playerBet;
            }
            else if ((m_rulingPlayer == -1 && playerBet.m_number >= MIN_LEADING_BET) ||
                     (m_rulingPlayer != -1 && playerBet > m_playerBets[m_rulingPlayer]))
            {
                m_rulingPlayer = playerIndex;
                m_playerBets[playerIndex] = playerBet;
            }
            else
            {
                return 4;
            }

            if (this->AreInitialBetsValid())
            {
                // Make it so all player but the ruling player need to bet again
                // by setting their bet to skip.
                for (uint8_t loopPlayerIndex = 0; loopPlayerIndex < NUM_PLAYERS; ++ loopPlayerIndex)
                {
                    if (loopPlayerIndex != m_rulingPlayer)
                    {
                        m_playerBets[loopPlayerIndex].m_number = SKIP_BET;
                    }
                }

                m_rulingType = m_playerBets[m_rulingPlayer].m_suit;
                m_startingPlayerIndex = m_rulingPlayer;

                m_playedPlayers.reset();
                m_playedPlayers[m_rulingPlayer] = true;

                m_gameState = eGameState::SECONDARY_BETTING;
            }
            else if (this->IsInvalid())
            {
                m_gameState = eGameState::INVALID;
            }
        }
        else
        {
            return 5;
        }

        return 0;
    }

    bool Game::PlaceCard(uint8_t playerIndex, Card cardPlaced)
    {
        if (m_gameState != eGameState::ROUNDS || m_playedPlayers[playerIndex])
        {
            return false;
        }

        const Hand& playerHand{m_playerHands[playerIndex]};

        // Validate card
        if (!playerHand.HasCard(cardPlaced))
        {
            return false;
        }

        if (playerIndex == m_startingPlayerIndex)
        {
            m_currentRoundType = cardPlaced.m_suit;
        }
        else if (cardPlaced.m_suit != m_currentRoundType && playerHand.HasType(m_currentRoundType))
        {
            return false;
        }

        // Place card
        m_playedPlayers[playerIndex] = true;
        m_currentRound[playerIndex] = cardPlaced;

        // End round if all players put a card.
        if (m_playedPlayers.all())
        {
            this->EndRound();
        }

        return true;
    }

    void Game::HandleInvalid()
    {
        if (m_gameState != eGameState::INVALID)
        {
            return;
        }

        m_rulingPlayer = 0;
        m_startingPlayerIndex = 0;
        m_rulingType = eCardSuit::NO_TYPE;

        m_gameState = eGameState::SECONDARY_BETTING;
    }

    bool Game::IsGameFinished() const
    {
        return m_gameState == eGameState::FINISHED;
    }

    uint8_t Game::GetRoundNumber() const
    {
        return m_roundNumber;
    }

    eCardSuit Game::GetRulingType() const
    {
        return m_rulingType;
    }

    int8_t Game::GetRulingPlayer() const
    {
        return m_rulingPlayer;
    }

    bool Game::ArePlayersSkipping()
    {
        uint8_t numSkips{0};

        for (const auto& bet : m_playerBets)
        {
            if (bet.m_number == SKIP_BET)
            {
                ++numSkips;
            }
        }

        return numSkips != 0;
    }

    bool Game::AreInitialBetsValid()
    {
        uint8_t numSkips{0};

        for (const auto& bet : m_playerBets)
        {
            if (bet.m_number == SKIP_BET)
            {
                ++numSkips;
            }
        }

        return numSkips == NUM_PLAYERS - 1 &&
               m_rulingPlayer >= 0 &&
               m_playerBets[m_rulingPlayer].m_number >= MIN_LEADING_BET;
    }

    bool Game::HasPlayerPlayed(uint8_t playerIndex) const
    {
        if (playerIndex > m_playedPlayers.size()) return false;
        return m_playedPlayers[playerIndex];
    }

    bool Game::IsInvalid()
    {
        uint8_t numSkips{0};

        for (const auto& bet : m_playerBets)
        {
            if (bet.m_number == SKIP_BET)
            {
                ++numSkips;
            }
        }

        return numSkips == NUM_PLAYERS;
    }

    void Game::EndRound()
    {
        uint8_t winningPlayerIndex{this->GetWinnderIndex()};

        m_startingPlayerIndex = winningPlayerIndex;
        m_playerTakes[winningPlayerIndex] += 1;

        if (m_roundNumber >= PLAYER_HAND_SIZE - 1)
        {
            m_gameState = eGameState::FINISHED;
        }

        // Reset game state.
        ++m_roundNumber;
        m_playedPlayers.reset();

        m_previousRound = m_currentRound;
        for (auto& card : m_currentRound)
        {
            card = Card{eCardSuit::NO_TYPE, 0};
        }
    }


    uint8_t Game::GetWinnderIndex() const
    {
        // The index will surely be changed because in every round there is assumed to be
        // at least one valid card, and every valid card will be larger than zero.
        uint8_t winningPlayerIndex{UINT8_MAX};
        Card winningPlayerCard{m_currentRoundType, 0};

        for (uint8_t playerIndex = 0; playerIndex < NUM_PLAYERS; ++playerIndex)
        {
            const Card& playerCard{m_currentRound[playerIndex]};

            if (playerCard.m_suit == winningPlayerCard.m_suit)
            {
                if (playerCard.m_number > winningPlayerCard.m_number)
                {
                    winningPlayerIndex = playerIndex;
                    winningPlayerCard = playerCard;
                }
            }
            else if (playerCard.m_suit == m_rulingType)
            {
                winningPlayerIndex = playerIndex;
                winningPlayerCard = playerCard;
            }
        }

        return winningPlayerIndex;
    }

    const Hand& Game::GetPlayer(uint8_t playerIndex) const
    {
        return m_playerHands.at(playerIndex);
    }

    Hand& Game::GetMutablePlayer(uint8_t playerIndex)
    {
        return m_playerHands.at(playerIndex);
    }

    eGameState Game::GetGameState() const
    {
        return m_gameState;
    }


    const std::array<Card, NUM_PLAYERS>& Game::GetBets() const
    {
        return m_playerBets;
    }

    const std::array<uint8_t, NUM_PLAYERS>& Game::GetTakes() const
    {
        return m_playerTakes;
    }

    int8_t Game::GetStartingPlayer() const
    {
        return m_startingPlayerIndex;
    }

    const std::array<Card, NUM_PLAYERS>& Game::GetCurrentRound() const
    {
        return m_currentRound;
    }

    const std::array<Card, NUM_PLAYERS>& Game::GetPreviousRound() const
    {
        return m_previousRound;
    }

    uint8_t Game::GetBetsSum() const
    {
        uint8_t betsSum{0};
        for (const auto& card : m_playerBets)
        {
            betsSum += card.m_number;
        }

        return betsSum;
    }


    int16_t Game::CalculatePoints(uint8_t playerIndex)
    {
        uint8_t playerTakes{m_playerTakes[playerIndex]};
        uint8_t playerBets{static_cast<uint8_t>(m_playerBets[playerIndex].m_number)};
        uint8_t betsSum{this->GetBetsSum()};

        if (playerBets == 0)
        {
            int16_t zeroBonus{};

            // Points for zero depends on up or down.
            if (betsSum > PLAYER_HAND_SIZE)
            {
                zeroBonus = 50;
            }
            else
            {
                zeroBonus = 100;
            }

            // If bet was successfull the full bonus is returned. If not, it's
            // negated from the take score.
            if (playerTakes == 0)
            {
                return zeroBonus;
            }
            else
            {
                return playerTakes * 10 - zeroBonus;
            }
        }
        else
        {
            return (std::min(playerTakes, playerBets) - std::abs(playerTakes - playerBets)) * 10;
        }
    }
}
