#include "genome.h"
#include <vector>
#include <algorithm>

Genome::Genome(const TrainingParameters& parameters) :
	parameters(parameters),
	genes(parameters.numberOfInputs * parameters.numberOfOutputs)
{
	auto *currentGene = &genes.front();
	for (auto in = 0U; in < parameters.numberOfInputs; ++in) {
		for (auto out = 0U; out < parameters.numberOfOutputs; ++out) {
			currentGene->from = in;
			currentGene->to = out + parameters.numberOfInputs;
			++currentGene;
		}
	}
}

std::size_t Genome::ExtrapolateNeuronCount() const {
	auto CompareToNeuron = [](const Gene& lhs, const Gene& rhs) {
		return lhs.to < rhs.to;
	};
	auto maxNeuronGene = std::max_element(genes.begin(), genes.end(), CompareToNeuron);
	// TODO jnf Maybe add lookup table
	return maxNeuronGene->to + 1U;
}

std::size_t Genome::GetGeneCount() const {
	return genes.size();
}

Genome& Genome::operator=(const Genome& other)
{
	this->genes = other.genes; 
	const_cast<TrainingParameters&>(this->parameters) = other.parameters;
	return *this;
}

void Genome::MutateGenes() {
	if (ShouldAddConnection()) {
		AddRandomConnection();
	}
	else
	if (ShouldAddNeuron()) {
		AddRandomNeuron();
	}
	else {
		ShuffleWeights();
	}
}

bool Genome::DidChanceOccure(float chance)
{
	auto num = rand() % 100;
	return num < int(100.0f * chance);
}

void Genome::AddRandomNeuron()
{
	Gene* randGene = nullptr;
	do {
		int num = rand() % genes.size();
		randGene = &genes[num];
	} while (!randGene->isEnabled);

	auto indexOfNewNeuron = ExtrapolateNeuronCount();

	Gene g1;
    g1.from = randGene->from;
	g1.to = indexOfNewNeuron;
    g1.weight = randGene->weight;

	Gene g2;
	g2.from = indexOfNewNeuron;
    g2.to = randGene->to;
    g2.weight = randGene->weight;

	randGene->isEnabled = false;
	genes.push_back(std::move(g1));
	genes.push_back(std::move(g2));
}

void Genome::AddRandomConnection()
{
	auto GetRandomNumberBetween = [](size_t min, size_t max) {
		return rand() % (max - min) + min;
	};

	Gene newConnection;
	auto highestNeuronIndex = ExtrapolateNeuronCount() - 1U;

	newConnection.from = GetRandomNumberBetween(0U, highestNeuronIndex - 1U);
	newConnection.to = GetRandomNumberBetween(newConnection.from + 1, highestNeuronIndex);

	//genes.push_back(newConnection);
}

void Genome::ShuffleWeights()
{
	for (size_t i = 0; i < genes.size(); i++) {
		if (ShouldMutateWeight()) {
			MutateWeightOfGeneAt(i);
		}
	}
}

void Genome::MutateWeightOfGeneAt(size_t index)
{
	constexpr float chanceOfTotalWeightReset = 0.1f;
	if (DidChanceOccure(chanceOfTotalWeightReset)) {
		genes[index].SetRandomWeight();
	} else {
		PerturbWeightAt(index);
	}
}

void Genome::PerturbWeightAt(size_t index)
{
	constexpr float perturbanceBoundaries = 0.2f;
	auto perturbance = (float)(rand() % 10'000) / 10'000.0f * perturbanceBoundaries;
	if (rand() % 2) {
		perturbance = -perturbance;
	}
	genes[index].weight *= perturbance;
}

double Genome::GetGeneticalDistanceFrom(const Genome& other) const
{
	double totalWeightDifference = 0.0;
	size_t numberOfOverlapingGenes = 0;

	size_t sizeOfSmallerGenome = std::min(this->GetGeneCount(), other.GetGeneCount());
	auto IsHistoricalMarkingSameAt = [&](size_t i) {
		return this->GetGeneAt(i).historicalMarking == other[i].historicalMarking;
	};

	for (size_t i = 0; i < sizeOfSmallerGenome && IsHistoricalMarkingSameAt(i); ++i) {
		totalWeightDifference += (double)std::abs(this->GetGeneAt(i).weight - other[i].weight);
		++numberOfOverlapingGenes;
	}

	auto numberOfDisjointGenes = this->GetGeneCount() + other.GetGeneCount() - (size_t)2 * numberOfOverlapingGenes;
	auto sizeOfBiggerGenome = std::max(this->GetGeneCount(), other.GetGeneCount());
	auto disjointGenesInfluence = (double)numberOfDisjointGenes / (double)sizeOfBiggerGenome;

	auto averageWeightDifference = totalWeightDifference / (double)numberOfOverlapingGenes;

	disjointGenesInfluence *= (double)parameters.advanced.speciation.importanceOfDisjointGenes;
	averageWeightDifference *= (double)parameters.advanced.speciation.importanceOfAverageWeightDifference;

	return disjointGenesInfluence + averageWeightDifference;
}