#ifndef WHIST_LOGIC_I_PLAYER_H
#define WHIST_LOGIC_I_PLAYER_H

#include "Card.h"

namespace Whist::Logic
{
    class IPlayer
    {
    public:
        virtual ~IPlayer() = default;

        // Called when dealing cards.
        virtual void ReceiveCard(Card newCard) = 0;

        // Called when player should play a card in ROUNDS.
        virtual void ShouldPlayCard() = 0;

        // Called when player should bet in initial/secondary betting rounds.
        virtual void ShouldPlaceBet() = 0;
    };
}

#endif // WHIST_LOGIC_I_PLAYER_H