// Input.h
#pragma once
#include <string>
#include <string_view>  


// Input handling
void ClearInputLine();
std::string_view TrimLeftWS(std::string_view s);
std::string JoinAllowedChoices(std::string_view allowedChars, std::string_view separator = " / ");

bool ValidateFirstCharInSet(std::string_view input, std::string_view allowedChars);
bool ValidateCharOrPrintCustomMsg(std::string_view input, std::string_view allowedChars, std::string_view errorMessage);
bool ValidateCharOrPrintAutoMsg(std::string_view input, std::string_view allowedChars,
    std::string_view prefix = "Please enter a valid input: ",
    std::string_view suffix = "\n");

void PrintPrompt(std::string_view prompt);

std::string ReadLine();
std::string ReadLine(std::string_view prompt);
char ReadCharChoice(std::string_view prompt, std::string_view allowed);
