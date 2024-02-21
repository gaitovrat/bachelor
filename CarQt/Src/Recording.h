#ifndef RECORDING_H_
#define RECORDING_H_

#include <chrono>
#include <vector>

#include <qstring.h>

#include "Data.h"

class Recording {
	struct Entry {
		std::chrono::time_point<std::chrono::system_clock> time;
		Data data;
	};

	std::chrono::time_point<std::chrono::system_clock> start;
	std::vector<Entry> entries;

public:
	Recording();

	virtual ~Recording() = default;

	void save(const QString& path);

	void add(const Data& data);
};

#endif // !RECORDING_H_
