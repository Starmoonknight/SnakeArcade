// GameRender.cpp
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "GameRender.h"
//#include "Game.h"
#include "GameGrid.h"

#include <Windows.h>    // cursor settings 
#include <cstdlib>      // std::system
#include <iostream>
#include <iomanip>      // format printed output, formatting tools for streams. 
#include <sstream>      // string streams. A stream that works on strings
#include <string>
#include <vector>
#include <algorithm>  




// namespace or static (outside classes only!) on free functions does kind of the same thing? 
// making it an internal to this script only / internal linkage / invisible to outside scripts kind of function 
namespace
{

    // game-over box draver helpers 
    constexpr int kInnerWidth = 29;

    std::string BoxTop()
    {
        return " " + std::string(kInnerWidth + 2, '_');
    }

    std::string BoxLine(const std::string& text)
    {
        std::ostringstream out;
        out << "| " << std::left << std::setw(kInnerWidth) << text << "|";
        return out.str();
    }

    std::string BoxBottom()
    {
        return "|" + std::string(kInnerWidth + 1, '_') + "|";
    }

    // ----------------------------------------------------------------------
    
    std::vector<std::string> SplitLines(const std::string& text)
    {
        std::vector<std::string> lines;
        std::string current; 
    }
    
    // ----------------------------------------------------------------------
    
    void PressEnterPrompt()
    {
        std::cout << "\nPress ENTER to return to menu...";
    }
    
    HANDLE GetConsoleHandle()
    {
        return GetStdHandle(STD_OUTPUT_HANDLE); 
    }

}


namespace GameRender
{

    void ClearConsole()
    {
        std::system("cls");
    }

    void SetCursorPosition(int x, int y)
    {
        const HANDLE hOut = GetConsoleHandle();
        std::cout.flush();

        COORD coord{ x,y };
        SetConsoleCursorPosition(hOut, coord);
    }

    void HideCursor()
    {
        const HANDLE hOut = GetConsoleHandle();

        CONSOLE_CURSOR_INFO info{};
        GetConsoleCursorInfo(hOut, &info);
        info.bVisible = FALSE;
        SetConsoleCursorInfo(hOut, &info);
    }

    void ShowCursor()
    {
        const HANDLE hOut = GetConsoleHandle();

        CONSOLE_CURSOR_INFO info{};
        GetConsoleCursorInfo(hOut, &info);
        info.bVisible = TRUE;
        SetConsoleCursorInfo(hOut, &info);
    }

    void RenderMainMenu()
    {
        ClearConsole();
        std::cout << "\n=== SNAKE MAIN MENU ===\n";
        std::cout << "1) Play" << std::endl;
        std::cout << "2) Controls / Help\n";
        std::cout << "3) Leaderboards\n";
        std::cout << "4) Quit\n";
        std::cout << "Select: ";
    }

    void RenderHelpMenu()
    {
        ClearConsole();
        std::cout << "\n--- Controls / Help ---\n";
        std::cout << "Player move with w/a/s/d\n ";
        std::cout << "In game press q to return to menu\n ";
        std::cout << "(More to come soon...)\n ";
    }

    void RenderScoreBoard(int highScore)
    {
        ClearConsole();
        std::cout << "\n--- Leaderboard ---\n";
        std::cout << "\nBest score: " << highScore << "\n";
    }


    // placeholder 
    void RenderGameOver(int score, int highScore)
    {
        ClearConsole();

        std::cout << BoxTop() << '\n';
        std::cout << BoxLine("") << '\n';
        std::cout << BoxLine("--- Game Over ---") << '\n';
        std::cout << BoxLine("") << '\n';
        std::cout << BoxLine("High Score: " + std::to_string(highScore)) << '\n';
        std::cout << BoxLine("Score:      " + std::to_string(score)) << '\n';
        std::cout << BoxLine("") << '\n';
        std::cout << BoxLine("Enter your name: _ _ _") << '\n';
        std::cout << BoxBottom() << '\n';

        // For now since it does not save anything yet
        std::string input;
        std::getline(std::cin, input);
    }

    void RenderPlaying(const GameConfig& cfg, const GameGrid& grid, int score)
    {
        SetCursorHome();

        std::cout << "WASD = Move    Q = Menu\n\n";

        std::cout << "Score: " << score << "\n\n";

        //m_grid.ClearGrid();
        //PaintFood();
        //PaintSnake();
        
        RenderGrid(cfg, grid);

        std::cout.flush();
    }

}