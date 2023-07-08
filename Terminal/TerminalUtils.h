#ifndef WHIST_TERMINAL_UTILS_H
#define WHIST_TERMINAL_UTILS_H

#include <Logic/Game.h>
// #include <Logic/Hand.h>
#include <Logic/eCardSuit.h>

namespace Whist::Terminal
{
    /**
     * Send ANSI clear screen and and ANSI cursor to row and col 0 to wcout.
    */
    void ClearScreen();

    /**
     * Converts the given game state to a human readable string.
     * 
     * @param gameState The game state to convert to string.
     * @return          A string of the current game state.
    */
    std::wstring GameStateToWString(Logic::eGameState gameState);

    std::wstring GetInitialBets(Logic::Game& whistGame);

    std::wstring GetTakes(Logic::Game& whistGame);

    bool GetCardInput(Logic::Card& o_card);

    bool GetNumberInput(int32_t& o_number);

    /**
     * Print the takes and bets of each player in a way that's readable
     * and understanable.
     * 
     * @param whistGame The game object to take player data from.
    */
    void PrintPlayersStatus(Logic::Game& whistGame);

    /**
     * Print the scores and status of each player. This is a large print
     * that is supposed to give information about a recently finished game.
    */
    void PrintEndScreen(Logic::Game& whistGame);

    /**
     * Get the unicode characters of the card suits, with color support
     * for the red suits.
     * 
     * @return The unicode string representation of the suit with color.
    */
    std::wstring CardSuitToWString(Logic::eCardSuit cardSuit);
} // namespace Whist::Terminal

#endif // WHIST_TERMINAL_UTILS_H
