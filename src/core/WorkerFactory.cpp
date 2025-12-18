#include "WorkerFactory.h"
#include "../workers/CopyWorker.h"
#include "../workers/MetadataWorker.h"
#include "../workers/FFmpegWorker.h"

Worker* WorkerFactory::createWorker(const Job& job) {
    if (job.type == "TEST_COPY") return new CopyWorker(job);
    if (job.type == "METADATA") return new MetadataWorker(job);
    if (job.type == "FFMPEG") return new FFmpegWorker(job);
    return nullptr;
}
