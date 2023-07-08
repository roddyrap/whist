#include <Logic/Hand.h>
#include <Logic/Game.h>
#include <Logic/IPlayer.h>
#include <Logic/AIPlayer.h>

#include "WhistConfig.h"

#include "TerminalUtils.h"
#include "TerminalPlayer.h"

#include <iostream>
#include <memory>
#include <unistd.h>

using namespace Whist::Terminal;
using namespace Whist::Logic;

int main(int argc, char** argv)
{
    int opt{0};
    while ((opt = getopt(argc, argv, "h")) != -1)
    {
        switch(opt)
        {
        case 'h':
            std::cout << "Whist Version: " << WHIST_VERSION_MAJOR << '.' << WHIST_VERSION_MINOR << std::endl <<
                         "Version Type: " << BUILD_TYPE << std::endl;
            exit(EXIT_SUCCESS);
        }
    }

    // Set terminal to print Unicode characters (Mostly card suit symbols).
    setlocale( LC_ALL, "en_US.utf8" );

    // Initialize whist game.
    Game whistGame{};
    whistGame.DealCards();

    // Create player types.
    std::vector<std::unique_ptr<IPlayer>> playerModels{};
    playerModels.push_back(std::make_unique<TerminalPlayer>(whistGame, 0));
    for (int8_t playerIndex = 1; playerIndex < NUM_PLAYERS; ++playerIndex)
    {
        playerModels.push_back(std::make_unique<AIPlayer>(whistGame, playerIndex));
    }

    // Handle input for INITIAL_BETTING.
    while (whistGame.GetGameState() == eGameState::INITIAL_BETTING)
    {
        for (auto& playerModelPointer : playerModels)
        {
            playerModelPointer->PlaceInitialBet();
        }
    }

    // Everyone skipped :(
    if (whistGame.GetGameState() == eGameState::INVALID)
    {
        std::wcout << "Unfortunately, Nobody set the ruling type. This is undefined behaviour." << std::endl;
        std::wcout << "0 - Continue the game without a ruling type, 1 - restart the game, 2 - continue. 0 is default." << std::endl;

        int32_t selection{0};
        std::wcout << "Enter Selection: ";
        GetNumberInput(selection);

        if (selection == 0)
        {
            whistGame.HandleInvalid();
        }
        else if (selection == 1)
        {
            main(argc, argv);
            return 0;
        }
    }

    // Handle input for SECONDARY_BETTING.
    while (whistGame.GetGameState() == eGameState::SECONDARY_BETTING)
    {
        for (auto& playerModelPointer : playerModels)
        {
            playerModelPointer->PlaceSecondaryBet();
        }
    }

    // Handle input for actual game.
    while (whistGame.GetGameState() == eGameState::ROUNDS)
    {
        int8_t startingPlayerIndex{whistGame.GetStartingPlayer()};
        for (int8_t playerOffset = 0; playerOffset < NUM_PLAYERS; ++playerOffset)
        {
            int8_t playerIndex{static_cast<int8_t>((startingPlayerIndex + playerOffset) % NUM_PLAYERS)};
            playerModels.at(playerIndex)->PlaceCard();
        }
    }

    ClearScreen();
    PrintEndScreen(whistGame);

    std::wcout << "Return to exit." << std::endl;
    getchar();
}