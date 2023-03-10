#include "../inc/command.hpp"

const std::string ACCESS_DENIED_MESSAGE = "Error: Access denied.";

std::vector<std::function<canCallFunc>> canCallFunctions;
std::vector<std::function<commandFunc>> commandFunctions;
std::vector<std::string> commands;

std::function<canCallFunc> defaultCanCall = [](Player* player){ return true; };

static Hook PlayerMessageHook(
    &PlayerMessage,
    [](int &playerID, char* &message)
    {
        // Convert char* to std::string, process input
        std::string input(message);
        std::vector<std::string> args;
        std::string curArg;
        bool inQuote = false;
        for (char c : input)
        {
            if (c == ' ' && !inQuote)
            {
                args.push_back(curArg);
                curArg = "";
            }
            else if (c == '\"')
                inQuote = !inQuote;
            else
                curArg += c;
        }
        if (curArg != "")
            args.push_back(curArg);

        Player* player = &Engine::players[playerID];
        for (int i = 0; i < commandFunctions.size(); i++)
        {
            // If the first word matches the command
            if (!args[0].compare(commands[i]))
            {
                if (!canCallFunctions[i](player))
                    player->sendMessage(ACCESS_DENIED_MESSAGE);
                else
                {
                    args.erase(args.begin());
                    commandFunctions[i](player, args);
                }
                return HOOK_OVERRIDE;
            }
        }
        return HOOK_CONTINUE;
    },
    -1
);
