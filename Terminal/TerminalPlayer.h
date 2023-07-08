#ifndef WHIST_TERMINAL_PLAYER_H
#define WHIST_TERMINAL_PLAYER_H

#include <Logic/Hand.h>
#include <Logic/Game.h>
#include <Logic/IPlayer.h>

namespace Whist::Terminal
{
    class TerminalPlayer : public Logic::IPlayer
    {
    public:
        TerminalPlayer(Logic::Game& game, uint8_t playerIndex) :
            m_game{game}, m_hand{game.GetMutablePlayer(playerIndex)}, m_playerIndex{playerIndex}
        {}

        void PrintPlayerCards();
        void PrintTurn();

        /**
         * @see IPlayer.h
        */
        virtual bool PlaceInitialBet() override;
        virtual bool PlaceSecondaryBet() override;
        virtual bool PlaceCard() override;

    private:
        Logic::Game& m_game;
        Logic::Hand& m_hand;

        uint8_t m_playerIndex;
    };
}

#endif // WHIST_TERMINAL_PLAYER_H