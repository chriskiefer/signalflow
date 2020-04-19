#pragma once

#include "signal/constants.h"
#include "signal/node.h"
#include "../registry.h"
#include "../util.h"

#include <list>

namespace libsignal
{

    class Mixer : public UnaryOpNode
    {

    public:

        Mixer(NodeRef input = 0, int channels = 1);

        virtual void process(sample **out, int num_frames);
        virtual void update_channels();

        std::list <NodeRef> inputs;
        int channels;
        float amp_compensation;

    };

    REGISTER(Mixer, "mixer");

}
