#include "TerminalUtils.h"

#include "TerminalCard.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace Whist::Logic;

namespace Whist::Terminal
{
    void ClearScreen()
    {
        // Clear terminal.
        std::wcout << L"\033[2J";

        // Move cursor to beginning.
        std::wcout << L"\033[0;0H";
    }

    std::wstring GetInitialBets(Game& whistGame)
    {
        std::wstringstream stringStream{};
        auto playerBets{whistGame.GetBets()};
        if (playerBets.size() == 0)
        {
            return L"";
        }

        // Format bets.
        if (playerBets[0].m_number != SKIP_BET)
        {
            stringStream << TerminalCard{playerBets[0]};
        }
        else
        {
            stringStream << "SKIP";
        }

        for (size_t betIndex = 1; betIndex < playerBets.size(); ++betIndex)
        {
            if (playerBets[betIndex].m_number != SKIP_BET)
            {
                stringStream << ", " << TerminalCard{playerBets[betIndex]};
            }
            else
            {
                stringStream << ", " << "SKIP";
            }
        }

        return stringStream.str();
    }

    std::wstring GetTakes(Game& whistGame)
    {
        std::array<uint8_t, NUM_PLAYERS> playerTakes{whistGame.GetTakes()};
        std::array<Card, NUM_PLAYERS> playerBets{whistGame.GetBets()};

        std::wstringstream stringStream{};
        for (int8_t playerIndex = 0; playerIndex < NUM_PLAYERS; ++playerIndex)
        {
            stringStream << playerTakes[playerIndex] << " | " << playerBets[playerIndex].m_number << ", ";
        }

        return stringStream.str();
    }

    std::wstring GameStateToWString(eGameState gameState)
    {
        std::wstring result{};

        switch (gameState)
        {
        case eGameState::INITIAL_BETTING:
            result = L"Initial Betting Round";
            break;

        case eGameState::INVALID:
            result = L"Invalid";
            break;

        case eGameState::SECONDARY_BETTING:
            result = L"Secondary Betting Round";
            break;

        case eGameState::ROUNDS:
            result = L"Playing Game";
            break;

        case eGameState::PAUSED:
            result = L"Paused";
            break;

        case eGameState::FINISHED:
            result = L"Finished";
            break;

        default:
            result = L"UNKNOWN";
            break;
        }

        return result;
    }

    bool GetCardInput(Card& o_card)
    {
        // Instruction on how to enter a card.
        std::string lineInput{};
        std::getline(std::cin, lineInput);

        // Convert input to lower case
        std::transform(lineInput.begin(), lineInput.end(), lineInput.begin(), tolower);

        // Help Support!
        if (lineInput == "?")
        {
            std::wcout
                << "First word: Number {2 - 14, a[ce], j[ack], q[ueen], k[ing]}" << std::endl
                << "Second word: Suit {s[pade[s]], h[eart[s]], d[iamond[s]], c[lub[s]]}" << std::endl
                << "Skipping: {-1, skip}" << std::endl << std::endl
                << "Examples: \"ace diamonds\", \"skip\", \"10 c\", \"7 spade\"" << std::endl;
            return false;
        }

        size_t spaceIndex{lineInput.find(' ')};

        if (lineInput.size() < 1)
        {
            return false;
        }

        std::string numberInput = lineInput.substr(0, spaceIndex);
        std::string typeInput = lineInput.substr(spaceIndex + 1);

        card_number_t cardNum{};
        if (numberInput == "ten" || numberInput == "t")
        {
            cardNum = 10;
        }
        else if (numberInput == "jack" || numberInput == "j")
        {
            cardNum = 11;
        }
        else if (numberInput == "queen" || numberInput == "q")
        {
            cardNum = 12;
        }
        else if (numberInput == "king" || numberInput == "k")
        {
            cardNum = 13;
        }
        else if (numberInput == "ace" || numberInput == "a")
        {
            cardNum = 14;
        }
        else if (numberInput == "skip" || numberInput == "s")
        {
            cardNum = SKIP_BET;
        }
        else
        {
            try
            {
                cardNum = std::stoi(numberInput);
            }
            catch(const std::exception& e)
            {
                return false;
            }
        }

        // If player is skipping no need for type.
        if (cardNum == SKIP_BET)
        {
            o_card = Card{eCardSuit::NO_TYPE, cardNum};
            return true;
        }

        eCardSuit cardSuit{};
        if (typeInput == "spade" || typeInput == "spades" || typeInput == "s")
        {
            cardSuit = eCardSuit::SPADES;
        }
        else if (typeInput == "heart" || typeInput == "hearts" || typeInput == "h")
        {
            cardSuit = eCardSuit::HEARTS;
        }
        else if (typeInput == "diamond" || typeInput == "diamonds" || typeInput == "d")
        {
            cardSuit = eCardSuit::DIAMONDS;
        }
        else if (typeInput == "club" || typeInput == "clubs" || typeInput == "c")
        {
            cardSuit = eCardSuit::CLUBS;
        }
        else if(typeInput == "no type" || typeInput == "n" || typeInput == "na" || typeInput == "no" || typeInput == "n/a")
        {
            cardSuit = eCardSuit::NO_TYPE;
        }
        else
        {
            return false;
        }

        o_card = Card{cardSuit, cardNum};
        return true;
    }


    bool GetNumberInput(int32_t& o_number)
    {
        std::string lineInput{};
        std::getline(std::cin, lineInput);

        int32_t tempNum{};
        try
        {
            tempNum = static_cast<int32_t>(std::stoll(lineInput));
        }
        catch(const std::exception& e)
        {
            return false;
        }

        o_number = tempNum;
        return true;
    }

    void PrintEndScreen(Game& whistGame)
    {
        std::wcout << "Game State: " << GameStateToWString(whistGame.GetGameState()) << std::endl;
        std::wcout << "Ruling Type: " << CardSuitToWString(whistGame.GetRulingType()) << std::endl;
        std::wcout << std::endl;

        std::array<uint8_t, NUM_PLAYERS> playerTakes{whistGame.GetTakes()};
        std::array<Card, NUM_PLAYERS> playerBets{whistGame.GetBets()};
        for (int8_t playerIndex = 0; playerIndex < NUM_PLAYERS; ++playerIndex)
        {
            int16_t numPoints{whistGame.CalculatePoints(playerIndex)};

            std::wcout << "Player " << playerIndex + 1 << ": " << playerTakes[playerIndex]
                       << " out of " << playerBets[playerIndex].m_number << ". Points: "
                       << numPoints << std::endl;
        }

        std::wcout << std::endl;
    }

    void PrintPlayersStatus(Game& whistGame)
    {
        auto& takes{whistGame.GetTakes()};
        auto& bets{whistGame.GetBets()};

        std::wstring takesString    {L"Player Takes: "};
        std::wstring separatorString{L"              "};
        std::wstring betsString     {L"Player Bets:  "};

        std::wstringstream takesLine{};
        std::wstringstream separatorLine{};
        std::wstringstream betsLine{};

        takesLine << takesString;
        separatorLine << separatorString;
        betsLine << betsString;

        for (int8_t playerIndex = 0; playerIndex < NUM_PLAYERS; ++playerIndex)
        {
            takesLine     << std::setw(2) << std::setfill(L'0') << takes[playerIndex]         << " ";
            separatorLine << std::setw(2) << std::setfill(L'0') << "--"                       << " ";
            betsLine      << std::setw(2) << std::setfill(L'0') << bets[playerIndex].m_number << " ";
        }

        std::wcout << takesLine.str()     << std::endl;
        std::wcout << separatorLine.str() << std::endl;
        std::wcout << betsLine.str()      << std::endl;
    }

    std::wstring CardSuitToWString(eCardSuit cardSuit)
    {
        std::wstring type_symbol = L"X";
        switch (cardSuit)
        {
            case eCardSuit::SPADES:
                type_symbol = L"♠";
                break;
            case eCardSuit::HEARTS:
                type_symbol = L"\e[31m♥\e[0m";
                break;
            case eCardSuit::DIAMONDS:
                type_symbol = L"\e[31m♦\e[0m";
                break;
            case eCardSuit::CLUBS:
                type_symbol = L"♣";
                break;
            case eCardSuit::NO_TYPE:
                type_symbol = L"\e[36mN\e[0m";
                break;
            default:
                type_symbol = L"X";
                break;
        }

        return type_symbol;
    }
}
