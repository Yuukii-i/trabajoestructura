#pragma once
#include "State.h"
#include <memory>

class StateManager {
public:
    void setState(std::unique_ptr<State> newState) {
        m_currentState = std::move(newState);
    }

    State* getState() {
        return m_currentState.get();
    }

private:
    std::unique_ptr<State> m_currentState;
};
