#include "square.h"

#include <math.h>
#include <stdio.h>

namespace signum::gen
{

void Square::next(sample **out, int num_frames)
{
	for (int frame = 0; frame < num_frames; frame++)
	{
		float frequency = this->frequency->out[0][frame];
		float width = this->width->out[0][frame];
		float rv = (this->phase < width) ? 1 : -1;

		for (int channel = 0; channel < this->channels_out; channel++)
		{
			out[channel][frame] = rv;
		}

		this->phase += 1.0 / (44100.0 / frequency);
		if (this->phase >= 1.0)
			this->phase -= 1.0;
	}
}

}
