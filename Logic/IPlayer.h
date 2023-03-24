#ifndef WHIST_LOGIC_I_PLAYER_H
#define WHIST_LOGIC_I_PLAYER_H

namespace Whist::Logic
{
    class IPlayer
    {
    public:
        virtual ~IPlayer() = default;

        virtual bool PlaceInitialBet() = 0;
        virtual bool PlaceSecondaryBet() = 0;
        virtual bool PlaceCard() = 0;
    };
}

#endif // WHIST_LOGIC_I_PLAYER_H