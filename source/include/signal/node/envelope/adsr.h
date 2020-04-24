#pragma once

#include "signal/core/constants.h"
#include "signal/node/node.h"

namespace libsignal
{

class ADSR : public Node
{
public:
    ADSR(NodeRef attack = 0.1, NodeRef decay = 0.1, NodeRef sustain = 0.5, NodeRef release = 0.1, NodeRef gate = 0);

    NodeRef attack;
    NodeRef decay;
    NodeRef sustain;
    NodeRef release;
    NodeRef gate;

    virtual void process(sample **out, int num_frames);

private:
    float phase;
    bool released;

};

REGISTER(ADSR, "env-adsr")

}