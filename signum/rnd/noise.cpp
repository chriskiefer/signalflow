#include "noise.h"

#include "stdlib.h"
#include "../util.h"

using namespace signum;

namespace signum::rnd
{

Noise::Noise(float frequency, bool interpolate, float min, float max)
{
	this->frequency = frequency;
	this->interpolate = interpolate;
	this->min = min;
	this->max = max;

	this->value = rng_uniform(min, max);

	this->steps_remaining = 0;
}

void Noise::next(int count)
{
	for (int i = 0; i < count; i++)
	{
		if (this->steps_remaining <= 0)
		{
			// pick a new target value
			float target = rng_uniform(min, max);
			printf("new target: %f\n", target);

			if (this->frequency > 0)
			{
				this->steps_remaining = rand() % (int) (44100.0 / this->frequency);
				if (this->steps_remaining == 0)
					this->steps_remaining = 1;
				this->step_change = (target - value) / this->steps_remaining;
			}
			else
			{
				this->steps_remaining = 0;
				this->step_change = target - value;
			}

			if (!this->interpolate)
			{
				this->value = target;
				this->step_change = 0;
			}
		}

		this->value += this->step_change;

		this->output->data[0][i] = this->value;
		this->steps_remaining--;
	}
}

}
