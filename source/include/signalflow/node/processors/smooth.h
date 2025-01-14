#pragma once

#include "signalflow/core/constants.h"
#include "signalflow/node/node.h"
#include <limits>

namespace signalflow
{
class Smooth : public UnaryOpNode
{
public:
    Smooth(NodeRef input = nullptr, NodeRef smooth = 0.99);

    virtual void alloc() override;
    virtual void process(Buffer &out, int num_frames) override;

private:
    NodeRef smooth;
    std::vector<sample> values;
};

REGISTER(Smooth, "smooth")
}
