#ifndef RECORDING_H_
#define RECORDING_H_

#include <qstring.h>

#include <QGraphicsView>
#include <chrono>
#include <vector>

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
    /*
     * Constructor
     */
    Recording();

    /*
     * Destructor
     */
    virtual ~Recording() = default;

    /*
     * Save recording to a file
     * @param path the path to save to
     */
    void save(const QString &path);

    /*
     * Add data to the recording
     * @param data the data to add
     */
    void add(const Shared::Data &data);
};
}  // namespace CarQt

#endif  // !RECORDING_H_
