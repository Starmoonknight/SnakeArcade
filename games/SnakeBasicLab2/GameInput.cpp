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
			case 'p':			outChar = 'p'; return true;	// pause
			default:			return false; 
			}
		}

		bool TryMapExtendedKey(int ext, char& outChar)
		{
			// could also use Win32/_getch arrow mapping.
			switch (ext)
			{
			case 72: outChar = 'w'; return true; // up
			case 75: outChar = 'a'; return true; // left
			case 80: outChar = 's'; return true; // down
			case 77: outChar = 'd'; return true; // right
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

		if (ch == 0 || ch == 0xE0)
		{
			int ext = _getch();		// consume second part.		Need to call again to consume full arrow/function-key sequence? There seems to be a two-call behavior for arrow/function keys. 
			return TryMapExtendedKey(ext, outChar); 
		}

		return TryMapBasicKey(ch, outChar);
	}

	bool TryReadLatestNormalizedChar(char& outChar)
	{
		bool found = false; 
		char latest{};

		while (_kbhit())
		{
			char current{};
			if (TryReadNormalizedChar(current))
			{
				latest = current;
				found = true;
			}
		}

		if (found)
			outChar = latest;

		return found;
	}


}