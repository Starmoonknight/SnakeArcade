#pragma once
#include <cstdint>

namespace GameInput
{

	// not implemented yet 
	enum class GameCommand : uint8_t 
	{
		None,
		Up,
		Down,
		Left,
		Right,
		Quit,
		Pause
	};
	
	//bool TryReadCommand(GameCommand& outCommand); 

	bool TryReadNormalizedChar(char& outChar);
	bool TryReadLatestNormalizedChar(char& outChar);

}