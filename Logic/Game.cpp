#include "Game.h"

#include "EmptyPlayer.h"
#include "AIPlayerModel.h"

#include <random>
#include <algorithm>
#include <vector>

namespace Whist::Logic
{
    // Initialize nothing players in first place.
    Game::Game() : m_players{
        std::make_unique<EmptyPlayer>(),
        std::make_unique<EmptyPlayer>(),
        std::make_unique<EmptyPlayer>(),
        std::make_unique<EmptyPlayer>()
    } {}

    void Game::SetPlayer(uint8_t playerIndex, std::unique_ptr<IPlayer> player)
    {
        if (playerIndex >= NUM_PLAYERS) return;
        m_players[playerIndex] = std::move(player);
    }

    void Game::SignalNextPlayer()
    {
        uint8_t waitingPlayerIndex = this->GetWaitingPlayer();
        if (m_gameState == eGameState::INITIAL_BETTING || m_gameState == eGameState::SECONDARY_BETTING)
        {
            m_players[waitingPlayerIndex]->ShouldPlaceBet();
        }
        else if (m_gameState == eGameState::ROUNDS)
        {
            m_players[waitingPlayerIndex]->ShouldPlayCard();
        }
    }

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
                // Notify player.
                this->m_players[playerIndex]->ReceiveCard(cardDeck.back());

                // Keep internal reference.
                m_hands[playerIndex].AddCard(cardDeck.back());
                cardDeck.pop_back();
            }
        }

        for (auto& hand : m_hands)
        {
            hand.SortCards();
        }
    }

    int8_t Game::IsBetValid(uint8_t playerIndex, Card playerBet) const
    {
        if (playerIndex >= NUM_PLAYERS) return BET_INVALID_PLAYER_INDEX;

        // Verify that the game state allows for betting.
        if (m_gameState != eGameState::SECONDARY_BETTING && m_gameState != eGameState::INITIAL_BETTING) return ACTION_INCORRECT_STATE;

        if (m_gameState == eGameState::SECONDARY_BETTING)
        {
            // If the player isn't the last better (Including if they already have an active bet) then we don't need to worry
            // about completing to 13, and most secondary bets are valid.
            if (m_playedPlayers.count() < 3 || (m_playedPlayers.count() == 3 && m_playedPlayers[playerIndex]))
            {
                // A bet of 13 isn't likely (Especially in the secondary round) but who am I to judge.
                return playerBet.m_number >= 0 && playerBet.m_number <= PLAYER_HAND_SIZE ? ACTION_SUCCESS : BET_INVALID_NUMBER;
            }
            else
            {
                return this->GetBetsSum() - m_playerBets[playerIndex].m_number + playerBet.m_number == 13 ? BET_WILL_COMPLETE_TO_HAND_SIZE : ACTION_SUCCESS;
            }
        }

        // From now the only viable state is initial betting.

        // NOTE: This might be the first bet, in which case the ruling bet is 0 0.
        Card ruling_bet = m_playerBets[m_rulingPlayer];
        bool rulingPlayer = playerIndex == m_rulingPlayer || ruling_bet.m_number < 5;

        if (playerBet.m_number == SKIP_BET)
        {
            // If there's no active ruling bet then the current player must not be the active ruling bet
            // in which case they are allowed to skip.
            if (ruling_bet.m_number == 0) return ACTION_SUCCESS;

            // If there is a ruling bet then that ruling player must not skip.
            return rulingPlayer? BET_RULING_PLAYER_MUST_NOT_SKIP : ACTION_SUCCESS;
        }

        // At this point the player's bet needs to be a valid initial bet.
        if (playerBet.m_number < 5) return BET_INITIAL_TOO_SMALL;

        if (playerBet.m_number < ruling_bet.m_number)
        {
            return BET_INITIAL_TOO_SMALL;
        }

        if (ruling_bet.m_number >= 5 && static_cast<uint8_t>(playerBet.m_suit) < (static_cast<uint8_t>(ruling_bet.m_suit) + !rulingPlayer))
        {
            return BET_INITIAL_LOW_PRIORITY_SUIT;
        }

        return ACTION_SUCCESS;
    }

    int8_t Game::IsPlayValid(uint8_t playerIndex, Card playerCard) const
    {
        if (m_gameState != eGameState::ROUNDS) return ACTION_INCORRECT_STATE;

        if (!m_hands[playerIndex].HasCard(playerCard)) return PLACEMENT_PLAYER_DOES_NOT_HAVE_CARD;

        // Checking if the player has cards with the round's suit but doesn't use them.
        if (playerIndex != m_startingPlayerIndex && (playerCard.m_suit != m_currentRoundType && m_hands[playerIndex].HasType(m_currentRoundType))) return PLACEMENT_INCORRECT_SUIT;

        return ACTION_SUCCESS;
    }

    int8_t Game::PlaceBet(uint8_t playerIndex, Card playerBet)
    {
        int8_t betValidationResult = this->IsBetValid(playerIndex, playerBet);
        if (betValidationResult != ACTION_SUCCESS)
        {
            return betValidationResult;
        }

        // At this point the bets MUST be valid (Except for invalid turn), otherwise the validation method should fail.
        if (playerIndex != this->GetWaitingPlayer()) return ACTION_NOT_PLAYER_TURN;

        m_playedPlayers[playerIndex] = true;

        if (m_gameState == eGameState::SECONDARY_BETTING)
        {
            m_playerBets[playerIndex] = playerBet;

            // If this is the last player to bet than the bet sum validation has already been
            // performed in the validation method, and the game can continue.
            if (!this->ArePlayersSkipping())
            {
                m_playedPlayers.reset();
                m_gameState = eGameState::ROUNDS;
            }
        }
        // Because of the bet validation this must be INITIAL_BETTING
        else
        {
            // Check that new bet is the highest bet or player is skipping.
            if (playerBet.m_number != SKIP_BET)
            {
                m_rulingPlayer = playerIndex;
            }

            m_playerBets[playerIndex] = playerBet;

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

        return ACTION_SUCCESS;
    }

    int8_t Game::PlaceCard(uint8_t playerIndex, Card cardPlaced)
    {
        int8_t placementValidationResult = this->IsPlayValid(playerIndex, cardPlaced);
        if (placementValidationResult != ACTION_SUCCESS)
        {
            return placementValidationResult;
        }

        if (playerIndex != this->GetWaitingPlayer()) return ACTION_NOT_PLAYER_TURN;

        if (playerIndex == m_startingPlayerIndex)
        {
            m_currentRoundType = cardPlaced.m_suit;
        }

        // Place card
        m_playedPlayers[playerIndex] = true;
        m_currentRound[playerIndex] = cardPlaced;

        // End round if all players put a card.
        if (m_playedPlayers.all())
        {
            this->EndRound();
        }

        return ACTION_SUCCESS;
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

    uint8_t Game::GetRulingPlayer() const
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


    bool Game::HasPlayerPlayed(uint8_t playerIndex) const
    {
        if (playerIndex > m_playedPlayers.size()) return false;
        return m_playedPlayers[playerIndex];
    }

    uint8_t Game::GetWaitingPlayer() const
    {
        if (m_playedPlayers.none()) return m_startingPlayerIndex;
        return (m_startingPlayerIndex + m_playedPlayers.count()) % 4;
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

    const Hand& Game::GetHand(uint8_t playerIndex) const
    {
        return m_hands.at(playerIndex);
    }

    Hand& Game::GetMutableHand(uint8_t playerIndex)
    {
        return m_hands.at(playerIndex);
    }

    eGameState Game::GetGameState() const
    {
        return m_gameState;
    }

    bool Game::IsBetting() const
    {
        return m_gameState == eGameState::INITIAL_BETTING || m_gameState == eGameState::SECONDARY_BETTING;
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
