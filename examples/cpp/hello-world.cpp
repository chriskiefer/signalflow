/*------------------------------------------------------------------------
 * Hello, world.
 *
 * The canonical 440hz sine wave example.
 *-----------------------------------------------------------------------*/

#include <signalflow/signalflow.h>

/*------------------------------------------------------------------------
 * All objects are in the signalflow:: namespace.
 * Import this namespace for code brevity.
 *-----------------------------------------------------------------------*/
using namespace signalflow;

int main()
{
    /*------------------------------------------------------------------------
     * Instantiate a single AudioGraph object for global audio processing.
     *-----------------------------------------------------------------------*/
    AudioGraphRef graph = new AudioGraph();

    /*------------------------------------------------------------------------
     * Create a sine wave oscillator at 440Hz.
     *-----------------------------------------------------------------------*/
    NodeRef sine = new SineOscillator(440);

    /*------------------------------------------------------------------------
     * Begin playing the oscillator.
     *-----------------------------------------------------------------------*/
    graph->play(sine);

    /*------------------------------------------------------------------------
     * Run indefinitely.
     *-----------------------------------------------------------------------*/
    graph->wait();
}
