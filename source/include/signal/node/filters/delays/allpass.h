#pragma once

#include "signal/core/constants.h"
#include "signal/core/graph.h"
#include "signal/buffer/ringbuffer.h"
#include "signal/node/node.h"

#include <vector>

namespace libsignal
{
class AllpassDelay : public UnaryOpNode
{
public:
    AllpassDelay(NodeRef input = 0.0, NodeRef delaytime = 0.1, NodeRef feedback = 0.5, float maxdelaytime = 0.5);
    ~AllpassDelay();

    NodeRef delaytime;
    NodeRef feedback;
    float maxdelaytime;

    std::vector<SampleRingBuffer *> buffers;

    virtual void process(sample **out, int num_frames);
};

REGISTER(AllpassDelay, "allpass-delay")
}
