#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <optional>

#include <QString>
#include <SDL2/SDL.h>

#include "Shared/Signal.h"

namespace CarQt {
class GameController {
    SDL_GameController *controller;
    QString name;

  public:
    GameController();

    virtual ~GameController();

    bool opened() const;

    const QString getName() const;

    bool open();

    Shared::Signal getSignal() const;
};
} // namespace CarQt

#endif
