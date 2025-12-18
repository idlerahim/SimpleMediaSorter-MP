#pragma once

#include "../core/Worker.h"

class SorterWorker : public Worker {
public:
    SorterWorker(const Job& job);
    void execute() override;
    void plan() override;
};
