// Input.cpp
#include "MenuInput.h"
#include <iostream>
#include <limits>       // std::numeric_limits  -> ClearInputLine()
#include <cctype>       // std::tolower  // trim 
#include <cstddef>      // std::size_t
 

namespace MenuInput
{ 
    // ----- Input Handling -----

    void ClearInputLine()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string_view TrimLeftWS(std::string_view s)
    {
        std::size_t i{ 0 };
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
            ++i;

        return s.substr(i);
    }

    // turns "1234" into "1 / 2 / 3 / 4"   (or could be "1, 2, 3, 4" if separator like ", " is provided)
    // rename to FormatAllowedChoices
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

    bool ValidateFirstCharInSet(std::string_view input, std::string_view allowedChars)
    {
        return !input.empty() && allowedChars.find(input.front()) != std::string_view::npos;
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


    void ExpectEnterConfirmation()
    {
        std::string input;
        std::getline(std::cin, input);
    }

}

