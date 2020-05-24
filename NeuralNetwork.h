#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iomanip>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <assert.h>

/**
 * A basic NeuralNetwork with no backward propogation. The sigma function
 * operates between 0.0 and 0.1.
 */
class NeuralNetwork {
public:
    // I don't usually like typedefs, but here we can collapse a complex 3D array into manageable types without creating a bunch of structs
    using InputWeight = double;
    using Node = std::vector<InputWeight>;
    using Layer = std::vector<Node>;

    enum class InitialWeights : bool {
        Random,
        PassThrough,
    };

    /**
     * Creates a rectangular network of the specified width and height, with
     * random edge weights between 0.0 and 1.0.
     */
    NeuralNetwork(unsigned layerCount, unsigned width, InitialWeights initialWeights);

    unsigned GetInputCount() const { return layers_.front().size(); }
    unsigned GetOutputCount() const { return layers_.back().size(); }

    /**
     * Inputs should be between 0.0 and 1.0 inclusive. Returns the final node
     * values.
     *
     * OPTIMISATION Perhaps nodes storing their value instead of populating
     * vectors could improve performance by reducting copying
     */
    void ForwardPropogate(std::vector<double>& inputs);

    NeuralNetwork Mutated();

private:
    std::vector<Layer> layers_;

    NeuralNetwork(std::vector<Layer>&& layers);

    std::vector<Layer> CreateRandomLayers(unsigned layerCount, unsigned width);
    std::vector<Layer> CreatePassThroughLayers(unsigned layerCount, unsigned width);
};

#endif // NEURALNETWORK_H
