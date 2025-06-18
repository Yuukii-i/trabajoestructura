#pragma once
#include <SFML/Graphics.hpp>

class State {
public:
    virtual void handleEvent(sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual bool shouldExit() const { return false; }
    virtual ~State() {}
};