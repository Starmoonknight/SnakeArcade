// GameRenderer.cpp
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "GameRenderer.h"
#include "Game.h"
#include "GameGrid.h"

#include <Windows.h>    // cursor settings 
#include <cstdlib>      // std::system
#include <iostream>
#include <iomanip>      // format printed output, formatting tools for streams. 
#include <sstream>      // string streams. A stream that works on strings
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
    
    struct ConsoleSize
    {
        int width{};
        int height{};
    };
    
    HANDLE GetConsoleHandle()
    {
        return GetStdHandle(STD_OUTPUT_HANDLE); 
    }

    ConsoleSize GetConsoleSize()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi{};
        if (!GetConsoleScreenBufferInfo(GetConsoleHandle(), &csbi))         // change to an error message that pauses the game window 
            return { 80, 25 };

        const int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        const int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        return { width, height };
    }

    // ----------------------------------------------------------------------

    std::vector<std::string> SplitLines(const std::string& text)    // not needed after change, but might be good for ascii art later? 
    {
        std::vector<std::string> lines;
        std::string current;

        for (char ch : text)
        {
            if (ch == '\n')
            {
                lines.push_back(current);   // if it is '\n', current line is finished, so push it into the vector and start a new empty line
                current.clear();
            }
            else
            {
                current.push_back(ch);      // if the character is not '\n', add it to the current line
            }
        }

        if (!current.empty())
            lines.push_back(current);       // if something was left behind in the line, push that too 

        return lines;
    }

    int MaxLineWidth(const std::vector<std::string>& lines)
    {
        int maxWidth{ 0 };
        for (const std::string& line : lines)
        {
            if (static_cast<int>(line.size()) > maxWidth)
                maxWidth = static_cast<int>(line.size());
        }
        return maxWidth;
    }

    std::vector<std::string> WindowSizeErrorPanelLines(const int frameWidth, const int frameHeight, const ConsoleSize console)
    {
        return
        {
            "Console window too small.",
            "Need at least " + std::to_string(frameWidth) + " x " + std::to_string(frameHeight),
            "Current size: " + std::to_string(console.width) + " x " + std::to_string(console.height),
            "Please resize the window.",
            ""
        };
    }

    std::vector<std::string> PressEnterPromptPanelLines()
    {
        return
        {
            "Press ENTER to return to menu..."
        };
    }

    std::vector<std::string> MakePlayingInfoPanelLines(int score)
    {
        return
        {
            "Score: " + std::to_string(score),
            "",
            "Controlls:",
            "W A S D = Move ",
            "Q = Menu"
        };
    }

    std::vector<std::string> MainMenuPanelLines()
    {
        return
        {
            "=== SNAKE MAIN MENU ===",
            "",
            "1) Play",
            "2) Controls / Help",
            "3) Leaderboards",
            "4) Quit",
            "",
            "Select: "
        };
    }

    std::vector<std::string> HelpMenuPanelLines()
    {
        return
        {
            "--- Controls / Help ---",
            "",
            "Player move with w/a/s/d",
            "In game press q to return to menu",
            "(More to come soon...)",
            "",
        }; 
    }

    std::vector<std::string> ScorePanelLines(int highScore)
    {
        return
        {
            "--- Leaderboard ---",
            "",
            "Best score: " + std::to_string(highScore),
            "",
        };
    }

    std::vector<std::string> GameOverPanelLines(int score, int highScore)
    {
        return
        {
            BoxTop(),
            BoxLine(""),
            BoxLine("--- Game Over ---"),
            BoxLine(""),
            BoxLine("High Score: " + std::to_string(highScore)),
            BoxLine("Score:      " + std::to_string(score)),
            BoxLine(""),
            BoxLine("Enter your name: _ _ _"),
            BoxBottom()
        };
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

        COORD coord;
        coord.X = static_cast<SHORT>(x);
        coord.Y = static_cast<SHORT>(y);
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

    BlockLayout RenderCenteredBlock(const std::vector<std::string>& panelLines)
    {
        BlockLayout layout;

        layout.height = static_cast<int>(panelLines.size());
        layout.width = MaxLineWidth(panelLines);

        const ConsoleSize console = GetConsoleSize();
        layout.originX = std::max(0, (console.width - layout.width) / 2);
        layout.originY = std::max(0, (console.height - layout.height) / 2);

        for (int i = 0; i < layout.height; ++i)
        {
            SetCursorPosition(layout.originX, layout.originY + i);
            std::cout << panelLines[i];
        }

        std::cout.flush();
        return layout;
    }

    BlockLayout RenderFollowupBlock(const BlockLayout& previousLayout, const std::vector<std::string>& lines)
    {
        BlockLayout layout;
        
        layout.height = static_cast<int>(lines.size());
        layout.width = MaxLineWidth(lines);

        const ConsoleSize console = GetConsoleSize();
        layout.originX = previousLayout.originX + (previousLayout.width - layout.width) / 2;
        layout.originY = previousLayout.originY + previousLayout.height + 1;   // below last row of previous prompt block + one blank line 
        //if (layout.originX < 0) layout.originX = 0;
        //if (layout.originY < 0) layout.originY = 0;
        // need other soft bounds check

        for (int i = 0; i < static_cast<int>(lines.size()); ++i)
        {
            SetCursorPosition(layout.originX, layout.originY + i);
            std::cout << lines[i];
        }

        std::cout.flush();
        return layout;
    }
    
    void RenderPressEnterPrompt(const BlockLayout layout)
    {
        RenderFollowupBlock(layout, PressEnterPromptPanelLines());
    }

    BlockLayout RenderWindowSizeError(const int frameWidth, const int frameHeight, const ConsoleSize console)
    {
        return RenderCenteredBlock(WindowSizeErrorPanelLines(frameWidth, frameHeight, console)); 
    }

    BlockLayout RenderMainMenu()
    {
        ClearConsole();
        return RenderCenteredBlock(MainMenuPanelLines());
    }

    BlockLayout RenderHelpMenu()
    {
        ClearConsole();
        return RenderCenteredBlock(HelpMenuPanelLines());
    }

    BlockLayout RenderScoreBoard(int highScore)
    {
        ClearConsole();
        return RenderCenteredBlock(ScorePanelLines(highScore));
    }


    // placeholder 
    BlockLayout RenderGameOver(int score, int highScore)
    {
        ClearConsole();
        return RenderCenteredBlock(GameOverPanelLines(score, highScore));
    }


    void RenderPlaying(const GameConfig& cfg, const GameGrid& grid, int score)
    {
        const std::vector<std::string> boardLines = grid.ToLines(cfg); 
        const std::vector<std::string> panelLines = MakePlayingInfoPanelLines(score); 

        const int gap{ 4 };
        const int boardHeight = static_cast<int>(boardLines.size()); 
        const int panelHeight = static_cast<int>(panelLines.size()); 
        const int totalFrameHeight = std::max(boardHeight, panelHeight); 

        const int boardWidth = (cfg.width + 2) * 2;

        std::vector<std::string> gameplayVisuals(totalFrameHeight);

        for (int i = 0; i < totalFrameHeight; ++i)
        {
            if (i < boardHeight)
                gameplayVisuals[i] += boardLines[i];                    // start by adding in the grid symbols
            else
                gameplayVisuals[i] += std::string(boardWidth, ' ');     // in case the panel is taller than the board, preserve left column width 

            gameplayVisuals[i] += std::string(gap, ' ');                // add the gap between main grid and side info  

            if (i < panelHeight)                                        // append existing side panel lines  
                gameplayVisuals[i] += panelLines[i];
        }

        RenderCenteredBlock(gameplayVisuals); 
    }


}