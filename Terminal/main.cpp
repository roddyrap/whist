#include <Logic/Hand.h>
#include <Logic/Game.h>
#include <Logic/IPlayer.h>
#include <Logic/DirectPlayer.h>

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

    whistGame.SetPlayer(0, std::make_unique<TerminalPlayer>(whistGame, 0)),
    whistGame.SetPlayer(1, CreateAIPlayer(whistGame, 1));
    whistGame.SetPlayer(2, CreateAIPlayer(whistGame, 2));
    whistGame.SetPlayer(3, CreateAIPlayer(whistGame, 3));

    whistGame.DealCards();

    while (whistGame.GetGameState() != eGameState::FINISHED)
    {
        whistGame.SignalNextPlayer();

        // Everyone skipped :(
        if (whistGame.GetGameState() == eGameState::INVALID)
        {
            std::wcout << "Unfortunately, Nobody set the ruling type. This is undefined behaviour." << std::endl;
            std::wcout << "0 - Continue the game without a ruling type, 1 - restart the game. 0 is default." << std::endl;

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

    }

    ClearScreen();
    PrintEndScreen(whistGame);

    std::wcout << "Return to exit." << std::endl;
    getchar();
}