#pragma once

#include "../core/Worker.h"

class CopyWorker : public Worker {
public:
    CopyWorker(const Job& job);
    void execute() override;
    void plan() override;
};
