#ifndef RECORDING_H_
#define RECORDING_H_

#include <chrono>
#include <vector>

#include <QGraphicsView>
#include <qstring.h>

#include "Shared/Data.h"

namespace CarQt {
class Recording {
    struct Entry {
        std::chrono::time_point<std::chrono::system_clock> time;
        Shared::Data data;
    };

    std::chrono::time_point<std::chrono::system_clock> start;
    std::vector<Entry> entries;

  public:
    Recording();

    virtual ~Recording() = default;

    void save(const QString &path);

    void add(const Shared::Data &data);
};
} // namespace CarQt

#endif // !RECORDING_H_
