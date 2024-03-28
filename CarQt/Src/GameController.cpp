#include "GameController.h"

#include <QDebug>

using namespace CarQt;

GameController::GameController() : controller(nullptr) {
    SDL_Init(SDL_INIT_JOYSTICK);
}

GameController::~GameController() {
    if (this->controller) {
        SDL_GameControllerClose(this->controller);
    }
}

bool GameController::open() {
    int numJoysticks;

    if (this->opened()) {
        return true;
    }

    numJoysticks = SDL_NumJoysticks();
    if (numJoysticks > 0) {
        this->controller = SDL_GameControllerOpen(0);
        this->name = SDL_GameControllerName(this->controller);
    }

    return this->opened();
}

bool GameController::opened() const { return this->controller != nullptr; }

Shared::Signal GameController::getSignal() const {
    Shared::Signal signal;
    SDL_Event event;

    if (!this->controller) {
        return {};
    }

    while (SDL_PollEvent(&event)) {
        if (event.type != SDL_CONTROLLERAXISMOTION &&
            event.type != SDL_JOYAXISMOTION) {
            continue;
        }

        const uint8_t button = event.caxis.axis;
        signal.value = event.caxis.value;

        switch (button) {
        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            signal.type = Shared::SignalType::INCREASE_SPEED;
            break;
        case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            signal.type = Shared::SignalType::DECREASE_SPEED;
            break;
        case SDL_CONTROLLER_AXIS_LEFTX:
            signal.type = Shared::SignalType::TURN;
            break;
        default:
            break;
        }
    }

    return signal;
}

const QString GameController::getName() const { return this->name; }
