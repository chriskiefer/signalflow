#pragma once

#include "signal/buffer/buffer.h"
#include "signal/core/constants.h"
#include "signal/core/platform.h"
#include "signal/core/property.h"
#include "signal/core/ringbuffer.h"
#include "signal/node/registry.h"

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace libsignal
{

/*------------------------------------------------------------------------
 * Forward-declare our operator classes to avoid interdependencies.
 *-----------------------------------------------------------------------*/
class Node;
class AudioGraph;
class NodeMonitor;

/*------------------------------------------------------------------------
 * Allows us to use a float (or direct node ptr) in place of a NodeRef
 * by specifying conversion constructors.
 *-----------------------------------------------------------------------*/
template <class T>
class NodeRefTemplate : public std::shared_ptr<T>
{
public:
    NodeRefTemplate()
        : std::shared_ptr<T>(nullptr) {};
    NodeRefTemplate(T *ptr)
        : std::shared_ptr<T>(ptr) {};
    NodeRefTemplate(double x);
    NodeRefTemplate(int x);
    NodeRefTemplate(std::initializer_list<NodeRefTemplate> x);
    NodeRefTemplate(std::vector<NodeRefTemplate> x);

    NodeRefTemplate operator*(NodeRefTemplate other);
    NodeRefTemplate operator*(double constant);
    NodeRefTemplate operator+(NodeRefTemplate other);
    NodeRefTemplate operator+(double constant);
    NodeRefTemplate operator-(NodeRefTemplate other);
    NodeRefTemplate operator-(double constant);
    NodeRefTemplate operator/(NodeRefTemplate other);
    NodeRefTemplate operator/(double constant);
    sample operator[](int index);
};

typedef NodeRefTemplate<Node> NodeRef;

/*------------------------------------------------------------------------
 * Mathematical operators where NodeRef is the RHS operand
 * TODO Test and restore
 *-----------------------------------------------------------------------*/
//    NodeRef operator*(double constant, const NodeRef other);
//    NodeRef operator+(double constant, const NodeRef other);
//    NodeRef operator-(double constant, const NodeRef other);
//    NodeRef operator/(double constant, const NodeRef other);

class Node
{

public:
    Node();
    Node(double x);
    virtual ~Node();

    /*------------------------------------------------------------------------
     * The main generation/processing method.
     *-----------------------------------------------------------------------*/
    virtual void process(sample **out, int num_frames);

    /*------------------------------------------------------------------------
     * Wrapper around process(), called by AudioGraph.pull_input,
     * which handles caching of earlier frames etc.
     *-----------------------------------------------------------------------*/
    virtual void _process(sample **out, int num_frames);

    /*------------------------------------------------------------------------
     * Called after add_input/route to update our routing ins/outs.
     *-----------------------------------------------------------------------*/
    virtual void update_channels();

    /*------------------------------------------------------------------------
     * Register parameters.
     *-----------------------------------------------------------------------*/
    virtual void set_input(std::string name, const NodeRef &input);

    /*------------------------------------------------------------------------
     * Register an output.
     * Note that this must be mirrored with a call to `set_input` on the
     * output node.
     *-----------------------------------------------------------------------*/
    virtual void add_output(Node *target, std::string name);
    virtual void remove_output(Node *target, std::string name);

    /*------------------------------------------------------------------------
     * Disconnect inputs and outputs.
     *-----------------------------------------------------------------------*/
    virtual void disconnect_inputs();
    virtual void disconnect_outputs();

    /*------------------------------------------------------------------------
     * Register properties.
     *-----------------------------------------------------------------------*/
    virtual void add_property(std::string name, PropertyRef &property);
    virtual void set_property(std::string name, const PropertyRef &value);
    virtual PropertyRef get_property(std::string name);

    /*------------------------------------------------------------------------
     * Register buffer params.
     *-----------------------------------------------------------------------*/
    virtual void add_buffer(std::string name, BufferRef &buffer);
    virtual void set_buffer(std::string name, BufferRef buffer);

    /*------------------------------------------------------------------------
     * Generic trigger method. 
     *-----------------------------------------------------------------------*/
    virtual void trigger(std::string name = SIGNAL_DEFAULT_TRIGGER, float value = 1);

    /*------------------------------------------------------------------------
     * Sets our output buffer to zero.
     *-----------------------------------------------------------------------*/
    virtual void zero_output();

    /*------------------------------------------------------------------------
     * Outputs the node's value at a user-specified frequency.
     *-----------------------------------------------------------------------*/
    virtual void poll(float frequency = 1.0, std::string label = "");
    NodeMonitor *monitor;

    /*------------------------------------------------------------------------
     * Returns a new Node that scales the output of this node from
     * `from` to `to`.
     *-----------------------------------------------------------------------*/
    virtual NodeRef scale(float from, float to, signal_scale_t scale = SIGNAL_SCALE_LIN_LIN);

    /*------------------------------------------------------------------------
     * Human-readable name identifier [a-z0-9-]
     *----------------------------------------------------------------------*/
    std::string name;

    /*------------------------------------------------------------------------
     * Hash table of parameters: (name, pointer to NodeRef)
     * Must be a pointer rather than the NodeRef itself as these
     * params are actually pointers to struct fields (this->frequency).
     *-----------------------------------------------------------------------*/
    std::unordered_map<std::string, NodeRef *> params;

    /*------------------------------------------------------------------------
     * Set of outputs.
     * Each output is a std::pair containing 
     *  - a reference to the Node connected outwards to
     *  - a string containing the name of the parameter that this node
     *    modulates.
     * Note that a node may modulate two different parameters of the same
     * node.
     *-----------------------------------------------------------------------*/
    std::set<std::pair<Node *, std::string>> outputs;

    /*------------------------------------------------------------------------
     * Hash table of properties: (name, PropertyRef *)
     * A property is a static, non-streaming value assigned to this node.
     * Properties may be ints, floats, strings or arrays.
     *
     * Similar to `inputs`, each property actually points to a local 
     * PropertyRef field which must be separatedly allocated on the object.
     *-----------------------------------------------------------------------*/
    std::unordered_map<std::string, PropertyRef *> properties;

    /*------------------------------------------------------------------------
     * Buffers are distinct from parameters, pointing to a fixed
     * area of sample storage that must be non-null.
     *-----------------------------------------------------------------------*/
    std::unordered_map<std::string, BufferRef *> buffers;

    /*------------------------------------------------------------------------
     * Pointer to the Graph that this node is a part of.
     * Set automatically in constructor.
     *
     * TODO: Should be an AudioGraphRef
     *-----------------------------------------------------------------------*/
    AudioGraph *graph;

    /*------------------------------------------------------------------------
     * Number of actual in/out channels. This should always reflect
     * the number of audio channels allocated in our `out` buffer,
     *-----------------------------------------------------------------------*/
    int num_input_channels;
    int num_output_channels;

    /*------------------------------------------------------------------------
     * Number of preferred in/out channels. This is used to determine
     * how audio should be up-mixed or down-mixed when passing signals
     * between nodes.
     *
     * N-to-N nodes should use N_CHANNELS / N_CHANNELS
     * 1-to-N nodes should use 1 / N_CHANNELS
     *-----------------------------------------------------------------------*/
    int min_input_channels,
        max_input_channels,
        min_output_channels,
        max_output_channels;
    bool no_input_automix;

    /*------------------------------------------------------------------------
     * Buffer containing this node's output.
     * TODO: Point this partway through a bigger frame buffer so that
     * its history can be read for delay lines etc.
     *-----------------------------------------------------------------------*/
    sample **out;

    /*------------------------------------------------------------------------
     * Stores the number of frames in the previous processing block. Used
     * to populate frame history in out[-1].
     *-----------------------------------------------------------------------*/
    int last_num_frames;

protected:
    /*------------------------------------------------------------------------
     * Creates a new named input.
     * Should only ever be used in the class constructor.
     *-----------------------------------------------------------------------*/
    virtual void add_input(std::string name, NodeRef &input);

    /*------------------------------------------------------------------------
     * Removing an input is only done by special classes
     * (Multiplex, AudioOut)
     *-----------------------------------------------------------------------*/
    virtual void remove_input(std::string name);
};

class GeneratorNode : public Node
{
public:
    GeneratorNode()
        : Node()
    {
        this->num_input_channels = 0;
        this->num_output_channels = 1;
    }
};

class UnaryOpNode : public Node
{
public:
    UnaryOpNode(NodeRef input = 0);

    NodeRef input;
};

class BinaryOpNode : public Node
{
public:
    BinaryOpNode(NodeRef a = 0, NodeRef b = 0);

    NodeRef input0;
    NodeRef input1;
};
}