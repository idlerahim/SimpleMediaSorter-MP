#pragma once

#include "Worker.h"
#include <memory>

class WorkerFactory {
public:
    static Worker* createWorker(const Job& job);
};
