// SnakeBasicLab2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


/*      Reflection
*
*
*/


//  NOTES:
//
// - Decleration Order     
//          Order matters for free functions but less so with member functions, those in structs and classes. 
//          Since all parts are read in at once? they can be declared out of order
// - Static 
//          Free functions that have: 'Static' makes it local to file only. Could also be using NameSpaces 
// 
// - Lambdas
//          Smaller unamed nested local functions that has scope access to variables and data members
// 
// - RNG  
//          Using a seed value make multiple individual rng to keep call order consistent between games 
// 
//      





// -------------------- Includes --------------------

#include "Game.h"
#include <iostream> 



// -------------------- Types --------------------






// -------------------- Forward declarations (Table of contents) --------------------




// -------------------- Implementations --------------------



// -------------------- main --------------------


int main()
{
    std::cout << "Hallowed Woid! Play some Snake\n";

    Game game;
    game.Run(); 
}
