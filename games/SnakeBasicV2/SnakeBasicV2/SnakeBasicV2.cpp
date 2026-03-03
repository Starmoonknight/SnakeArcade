// SnakeBasicV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <cctype>       // std::tolower
#include <cstddef>      // std::size_t
#include <string_view>      // string_view is C++17+, but some interactions (cout << view, string += view) are C++20/library-dependent;
                            // - std::cout << std::string_view and std::string += std::string_view may not work in C++17,
                            // use .write() / .append(data,size) to stay compatible with C++17 builds.


//constexpr int WIDTH{ 10 };
//constexpr int HEIGHT{ 10 }; 


// ----- Input Handling -----

void ClearInputLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// inline = safe to if this same function is defined in multiple files, as long as they’re identical.
//    - static functions VS inline functions, when to use? 
inline bool IsFirstCharInSet(std::string_view input, std::string_view allowedChars)
{
    return !input.empty() && allowedChars.find(input.front()) != std::string_view::npos;
}

// turns "1234" into "1 / 2 / 3 / 4"   (or could be "1, 2, 3, 4" if separator like ", " is provided)
std::string JoinAllowedChoices(std::string_view allowed, std::string_view separator = " / ")
{
    std::string out;
    for (std::size_t i = 0; i < allowed.size(); ++i)
    {
        if (i > 0) out.append(separator.data(), separator.size());  // appends separator text 
        out.push_back(allowed[i]);                                  // appends a single char to the end of the string.
    }
    return out; // returns std::string of the constructed text 
}

bool ValidateOrPrintCustomMsg(
    std::string_view input,
    std::string_view allowedChars,
    std::string_view errorMessage)
{
    if (IsFirstCharInSet(input, allowedChars))
        return true;

    // static_cast<std::streamsize>: std::ostream::write expects streamsize (signed), but string_view::size() is size_t (unsigned); cast avoids warnings and matches the API type.
    // this is used to avoid internal IED warnings and does not affect runtime behavior 
    std::cout.write(errorMessage.data(), static_cast<std::streamsize>(errorMessage.size()));
    return false;
}

bool ValidateOrPrintAutoMsg(
    std::string_view input,
    std::string_view allowedChars,
    std::string_view prefix = "Please enter a valid input: ",
    std::string_view suffix = "\n")
{
    if (IsFirstCharInSet(input, allowedChars))
        return true;

    std::cout.write(prefix.data(), static_cast<std::streamsize>(prefix.size()));
    std::cout << JoinAllowedChoices(allowedChars);
    std::cout.write(suffix.data(), static_cast<std::streamsize>(suffix.size()));
    return false;
}



// ----- Game Types -----

struct Vec2
{
    int x{};
    int y{};
};

void ClampPosition(const GameConfig& cfg, Vec2& pos)
{

    if (pos.x < 0) pos.x = 0;
    if (pos.x >= cfg.width) pos.x = cfg.width - 1;
    if (pos.y < 0) pos.y = 0;
    if (pos.y >= cfg.height) pos.y = cfg.height - 1;
}


enum class Direction { Up, Down, Left, Right }; // strong typed enums? Look up more later

Vec2 Delta(Direction dir)
{
    switch (dir)
    {
    case Direction::Up:    return { 0, -1 };
    case Direction::Down:  return { 0,  1 };
    case Direction::Left:  return { -1, 0 };
    case Direction::Right: return { 1,  0 };
    }
    return { 0,0 };
}


struct Snake
{
    std::vector<Vec2> segments{};
    Direction dir = Direction::Up;
};

struct GameBoard
{
    int width{};
    int height{};
    char empty{};
    std::vector<char> cells;

    GameBoard() = default;

    explicit GameBoard(const GameConfig& cfg)
        : width(cfg.width)
        , height(cfg.height)
        , empty(cfg.emptyCell)
        , cells(static_cast<std::size_t>(cfg.width)* static_cast<std::size_t>(cfg.height), cfg.emptyCell)
    {
    }

    // railing const on a member function affects this object? Treating GameBoard as const GameBoard  
    int Index(int x, int y) const
    {
        int coord = y * width + x;
        return coord;
    }

    bool InBounds(const Vec2& pos) const
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    void Set(int x, int y, char c)
    {
        cells[static_cast<std::size_t>(Index(x, y))] = c;
    }

    char Get(int x, int y) const
    {
        return cells[static_cast<std::size_t>(Index(x, y))];
    }

    void Clear()
    {
        std::fill(cells.begin(), cells.end(), empty);
    }
};

enum GameState
{
    MainMenu,
    SubMenu,
    Playing,
    Paused,
    Quit,
};

struct GameConfig
{
    int width = 12;
    int height = 8;
    char emptyCell = '.';
    char playerCell = '@';
    char boarderCell = '#';
    char appleCell = '?';
};


// ----- Render Methods -----

void PressEnterPrompt()
{
    std::cout << "\nPress ENTER to return to menu...";
}

void ClearGrid(const GameConfig& cfg, std::vector<char>& grid)
{
    grid.assign(cfg.width * cfg.height, cfg.emptyCell);
}

void RenderMainMenu()
{
    system("cls");
    std::cout << "\n=== SNAKE MAIN MENU ===\n";
    std::cout << "1) Play" << std::endl;
    std::cout << "2) Controls / Help\n";
    std::cout << "3) Leaderboards\n";
    std::cout << "4) Quit\n";
    std::cout << "Select: ";
}

void RenderHelpMenu()
{
    system("cls");
    std::cout << "\n--- Controls / Help ---\n";
    std::cout << "Player move with w/a/s/d\n ";
    std::cout << "In game press q to return to menu\n ";
    std::cout << "(More to come soon...)\n ";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}

void RenderScoreBoard(const int& bestScore)
{
    system("cls");
    std::cout << "\n--- Leaderboard ---\n";
    std::cout << "\nBest score: " << bestScore << "\n";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}

void RenderGameGrid(GameBoard& board, const std::deque<Vec2>& snakeBody, const GameConfig& cfg)
{
    system("cls");
    std::cout << "\n";

    std::vector<bool> occupied(cfg.width * cfg.height, false);

    auto idx

        for (int y = 0; y < cfg.height; ++y)
        {
            for (int x = 0; x < cfg.width; ++x)
            {
                if (Vec2(x, y)  snake.bodyParts[x, y])
                {
                    std::cout << cfg.playerCell;
                }
                else
                {
                    std::cout << cfg.emptyCell;
                }
            }
            std::cout << std::endl;
        }
    std::cout << "Use WASD to move, Q to go back to main menu\n";
}


// ----- Gameplay Logic -----

bool UpdatePlayerFromInput(GameConfig& cfg, Vec2& playerPos, char cmd)
{
    if (cmd == 'q')
        return false;

    Vec2 next = playerPos;

    switch (cmd)
    {
    case 'w': next.y -= 1; break;
    case 's': next.y += 1; break;
    case 'a': next.x -= 1; break;
    case 'd': next.x += 1; break;
    default:
        return true;
    }

    playerPos = next;
    //WrapPosition(cfg, playerPos);
    //ClampPosition(cfg, playerPos);


}




int main()
{
    std::cout << "Hallowed Woid! Play some Snake\n";

    GameConfig cfg;
    GameState state = GameState::MainMenu;
    GameBoard board(cfg);
    Snake snake{};

    char choice{};
    int bestScore{};
    bool running{ true };

    while (running)
    {

        switch (state)
        {
        case GameState::MainMenu:
        {
            RenderMainMenu();

            std::string choiceStr;
            std::getline(std::cin, choiceStr);

            if (!ValidateOrPrintCustomMsg(choiceStr, "1234", "Please enter a valid value number between 1-4\n"))
                continue;

            choice = choiceStr[0];

            if (choice == '1') state = GameState::Playing;
            else if (choice == '2' || choice == '3') state = GameState::SubMenu;
            else if (choice == '4') state = GameState::Quit;

            break;
        }
        case GameState::SubMenu:
        {
            if (choice == '2')
            {
                RenderHelpMenu();
            }
            else if (choice == '3')
            {
                RenderScoreBoard(bestScore);
            }

            state = GameState::MainMenu;
            break;
        }
        case GameState::Playing:
        {
            // PLAYER
            //int playerX = (cfg.width / 2) - 1;
            //int playerY = (cfg.height / 2) - 1;

            snake.segments.assign(1, Vec2{ ((cfg.width / 2) - 1),((cfg.height / 2) - 1) });


            int score{ 0 };

            bool playing = true;
            while (playing)
            {
                RenderGameGrid(snake, cfg);


                std::string input;
                std::getline(std::cin, input);
                if (input.size() == 0)
                    continue;

                char c = tolower(input[0]);

                // UPDATE
                if (c == 'q')
                {
                    playing = false;
                    state = GameState::MainMenu;
                }
                else if (c == 'w')
                {
                    playerY -= 1;
                    score += 1;
                }
                else if (c == 's')
                {
                    playerY += 1;
                    score += 1;
                }
                else if (c == 'a')
                {
                    playerX -= 1;
                    score += 1;
                }
                else if (c == 'd')
                {
                    playerX += 1;
                    score += 1;
                }

                ClampPosition();

            }
            std::cout << "Score: " << score << std::endl;
            if (score > bestScore)
                bestScore = score;

            break;
        }
        case GameState::Paused:
        {

            break;
        }
        case GameState::Quit:
        {
            running = false;

            break;
        }
        }
    }
}
