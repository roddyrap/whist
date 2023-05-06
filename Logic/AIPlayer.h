#ifndef WHIST_LOGIC_AI_PLAYER_H
#define WHIST_LOGIC_AI_PLAYER_H

#include "Hand.h"
#include "Game.h"
#include "IPlayer.h"

#include <map>

namespace Whist::Logic
{
    class AIPlayer : public IPlayer
    {
    public:
        AIPlayer(Game& game, uint8_t playerIndex) :
            m_game{game}, m_hand{game.GetMutablePlayer(playerIndex)}, m_playerIndex{playerIndex}
        {}

        /**
         * @see IPlayer.h
        */
        virtual bool PlaceInitialBet() override;
        virtual bool PlaceSecondaryBet() override;
        virtual bool PlaceCard() override;

    private:
        /**
         * A plain mathematical Sigmoid function. 1 / (1 + e^(-x)).
         * 
         * @param x                The number to compute the sigmoid of.
         * @return constexpr float The sigmoid of x.
         */
        static constexpr float Sigmoid(float x);

        /**
         * These functions calculate the capturing power of a card. The difference
         * between the functions is the numerical constants used as each state of
         * the card changes the capturing power of a card appropriately.
         * 
         * @param card             The card to calculate.
         * @return constexpr float The probability of the card to generate a take.
         */
        static constexpr float RulingCardSigmoid(const Card& card);
        static constexpr float NonRulingCardSigmoid(const Card& card);
        static constexpr float GeneralCardSigmoid(const Card& card);

        /**
         * Calculate an initial bet for the given hand. Each card is evaluated
         * individually for the three possible states in the game (ruling, no-ruler, not ruling).
         * A random number is then generated that represents the AI's certainty of that card.
         * If the AI's certainty is smaller than the card's taking power, it's added to each
         * bet of the suit.
         * 
         * @return Card The highest bet with its suit.
         */
        Card CalculateInitialBet() const;

        /**
         * Counts the number of cards the player has for each suit.
         * 
         * @return std::map<eCardSuit, int8_t> Each suit the player has mapped to the number
         *                                     of card of the same suit in the hand.
         */
        std::map<eCardSuit, int8_t> CountSuits() const;

    private:
        // An array containing all of the available card suites, for easy iterations.
        static constexpr std::array<eCardSuit, 5> BETTING_SUITS{eCardSuit::CLUBS, eCardSuit::DIAMONDS,
                                                                eCardSuit::HEARTS, eCardSuit::SPADES, 
                                                                eCardSuit::NO_TYPE};

        // Game state references.
        Game& m_game;
        Hand& m_hand;

        uint8_t m_playerIndex;
    };
}

#endif // WHIST_LOGIC_AI_PLAYER_H