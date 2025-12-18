#pragma once

#include "../core/Worker.h"

class ImageWorker : public Worker {
public:
    ImageWorker(const Job& job);
    void execute() override;
    void plan() override;
};
