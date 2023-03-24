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
        std::map<eCardSuit, int8_t> CountSuits() const;

        Game& m_game;
        Hand& m_hand;

        uint8_t m_playerIndex;
    };
}

#endif // WHIST_LOGIC_AI_PLAYER_H