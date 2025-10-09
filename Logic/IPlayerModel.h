#ifndef WHIST_LOGIC_I_PLAYER_MODEL_H
#define WHIST_LOGIC_I_PLAYER_MODEL_H

#include "Card.h"
#include "Game.h"

namespace Whist::Logic
{
    class IPlayerModel
    {
    public:
        // Because you can only place a card or place a bet in different game states, we don't
        // really need to differentiate the two.
        virtual Card GetAction() = 0;
    };
}

#endif // WHIST_LOGIC_I_PLAYER_MODEL_H