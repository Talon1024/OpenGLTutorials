#include "input.h"

#include <vector>
#include <map>
#include <functional>
#include <stdexcept>

/*
KeyboardInputHandler::KeyboardInputHandler(std::vector<int> keys)
{
    this->keys = keys;
}
*/

bool KeyboardInputHandler::handle(int key, bool state)
{
    /*
    bool hasHandler = false;
    try
    {
        std::function<void()> handler = nullptr;
        if (state)
        {
            handler = downHandlers.at(key);
        }
        else
        {
            handler = upHandlers.at(key);
        }
        hasHandler = true;
        handler();
    }
    catch (const std::out_of_range)
    {
        hasHandler = false;
    }
    return hasHandler;
    */
    std::function<void()> handler = nullptr;
    if (hasHandler(key, state, &handler))
    {
        handler();
        return true;
    }
    return false;
}

bool KeyboardInputHandler::hasHandler(int key, bool state, std::function<void()>* outHandler) const
{
    bool hasHandler = false;
    try
    {
        std::function<void()> handler = nullptr;
        if (state)
        {
            handler = downHandlers.at(key);
        }
        else
        {
            handler = upHandlers.at(key);
        }
        if (outHandler != nullptr)
        {
            *outHandler = handler;
        }
        hasHandler = true;
    }
    catch (const std::out_of_range)
    {
        hasHandler = false;
    }
    return hasHandler;
}

void KeyboardInputHandler::setHandler(int key, bool state, std::function<void()> handler)
{
    std::map<int, std::function<void()>>* handlerMap = state ? &this->downHandlers : &this->upHandlers;
    (*handlerMap)[key] = handler;
}