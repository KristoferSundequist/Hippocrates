#include "neuron.h"
#include <cmath>
#include <stdexcept>

Neuron::Neuron(const Connections& connections) :
connections(connections) {
}

void Neuron::AddConnection(Neuron* incoming, float weight)
{
    if (incoming == this || incoming == nullptr) {
        throw std::invalid_argument("Invalid incomming connection");
    }
	connections[incoming] = weight;
}

float Neuron::RequestDataAndGetActionPotential() {
    if (isSensor){
        return lastActionPotential;
    }

    float incomingPotentials = 0.0f;
    for (auto& in : connections){
        incomingPotentials += in.first->RequestDataAndGetActionPotential() * in.second;
    }
    lastActionPotential = sigmoid(incomingPotentials);
    return lastActionPotential;
}

float Neuron::sigmoid(float d) {
    return (float)tanh(d);
}

void Neuron::SetInput(float input) {
    isSensor = true;
    lastActionPotential = input;
}

void Neuron::SetDistanceFromOutput(std::size_t distanceFromOutput) {
    if (distanceFromOutput > this->distanceFromOutput){
        this->distanceFromOutput = distanceFromOutput;
    }
}

