#include "Worker.h"

void Worker::run() {
    emit started(m_job.id);
    
    if (m_mode == Mode::Planning) {
        plan();
        // planningFinished is emitted by the specific worker implementation usually, 
        // or we can expect m_job.items to be populated and emit here.
        // Better to let the worker emit it or we emit it here using m_job.items
        emit planningFinished(m_job.id, m_job.items);
    } else {
        execute();
        emit finished(m_job.id, m_success);
    }
}
