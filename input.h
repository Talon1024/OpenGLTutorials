#pragma once

#include <vector>
#include <map>
#include <functional>

class KeyboardInputHandler
{
protected:
    // std::vector<int> keys;
    std::map<int, std::function<void()>> downHandlers;
    std::map<int, std::function<void()>> upHandlers;
public:
    // KeyboardInputHandler(std::vector<int> keys);
    // KeyboardInputHandler();
    bool handle(int key, bool state);
    // bool hasHandler(int key, bool state, std::function<void()>* outHandler = nullptr) const;
    void setHandler(int key, bool state, std::function<void()> handler);
};