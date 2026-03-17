#pragma once


namespace GameInput
{

	enum class GameCommand
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