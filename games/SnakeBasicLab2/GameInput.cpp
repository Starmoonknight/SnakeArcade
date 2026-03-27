#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "GameInput.h"

#include <conio.h>   // _kbhit, _getch
#include <cctype>    // std::tolower



namespace GameInput
{

	namespace
	{
		bool TryMapBasicKey(int ch, char& outChar)
		{
			unsigned char uch = static_cast<unsigned char>(ch);
			char c = static_cast<char>(std::tolower(uch)); 

			switch (c)
			{
			case 'w': case '8': outChar = 'w'; return true;	// up
			case 'a': case '4': outChar = 'a'; return true;	// left
			case 's': case '2': outChar = 's'; return true;	// down
			case 'd': case '6': outChar = 'd'; return true;	// right
			case 'q':			outChar = 'q'; return true;	// quit
			//case 'p':			outChar = 'p'; return true;	// pause
			default:			return false; 
			}
		}

		bool TryMapExtendedKey(int ext, char& outChar)
		{
			constexpr int kArrowUp = 72;
			constexpr int kArrowLeft = 75;
			constexpr int kArrowDown = 80;
			constexpr int kArrowRight = 77;

			// could also use Win32/_getch arrow mapping.
			switch (ext)
			{
			case kArrowUp:	  outChar = 'w'; return true; // arrow up
			case kArrowLeft:  outChar = 'a'; return true; // arrow left
			case kArrowDown:  outChar = 's'; return true; // arrow down
			case kArrowRight: outChar = 'd'; return true; // arrow right
			default: return false;
			}
		}
	}

	/*
	bool TryReadCommand(GameCommand& outCommand)
	{

	}
	*/

	bool TryReadNormalizedChar(char& outChar)
	{
		if (!_kbhit())
			return false; 

		int ch = _getch();

		if (ch == 0 || ch == 0xE0)	// if input is special symbols like arrow keys:  Need to call again to consume full arrow/function-key sequence? There seems to be a two-call behavior for arrow/function keys. 
		{
			int ext = _getch();		// consume second part.
			return TryMapExtendedKey(ext, outChar); 
		}

		// when no special symbols where noticed, use the simpler compare 
		return TryMapBasicKey(ch, outChar);
	}

	bool TryReadLatestNormalizedChar(char& outChar)
	{
		bool found = false; 
		char latest{};

		// while any key is still stored in the buffer, continue to loop through them all for any match
		while (_kbhit())
		{
			char current{};
			if (TryReadNormalizedChar(current))
			{
				// save inputs that match commands, but overwrite if newer are found. Allow keyboard bashing 
				// Latest input == used input. 
				latest = current;
				found = true;
			}
		}
		// after loop is done all inputs in the buffer should have been consumed 

		if (found)
			outChar = latest;	// update the stored char directly by ref, but only if a valid command was found 

		return found;			// the bool tells if the outChar is ok to use or not
	}


}