#include "NeuralNetwork.h"

#include "Random.h"

NeuralNetwork::NeuralNetwork(unsigned layerCount, unsigned width, NeuralNetwork::InitialWeights initialWeights)
    : NeuralNetwork(initialWeights == InitialWeights::Random ? CreateRandomLayers(layerCount, width) : CreatePassThroughLayers(layerCount, width))
{
}

void NeuralNetwork::ForwardPropogate(std::vector<double>& toPropogate)
{
    static std::vector<double> previousNodeValues;

    // about to swap with previousNodeValues so we can return outputs at the end
    // also allows to skip propogation when no hidden layers
    for (auto& layer : layers_) {
        std::swap(toPropogate, previousNodeValues);
        // We'll reuse this vector for the output of each layer
        toPropogate.assign(layer.size(), 0.0);

        size_t nodeIndex = 0;
        for (auto& node : layer) {
            double nodeValue = 0.0;
            size_t edgeIndex = 0;
            for (auto& edge : node) {
                nodeValue += edge * previousNodeValues[edgeIndex];
                ++edgeIndex;
            }
            // tanh is our sigma function
            toPropogate[nodeIndex] = std::tanh(nodeValue);
            nodeIndex++;
        }
    }
}

NeuralNetwork NeuralNetwork::Mutated()
{
    size_t layerCount = layers_.size();
    std::vector<Layer> layers;
    for (auto& layer : layers_) {
        layers.push_back(Layer());
        for (auto& node : layer) {
            layers.back().push_back(Node());
            for (auto& edge : node) {
                double newEdge = edge;
                // i.e average 3 mutations per child
                if (Random::Number(0u, layerCount * layer.size() * layer.size()) < 3) {
                    edge += Random::Gaussian(-0.5, 0.5);
                }
                layers.back().back().push_back(newEdge);
            }
        }
    }
    return NeuralNetwork(std::move(layers));
}

NeuralNetwork::NeuralNetwork(std::vector<NeuralNetwork::Layer>&& layers)
    : layers_(std::move(layers))
{
}

std::vector<NeuralNetwork::Layer> NeuralNetwork::CreateRandomLayers(unsigned layerCount, unsigned width)
{
    std::vector<Layer> layers(layerCount, Layer(width, Node(width)));

    // First layer doesn't need input weights, as it will be assigned a value by the ForwardPropogate() func
    for (auto& layer : layers) {
        for (auto& node : layer) {
            double mean = 0.75;
            double stdDev = 0.25;
            node = Random::DualPeakGaussians(width, -mean, stdDev, mean, stdDev);
        }
    }

    return layers;
}

std::vector<NeuralNetwork::Layer> NeuralNetwork::CreatePassThroughLayers(unsigned layerCount, unsigned width)
{
    std::vector<Layer> layers(layerCount, Layer(width, Node(width)));

    // First layer doesn't need input weights, as it will be assigned a value by the ForwardPropogate() func
    for (auto& layer : layers) {
        unsigned nodeColumn = 0;
        for (auto& node : layer) {
            for (unsigned inputColumn = 0; inputColumn < node.size(); inputColumn++) {
                node[inputColumn] = (inputColumn == nodeColumn ? 1.0 : 0.0);
            }
        }
        nodeColumn++;
    }

    return layers;
}
