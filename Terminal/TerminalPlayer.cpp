#include "TerminalPlayer.h"

#include "TerminalUtils.h"
#include "TerminalCard.h"

using namespace Whist::Logic;

namespace Whist::Terminal
{
    void TerminalPlayer::PrintPlayerCards()
    {
        const auto& playerCards{m_hand.GetCards()};
        for (size_t cardIndex = 0; cardIndex < playerCards.size(); ++cardIndex)
        {
            std::wcout << TerminalCard{m_hand.GetCards().at(cardIndex)} << ", ";
        }

        std::wcout << "\n";
    }

    void TerminalPlayer::PrintTurn()
    {
        std::wcout << "Game state: " << GameStateToWString(m_game.GetGameState()) << std::endl;
        std::wcout << "Current round: " << m_game.GetRoundNumber() + 1 << std::endl;

        if (m_game.GetGameState() == eGameState::ROUNDS)
        {
            std::wcout << "Game Info: " << CardSuitToWString(m_game.GetRulingType()) << std::showpos << m_game.GetBetsSum() - PLAYER_HAND_SIZE << std::noshowpos << std::endl;

            std::wcout << std::endl;
            Terminal::PrintPlayersStatus(m_game);
            std::wcout << std::endl;

            // Print previous round if it exists.
            if (m_game.GetRoundNumber() != 0)
            {
                std::wcout << "Previous Round: ";
                for (const auto& card : m_game.GetPreviousRound())
                {
                    std::wcout << TerminalCard{card} << ", ";
                }

                std::wcout << std::endl << std::endl;
            }

            std::array<Card, NUM_PLAYERS> currentRound{m_game.GetCurrentRound()};

            if (m_game.GetStartingPlayer() == m_playerIndex)
            {
                std::wcout << "You are first!" << std::endl;
            }
            else
            {
                for (int8_t playerIndex = m_game.GetStartingPlayer(); playerIndex != m_playerIndex; playerIndex = (playerIndex + 1) % NUM_PLAYERS)
                {
                    std::wcout << "Player " << playerIndex + 1 << ": " << TerminalCard{currentRound[playerIndex]} << std::endl;
                }
            }

            std::wcout << std::endl;
        }
        else
        {
            std::wcout << "Player Bets: " << Terminal::GetInitialBets(m_game) << std::endl;
        }

        std:: wcout << "Hand: ";
        this->PrintPlayerCards();
    }

    bool TerminalPlayer::PlaceInitialBet()
    {
        // Don't bet again if already skipped.
        if (m_game.GetBets()[m_playerIndex].m_number == SKIP_BET)
        {
            return true;
        }

        Terminal::ClearScreen();
        this->PrintTurn();

        Card inputCard{};
        std::wcout << "Enter bet: ";
        while (!Terminal::GetCardInput(inputCard) || m_game.PlaceBet(m_playerIndex, inputCard) != 0)
        {
            std::wcout << "Enter bet: ";
        };

        std::wcout << "Turn Done! Return for next turn." << std::endl;
        std::getchar();

        return true;
    }

    bool TerminalPlayer::PlaceSecondaryBet()
    {
        // Don't change bet if player is leader and game wasn't invalid.
        if (m_game.GetRulingPlayer() == m_playerIndex && m_game.AreInitialBetsValid())
        {
            return true;
        }

        Terminal::ClearScreen();
        this->PrintTurn();

        int32_t betInput{};
        do
        {
            std::wcout << "Enter bet: ";
        } while (!Terminal::GetNumberInput(betInput) || m_game.PlaceBet(m_playerIndex, {eCardSuit::NO_TYPE, static_cast<card_number_t>(betInput)}));

        std::wcout << "Turn Done! Return for next turn." << std::endl;
        std::getchar();

        return true;
    }

    bool TerminalPlayer::PlaceCard()
    {
        Terminal::ClearScreen();
        this->PrintTurn();

        Card inputCard{};
        std::wcout << "Enter Card: ";
        while (!Terminal::GetCardInput(inputCard) || !m_game.PlaceCard(m_playerIndex, inputCard))
        {
            std::wcout << "Enter Card: ";
        };

        m_hand.RemoveCard(inputCard);
        std::wcout << "Turn Done! Return for next turn." << std::endl;
        std::getchar();

        return true;
    }
}
