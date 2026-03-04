// SnakeBasicV2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



// NOTES: 
// - static_cast    Makes conversions explicit (often to satisfy APIs / silence warnings): “convert expr to type T using normal, compile-time rules.”. 
//                  But also for correctness case when casting since it will not silently cast away const? Importent with <cctype> functions: std::isspace, std::toupper, etc. 
//                  Values representable as unsigned char. If char is signed on a platform and has a negative value (common for non-ASCII bytes), calling isspace(ch) can be undefined behavior.
// 
// - const          In params: prevents modifying that parameter (esp. const& = read-only view of caller's object).
//                  Can also be put after a method, trailing const on after a member fn: promises it won’t modify *this*; inside, 'this' is treated as pointer-to-const (can call only const members) ? 
//                  Example: Treating GameBoard as a const GameBoard in .Get  
// 
// - &               Reference: aliases an existing object (no copy). In params, T& lets you modify caller; const T& reads without copying.
//
// - string_view    string_view is C++17+, but some interactions (cout << view, string += view) are C++20/library-dependent;
//                      - std::cout << std::string_view and std::string += std::string_view may not work in C++17,
//                  use .write() / .append(data,size) to stay compatible with C++17 builds.
//
// -                (void) before the function call ignores the return value.




// -------------------- Includes --------------------

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
    int width{ 12 };
    int height{ 8 };
    char empty{ '.' };
    char gridBoarder{ '#' };
    char snakeHead{ 'O' };
    char snakeBody{ 'o' };
    char apple{ '?' };
};

struct Vec2
{
    int x{};
    int y{};
};

// allow the value of a Vec2 to be added to another Vec2, just like an 'int a += int b'
inline Vec2& operator+=(Vec2& a, const Vec2& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

// create a new Vec2 from two provided Vec2's, just like an 'int a + int b' 
[[nodiscard]] inline Vec2 operator+(Vec2 a, const Vec2& b)
{
    // a is a copy of the left operand, modify the copy with += and then return the copy    (return a new value without changing the originals.)
    a += b;
    return a;
}

// allow the value of a Vec2 to be removed from another Vec2, just like an 'int a -= int b'
inline Vec2& operator-=(Vec2& a, const Vec2& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

// create a new Vec2 from two provided Vec2's, just like an 'int a - int b' 
[[nodiscard]] inline Vec2 operator-(Vec2 a, const Vec2& b)
{
    a -= b;
    return a;
}

// allow comparison checks for Vec2, just like if( int a == int b )
inline bool operator==(const Vec2& a, const Vec2& b)
{
    return a.x == b.x && a.y == b.y;
}

// allow comparison checks for Vec2, just like if( int a != int b )
inline bool operator!=(const Vec2& a, const Vec2& b)
{
    return !(a == b);
}

enum class Direction { STOP, Up, Down, Left, Right }; // strong typed enums? Look up more later

struct Snake
{
    std::deque<Vec2> segments{};
    Direction dir = Direction::Up;
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
inline bool IsFirstCharInSet(std::string_view input, std::string_view allowedChars);
std::string_view TrimLeftWS(std::string_view s);
std::string JoinAllowedChoices(std::string_view allowed, std::string_view separator = " / ");
bool ValidateOrPrintCustomMsg(std::string_view input, std::string_view allowedChars, std::string_view errorMessage);
bool ValidateOrPrintAutoMsg(std::string_view input, std::string_view allowedChars,
    std::string_view prefix = "Please enter a valid input: ",
    std::string_view suffix = "\n");
char ReadCommand(std::string_view prompt);

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
void RunPlayLoop(const GameConfig& cfg, Snake& snake); 



// -------------------- Implementations --------------------


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

std::string_view TrimLeftWS(std::string_view s)
{
    std::size_t i{ 0 };
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
        ++i;

    return s.substr(i); 
}

// turns "1234" into "1 / 2 / 3 / 4"   (or could be "1, 2, 3, 4" if separator like ", " is provided)
std::string JoinAllowedChoices(std::string_view allowed, std::string_view separator = " / ")            // this is probably an uneccessary method, better to hardcode? 
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

char ReadCommand(std::string_view prompt)
{
    *....*
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

    if (pos.x < 0) pos.x = grid.width -1;
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
    if (snake.dir == Direction::STOP || snake.segments.empty())
        return;

    const Vec2 headpos = snake.segments.front();
    const Vec2 moveDir = Delta(snake.dir); 

    Vec2 nextPos = headpos + moveDir;

    ClampPosition(grid, nextPos);       // change to gameover in next step 

    if (nextPos == headpos)             
        return; 

    // Also add body hit check at next step
    // Also add fruit hit check at next step

    snake.segments.push_front(nextPos);

    if(!growThisTurn)
        snake.segments.pop_back();
}

void RunPlayLoop(const GameConfig& cfg, Snake& snake)
{
    GameGrid grid{ cfg };

    const Vec2 startPos{ ((cfg.width / 2) - 1),((cfg.height / 2) - 1) };  
    snake.segments.clear();
    snake.segments.push_front(startPos);
    
    bool playing = true;
    while (playing)
    {
        // render grid
        grid.ClearGrid(); 
        PaintSnake(grid, cfg, snake);
        RenderGrid(grid, cfg.gridBoarder); 

        // read input
        char cmd = ReadCommand(*...*); 

        // update 
        if (cmd == 'q')
        {
            playing = false;
            continue;
        }

        UpdateSnakeDirFromInput(snake, cmd); 
        StepSnake(grid, snake /* growThisTurn = false */);
    }
}



// -------------------- main --------------------


int main()
{
    std::cout << "Hallowed Woid! Play some Snake\n";

    GameConfig cfg;
    GameState state = GameState::MainMenu;
    GameGrid grid(cfg);
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
            if (!ValidateOrPrintCustomMsg(stringView, "1234", "Please enter a valid value number between 1-4\n"))
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
