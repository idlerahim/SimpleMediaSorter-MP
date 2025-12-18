#pragma once

#include "../core/Worker.h"

class MetadataWorker : public Worker {
public:
    MetadataWorker(const Job& job);
    void execute() override;
    void plan() override;
};
