#ifndef WHIST_LOGIC_EMPTY_PLAYER_H
#define WHIST_LOGIC_EMPTY_PLAYER_H

#include "IPlayerModel.h"
#include "IPlayer.h"
#include <memory>

namespace Whist::Logic
{
    // The most basic kind of player. Does nothing.
    class EmptyPlayer : public IPlayer
    {
    public:
        virtual ~EmptyPlayer() = default;

        // @see IPlayer.h
        void ReceiveCard(Card newCard) override
        {
            (void)newCard;
            return;
        }

        // @see IPlayer.h
        void ShouldPlayCard() override
        {
            return;
        }

        // @see IPlayer.h
        void ShouldPlaceBet() override
        {
            return;
        }
    };
}

#endif // WHIST_LOGIC_EMPTY_PLAYER_H