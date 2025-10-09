#ifndef WHIST_LOGIC_GAME_H
#define WHIST_LOGIC_GAME_H

#include "Hand.h"
#include "Card.h"

#include <array>
#include <vector>
#include <bitset>

namespace Whist::Logic
{
    constexpr uint8_t NUM_PLAYERS{4};
    constexpr int8_t  SKIP_BET{-1};
    constexpr int8_t  MIN_LEADING_BET{5};

    enum class eGameState
    {
        INITIAL_BETTING,
        SECONDARY_BETTING,
        ROUNDS,
        PAUSED,
        FINISHED,

        // Initial betting failed.
        INVALID
    };

    class Game
    {
    public:
        virtual ~Game() = default;

        /**
         * Deals cards to the hands in the game.
         */
        virtual void DealCards();

        /**
         * Places a bet for the player, works for both initial & secondary
         * betting rounds. Will return error if the game state doesn't allow betting.
         *
         * @param playerIndex The index of the player to set the new bet of.
         * @param playerBet   The new player's bet. -1 is pass, and is only
         *                    valid in initial betting round.
         * @return            Whether the operation was completed successfully.
         */
        virtual int8_t PlaceBet(uint8_t playerIndex, Card playerBet);

        /**
         * Simulates a player taking their turn in a betting round. Will allow
         * players to place cards out of order, but will not allow a player to
         * place a card more than once or to place a card it didn't have.
         *
         * @param playerIndex The index of the player to play the card for.
         * @param cardPlaced  The card to play for the player.
         * @return            Whether the operation was completed successfully.
         */
        virtual bool PlaceCard(uint8_t playerIndex, Card cardPlaced);

        /**
         * A helper function for solving the invalid game state.
         * It initializes the game without a ruler and the ruling player will
         * be player number 0.
         *
         * Will only work if the game is invalid.
        */
        virtual void HandleInvalid();

        const Hand& GetPlayer(uint8_t playerIndex) const;
        Hand& GetMutablePlayer(uint8_t playerIndex);

        const std::array<Card, NUM_PLAYERS>& GetBets() const;
        const std::array<uint8_t, NUM_PLAYERS>& GetTakes() const;
        eCardSuit GetRulingType() const;
        int8_t GetRulingPlayer() const;

        eGameState GetGameState() const;

        bool IsGameFinished() const;

        uint8_t GetRoundNumber() const;
        int8_t GetStartingPlayer() const;
        const std::array<Card, NUM_PLAYERS>& GetCurrentRound() const;
        const std::array<Card, NUM_PLAYERS>& GetPreviousRound() const;
        uint8_t GetBetsSum() const;

        /**
         * Checks if the initial bets are valid and the game is supposed to continue
         * to secondary betting.
         *
         * @return If the secondary betting round should be started.
         */
        bool AreInitialBetsValid();

        int16_t CalculatePoints(uint8_t playerIndex);

        /**
         * @param playerIndex The player to check.
         * @return            Whether a player has bet/played a card in the current round. 
         */
        bool HasPlayerPlayed(uint8_t playerIndex) const;

    private:

        /**
         * The game can reach an undefined state in which everyone skipped during the
         * iniital betting round (as nobody had a strong enough bet) and as such no
         * ruling type and player have been established.
         *
         * @return If the game has reached a invalid state.
        */
        bool IsInvalid();

        /**
         * Checks if the secondary bets are valid and the game is supposed to continue
         * to game rounds.
         *
         * @return If the game should be started.
         */
        bool ArePlayersSkipping();


        /**
         * Sets the winner of the current round and clears the state for a new
         * round to start.
         */
        void EndRound();

        /**
         * Finds the index of the player who put the highest ranked card in the
         * current round.
         *
         * @return The index of the winning player in the current round.
        */
        uint8_t GetWinnderIndex() const;

    protected:
        std::array<Hand, NUM_PLAYERS> m_playerHands{};

    private:
        // Use card data type to save bet type and bet value.
        std::array<Card, NUM_PLAYERS> m_playerBets{};
        std::array<uint8_t, NUM_PLAYERS> m_playerTakes{0};

        int8_t m_rulingPlayer{-1};
        eCardSuit m_rulingType{eCardSuit::NO_TYPE};

        std::array<Card, NUM_PLAYERS> m_previousRound{};
        std::array<Card, NUM_PLAYERS> m_currentRound{};
        std::bitset<NUM_PLAYERS> m_playedPlayers{};
        eCardSuit m_currentRoundType{};
        int8_t m_startingPlayerIndex{-1};

        eGameState m_gameState{};
        uint8_t m_roundNumber{0};
    };
}

#endif // WHIST_LOGIC_GAME_H