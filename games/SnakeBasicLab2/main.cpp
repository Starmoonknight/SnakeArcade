// SnakeBasicLab2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


/*      Reflection
*
*
*/





// -------------------- Includes --------------------

#include <cstdlib>      // std::system
#include <iostream>
#include <deque>        
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <cctype>       // std::tolower  // trim 
#include <cstddef>      // std::size_t
#include <string_view>      



// -------------------- Types --------------------


struct GameConfig
{
    // game settings 
    int tickSpeed{};

    // grid settings
    int width{ 16 };
    int height{ 12 };
    char empty{ '.' };
    char gridBoarder{ '#' };

    // icons
    char snakeHead{ 'O' };
    char snakeBody{ 'o' };
    char apple{ '?' };
};



struct GameGrid
{
    int width{};
    int height{};
    char empty{};
    Vec2 fruitCoord{};
    std::vector<char> grid;

    GameGrid() = default;

    explicit GameGrid(const GameConfig& cfg)
        : width(cfg.width)
        , height(cfg.height)
        , empty(cfg.empty)
        , grid(static_cast<std::size_t>(cfg.width)* static_cast<std::size_t>(cfg.height), cfg.empty)
    {
    }

    // converts (x,y) to a 1D index.
    int Index(int x, int y) const
    {
        int coord = y * width + x;
        return coord;
    }

    // checks against current board limits
    bool InBounds(const Vec2& pos) const
    {
        return (pos.x >= 0 && pos.x < width)
            && (pos.y >= 0 && pos.y < height);
    }

    // writes a char to a grid cell
    void Set(int x, int y, char c)
    {
        grid[static_cast<std::size_t>(Index(x, y))] = c;
    }

    // returns char at (x,y), assumes caller gives valid coords and has nos no checks. 
    // returns same as board.grid[board.Index(x, y)] would have done 
    char Get(int x, int y) const
    {
        return grid[static_cast<std::size_t>(Index(x, y))];
    }

    void ClearGrid()
    {
        std::fill(grid.begin(), grid.end(), empty);
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



// -------------------- Forward declarations (Table of contents) --------------------


// Input handling
void ClearInputLine();
inline bool ValidateFirstCharInSet(std::string_view input, std::string_view allowedChars);
std::string_view TrimLeftWS(std::string_view s);
std::string JoinAllowedChoices(std::string_view allowedChars, std::string_view separator = " / ");
bool ValidateCharOrPrintCustomMsg(std::string_view input, std::string_view allowedChars, std::string_view errorMessage);
bool ValidateCharOrPrintAutoMsg(std::string_view input, std::string_view allowedChars,
    std::string_view prefix = "Please enter a valid input: ",
    std::string_view suffix = "\n");
void PrintPrompt(std::string_view prompt);
std::string ReadLine();
std::string ReadLine(std::string_view prompt);
char ReadCharChoice(std::string_view prompt, std::string_view allowed);

// Game helpers / math
Vec2 Delta(Direction dir);
void ClampPosition(const GameGrid& grid, Vec2& pos);
void WrapPosition(const GameGrid& grid, Vec2& pos);

// Rendering
void PressEnterPrompt();
void RenderMainMenu();
void RenderHelpMenu();
void RenderScoreBoard(const int& bestScore);
void RenderGrid(const GameGrid& grid, char gridBoarder);

// Gameplay
void PaintSnake(GameGrid& grid, const GameConfig& cfg, const Snake& snake);
void UpdateSnakeDirFromInput(Snake& snake, const char& cmd);
void StepSnake(const GameGrid& grid, Snake& snake, bool growThisTurn = false);
void RunPlayLoop(const GameConfig& cfg, Snake& snake, int& score);



// -------------------- Implementations --------------------


// ----- Input Handling -----

void ClearInputLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// inline = safe to if this same function is defined in multiple files, as long as they’re identical.
//    - static functions VS inline functions, when to use? 
inline bool ValidateFirstCharInSet(std::string_view input, std::string_view allowedChars)
{
    return !input.empty() && allowedChars.find(input.front()) != std::string_view::npos;
}

std::string_view TrimLeftWS(std::string_view s)
{
    std::size_t i{ 0 };
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
        ++i;

    return s.substr(i);
}

// turns "1234" into "1 / 2 / 3 / 4"   (or could be "1, 2, 3, 4" if separator like ", " is provided)
std::string JoinAllowedChoices(std::string_view allowedChars, std::string_view separator)            // this is probably an uneccessary method, better to hardcode? 
{
    std::string out;
    for (std::size_t i = 0; i < allowedChars.size(); ++i)
    {
        if (i > 0) out.append(separator.data(), separator.size());  // appends separator text 
        out.push_back(allowedChars[i]);                                  // appends a single char to the end of the string.
    }
    return out; // returns std::string of the constructed text 
}

bool ValidateCharOrPrintCustomMsg(
    std::string_view input,
    std::string_view allowedChars,
    std::string_view invalidInputMessage)
{
    if (ValidateFirstCharInSet(input, allowedChars))
        return true;

    // static_cast<std::streamsize>: std::ostream::write expects streamsize (signed), but string_view::size() is size_t (unsigned); cast avoids warnings and matches the API type.
    // this is used to avoid internal IED warnings and does not affect runtime behavior 
    std::cout.write(invalidInputMessage.data(), static_cast<std::streamsize>(invalidInputMessage.size()));
    return false;
}

bool ValidateCharOrPrintAutoMsg(
    std::string_view input,
    std::string_view allowedChars,
    std::string_view prefix,
    std::string_view suffix)
{
    if (ValidateFirstCharInSet(input, allowedChars))
        return true;

    std::cout.write(prefix.data(), static_cast<std::streamsize>(prefix.size()));
    std::cout << JoinAllowedChoices(allowedChars);
    std::cout.write(suffix.data(), static_cast<std::streamsize>(suffix.size()));
    return false;
}

void PrintPrompt(std::string_view prompt)
{
    std::cout.write(prompt.data(), static_cast<std::streamsize>(prompt.size()));
}

std::string ReadLine()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string ReadLine(std::string_view prompt)       // split the original into PrintPrompt / ReadLine since prints + reads are tow things, but kept this wrapper for cleaner call sites 
{
    PrintPrompt(prompt);
    return ReadLine();
}

char ReadCharChoice(std::string_view prompt, std::string_view allowedChar)
{
    PrintPrompt(prompt);

    while (true)
    {
        std::string line = ReadLine();
        std::string_view stringView = TrimLeftWS(line);

        /*
        if (!ValidateFirstCharInSet(stringView, allowedChar))
            continue; // silent retry
        */

        if (ValidateCharOrPrintAutoMsg(stringView, allowedChar, "Invalid input. Use: ", "\n"))
        {
            // std::tolower(int) is only defined for: EOF, or values representable as unsigned char
            unsigned char uch = static_cast<unsigned char>(stringView.front());

            // If char is signed on your platform (common), and the byte value is >= 128, it can become negative and calling tolower(negative) is undefined behavior.
            return static_cast<char>(std::tolower(uch)); // invalid input message + retry
        }
    }

    // if no while loop could do:
    /*
    if (sv.empty())
        return '\0';

    unsigned char ch = static_cast<unsigned char>(sv.front());
    return static_cast<char>(std::tolower(ch));
    */
}


// ----- Game helpers / math -----

Vec2 Delta(Direction dir)
{
    switch (dir)
    {
    case Direction::STOP:  return { 0, 0 };
    case Direction::Up:    return { 0, -1 };
    case Direction::Down:  return { 0,  1 };
    case Direction::Left:  return { -1, 0 };
    case Direction::Right: return { 1,  0 };
    }
    return { 0,0 }; // fallback
}

void ClampPosition(const GameGrid& grid, Vec2& pos)
{

    if (pos.x < 0) pos.x = 0;
    if (pos.x >= grid.width) pos.x = grid.width - 1;
    if (pos.y < 0) pos.y = 0;
    if (pos.y >= grid.height) pos.y = grid.height - 1;
}

void WrapPosition(const GameGrid& grid, Vec2& pos)
{

    if (pos.x < 0) pos.x = grid.width - 1;
    if (pos.x >= grid.width) pos.x = 0;
    if (pos.y < 0) pos.y = grid.height - 1;
    if (pos.y >= grid.height) pos.y = 0;
}


// ----- Render Methods -----

void PressEnterPrompt()
{
    std::cout << "\nPress ENTER to return to menu...";
}

void RenderMainMenu()
{
    std::system("cls");
    std::cout << "\n=== SNAKE MAIN MENU ===\n";
    std::cout << "1) Play" << std::endl;
    std::cout << "2) Controls / Help\n";
    std::cout << "3) Leaderboards\n";
    std::cout << "4) Quit\n";
    std::cout << "Select: ";
}

void RenderHelpMenu()
{
    std::system("cls");
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
    std::system("cls");
    std::cout << "\n--- Leaderboard ---\n";
    std::cout << "\nBest score: " << bestScore << "\n";
    PressEnterPrompt();

    std::string input;
    std::getline(std::cin, input);
}

void RenderGrid(const GameGrid& grid, char gridBoarder)       // char gridBoarder, why not const + &   Answer: char is small, a ref pointer is not less expensive so not worth it.      (does a char_view exist?)  
{
    for (int x = 0; x < grid.width + 2; ++x)   // top row rim
        std::cout << gridBoarder << ' ';

    std::cout << "\n";

    for (int y = 0; y < grid.height; ++y)
    {
        std::cout << gridBoarder << ' ';            // left row rim
        for (int x = 0; x < grid.width; ++x)
        {
            std::cout << grid.Get(x, y) << ' ';
        }
        std::cout << gridBoarder << ' ';            // right row rim 
        std::cout << '\n';
    }

    for (int x = 0; x < grid.width + 2; ++x)   // bottom row rim
        std::cout << gridBoarder << ' ';

    std::cout << '\n';
}


// ----- Gameplay -----

void PaintSnake(GameGrid& grid, const GameConfig& cfg, const Snake& snake)
{
    bool isHead = true;
    // PLACEHOLDER: painting all segments as the head char for now
    for (const Vec2& coord : snake.segments)
    {
        if (!grid.InBounds(coord))
            continue;

        grid.Set(coord.x, coord.y, isHead ? cfg.snakeHead : cfg.snakeBody);
        isHead = false;
    }
}

void UpdateSnakeDirFromInput(Snake& snake, const char& cmd)
{
    Direction dir{};

    switch (cmd)
    {
    case 'w': dir = Direction::Up;    break;
    case 's': dir = Direction::Down;  break;
    case 'a': dir = Direction::Left;  break;
    case 'd': dir = Direction::Right; break;
    default:  return;
    }

    snake.dir = dir;
}

void StepSnake(const GameGrid& grid, Snake& snake, bool growThisTurn)
{
    if (snake.dir == Direction::STOP || snake.Empty())
        return;

    const Vec2 headpos = snake.Head();

    Vec2 nextPos = headpos + Delta(snake.dir);
    ClampPosition(grid, nextPos);       // change to GameOver in next step 

    if (nextPos == headpos)
        return;

    // Also add body hit check at next step
    // Also add fruit hit check at next step

    snake.AddHead(nextPos);
    if (!growThisTurn)
        snake.RemoveTail();
}

void RunPlayLoop(const GameConfig& cfg, Snake& snake, int& score)
{
    GameGrid grid{ cfg };

    const Vec2 startPos{ ((cfg.width / 2) - 1),((cfg.height / 2) - 1) };
    snake.segments.clear();
    snake.segments.push_front(startPos);

    bool playing = true;
    while (playing)
    {
        // RENDER
        grid.ClearGrid();
        PaintSnake(grid, cfg, snake);
        RenderGrid(grid, cfg.gridBoarder);

        // READ
        char cmd = ReadCharChoice("Move (W/A/S/D), menu (Q): ", "wasdq");

        // UPDATE 
        if (cmd == 'q')
        {
            playing = false;
            continue;
        }

        UpdateSnakeDirFromInput(snake, cmd);
        StepSnake(grid, snake /* growThisTurn = false */);
        // update score at each succefull movement in next step / when fruit exists change to that 
    }
}



// -------------------- main --------------------


int main()
{
    std::cout << "Hallowed Woid! Play some Snake\n";

    GameConfig cfg;
    GameState state = GameState::MainMenu;
    //GameGrid grid(cfg);
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

            std::string_view stringView = TrimLeftWS(choiceStr);
            if (!ValidateCharOrPrintCustomMsg(stringView, "1234", "Please enter a valid value number between 1-4\n"))
                continue;

            choice = stringView.front();

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
            int score{ 0 };
            RunPlayLoop(cfg, snake, score);

            std::cout << "Score: " << score << std::endl;
            if (score > bestScore)
                bestScore = score;

            state = GameState::MainMenu;

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
