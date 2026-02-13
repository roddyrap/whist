#ifndef WHIST_LOGIC_GAME_H
#define WHIST_LOGIC_GAME_H

#include "IPlayer.h"
#include "Hand.h"
#include "Card.h"

#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <optional>

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
        enum eGameActionResult
        {
            ACTION_SUCCESS = 0,
            ACTION_INCORRECT_STATE,
            ACTION_NOT_PLAYER_TURN,
            BET_INVALID_NUMBER, // For bets smaller than 0 or larger than 13.
            BET_RULING_PLAYER_MUST_NOT_SKIP,
            BET_INVALID_PLAYER_INDEX,
            BET_WILL_COMPLETE_TO_HAND_SIZE,
            BET_INITIAL_TOO_SMALL,
            BET_INITIAL_LOW_PRIORITY_SUIT,
            PLACEMENT_PLAYER_DOES_NOT_HAVE_CARD,
            PLACEMENT_INCORRECT_SUIT,
        };

    public:
        Game();
        virtual ~Game() = default;

        // Change a player.
        void SetPlayer(uint8_t playerIndex, std::unique_ptr<IPlayer> player);

        void SignalNextPlayer();

        /**
         * Deals cards to the hands in the game.
         */
        virtual void DealCards();

        // Will return success (0) even if it's not the player's turn.
        virtual int8_t IsBetValid(uint8_t playerIndex, Card playerBet) const;
        virtual int8_t IsPlayValid(uint8_t playerIndex, Card playerBet) const;

        /**
         * Places a bet for the player, works for both initial & secondary
         * betting rounds. Will return error if the game state doesn't allow betting.
         *
         * @param playerIndex The index of the player to set the new bet of.
         * @param playerBet   The new player's bet. -1 is pass, and is only
         *                    valid in initial betting round.
         * @return            Zero on success, a positive integer on error.
         */
        virtual int8_t PlaceBet(uint8_t playerIndex, Card playerBet);

        /**
         * Simulates a player taking their turn in a betting round. Will allow
         * players to place cards out of order, but will not allow a player to
         * place a card more than once or to place a card it didn't have.
         *
         * @param playerIndex The index of the player to play the card for.
         * @param cardPlaced  The card to play for the player.
         * @return            Zero on success, a positive integer on error.
         */
        virtual int8_t PlaceCard(uint8_t playerIndex, Card cardPlaced);

        /**
         * A helper function for solving the invalid game state.
         * It initializes the game without a ruler and the ruling player will
         * be player number 0.
         *
         * Will only work if the game is invalid.
        */
        virtual void HandleInvalid();

        const Hand& GetHand(uint8_t playerIndex) const;
        Hand& GetMutableHand(uint8_t playerIndex);

        const std::array<Card, NUM_PLAYERS>& GetBets() const;
        const std::array<uint8_t, NUM_PLAYERS>& GetTakes() const;
        std::optional<eCardSuit> GetRulingSuit() const;
        std::optional<uint8_t> GetRulingPlayer() const;

        eGameState GetGameState() const;
        bool IsBetting() const;

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
        bool IsInvalid();

        int16_t CalculatePoints(uint8_t playerIndex);

        /**
         * @param playerIndex The player to check.
         * @return            Whether a player has bet/played a card in the current round. 
         */
        bool HasPlayerPlayed(uint8_t playerIndex) const;

        // Get the player we are waiting for input from.
        uint8_t GetWaitingPlayer() const;

    private:

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

    private:
        std::array<std::unique_ptr<IPlayer>, NUM_PLAYERS> m_players;
        std::array<Hand, NUM_PLAYERS> m_hands{};

        // Use card data type to save bet type and bet value.
        std::array<Card, NUM_PLAYERS> m_playerBets{};
        std::array<uint8_t, NUM_PLAYERS> m_playerTakes{0};

        std::optional<uint8_t> m_rulingPlayer{std::nullopt};

        std::array<Card, NUM_PLAYERS> m_previousRound{};
        std::array<Card, NUM_PLAYERS> m_currentRound{};
        std::bitset<NUM_PLAYERS> m_playedPlayers{};
        eCardSuit m_currentRoundType{};
        int8_t m_startingPlayerIndex{0};

        eGameState m_gameState{};
        uint8_t m_roundNumber{0};
    };
}

#endif // WHIST_LOGIC_GAME_H