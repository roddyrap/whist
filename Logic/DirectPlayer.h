#ifndef WHIST_LOGIC_DIRECT_PLAYER_H
#define WHIST_LOGIC_DIRECT_PLAYER_H

#include "AIPlayerModel.h"
#include "IPlayerModel.h"
#include "IPlayer.h"

#include <memory>

namespace Whist::Logic
{
    // The most basic kind of player that does something, just passes of data between the game and the underlying model.
    class DirectPlayer : public IPlayer
    {
    public:
        DirectPlayer(Game& game, uint8_t playerIndex, std::unique_ptr<IPlayerModel> playerModel) :
            m_game{game}, m_playerIndex{playerIndex}, m_playerModel{std::move(playerModel)} {}

        // @see IPlayer.h
        void ReceiveCard(Card newCard) override
        {
            return;
        }

        // @see IPlayer.h
        void ShouldPlayCard() override
        {
            m_game.PlaceCard(m_playerIndex, m_playerModel->GetAction());
        }

        // @see IPlayer.h
        void ShouldPlaceBet() override
        {
            m_game.PlaceBet(m_playerIndex, m_playerModel->GetAction());
        }

    private:
        Game& m_game;
        uint8_t m_playerIndex;
        std::unique_ptr<IPlayerModel> m_playerModel;
    };

    std::unique_ptr<IPlayer> CreateAIPlayer(Game& game, uint8_t playerIndex)
    {
        return std::make_unique<DirectPlayer>(game, playerIndex, std::make_unique<AIPlayerModel>(game, playerIndex));
    }
}

#endif // WHIST_LOGIC_DIRECT_PLAYER_H