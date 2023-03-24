#ifndef WHIST_TERMINAL_UTILS_H
#define WHIST_TERMINAL_UTILS_H

#include <Logic/Game.h>
// #include <Logic/Hand.h>
#include <Logic/eCardSuit.h>

namespace Whist::Terminal
{
    void ClearScreen();

    std::wstring GameStateToWString(Logic::eGameState gameState);

    std::wstring GetInitialBets(Logic::Game whistGame);

    std::wstring GetTakes(Logic::Game whistGame);

    bool GetCardInput(Logic::Card& o_card);

    bool GetNumberInput(int32_t& o_number);

    void PrintPlayersStatus(Logic::Game whistGame);
    void PrintEndScreen(Logic::Game whistGame);

    std::wstring CardSuitToWString(Logic::eCardSuit cardSuit);
}

#endif // WHIST_TERMINAL_UTILS_H
