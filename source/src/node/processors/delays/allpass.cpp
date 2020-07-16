#include "signalflow/node/oscillators/constant.h"
#include "signalflow/node/processors/delays/allpass.h"

#include <stdlib.h>

namespace signalflow
{

AllpassDelay::AllpassDelay(NodeRef input, NodeRef delaytime, NodeRef feedback, float maxdelaytime)
    : UnaryOpNode(input), delaytime(delaytime), feedback(feedback), maxdelaytime(maxdelaytime)
{
    this->name = "allpass-delay";
    this->add_input("delay_time", this->delaytime);
    this->add_input("feedback", this->feedback);

    SIGNAL_CHECK_GRAPH();
    for (int i = 0; i < SIGNAL_MAX_CHANNELS; i++)
    {
        buffers.push_back(new SampleRingBuffer(maxdelaytime * this->graph->get_sample_rate()));
    }
}

AllpassDelay::~AllpassDelay()
{
    for (auto buffer : buffers)
    {
        delete buffer;
    }
}

void AllpassDelay::process(sample **out, int num_frames)
{
    SIGNAL_CHECK_GRAPH();

    for (int channel = 0; channel < this->num_input_channels; channel++)
    {
        for (int frame = 0; frame < num_frames; frame++)
        {
            sample delay = this->delaytime->out[channel][frame];
            sample feedback = this->feedback->out[channel][frame];
            float offset = delay * this->graph->get_sample_rate();

            sample v = input->out[channel][frame] - feedback * buffers[channel]->get(-offset);
            sample rv = feedback * v + buffers[channel]->get(-offset);
            out[channel][frame] = rv;
            buffers[channel]->append(v);
        }
    }
}

}