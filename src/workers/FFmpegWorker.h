#pragma once

#include "../core/Worker.h"

class FFmpegWorker : public Worker {
public:
    FFmpegWorker(const Job& job);
    void execute() override;
    void plan() override;
};
