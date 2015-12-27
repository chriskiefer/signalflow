#pragma once 

#include "../unit.h"

namespace signum
{
	class Line : public Unit
	{
	public:
		Line(UnitRef time = 1.0, UnitRef from = 0.0, UnitRef to = 1.0) : 
			time(time), from(from), to(to)
		{
			this->name = "line";

			this->add_param("time", this->time);
			this->add_param("from", this->from);
			this->add_param("to", this->to);

			this->value = 0.0;
			this->value_change_per_step = 0.0;
			this->step = 0;
			this->step_target = 0;
		}

		virtual void next(sample **out, int num_frames)
		{
			for (int frame = 0; frame < num_frames; frame++)
			{
				if (!step_target)
				{
					float from = this->from->out[0][frame];
					float to = this->to->out[0][frame];
					float time = this->time->out[0][frame];

					this->step_target = this->graph->sample_rate * time;
					this->value = from;
					this->value_change_per_step = (to - from) / this->step_target;
				}

				if (this->step < this->step_target)
				{
					this->value += this->value_change_per_step;
					this->step++;
				}

				for (int channel = 0; channel < channels_out; channel++)
				{
					this->out[channel][frame] = this->value;
				}
			}
		}

		UnitRef time;
		UnitRef from;
		UnitRef to;

		float value;
		float value_change_per_step;
		int step;
		int step_target;

	};

	REGISTER(Line, "line");
}
