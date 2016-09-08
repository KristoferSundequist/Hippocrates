#pragma once
#include <vector>
#include <memory>
#include "matrix.h"

namespace Convolutional {

class MultiDimensionalMatrix {
public:
	using SubDimensionType = std::vector<Matrix>;

	MultiDimensionalMatrix(SubDimensionType subDimensions);

	auto begin() { return subDimensions.begin(); }
	const auto begin() const { return subDimensions.begin(); }
	auto end() { return subDimensions.end(); }
	const auto end() const { return subDimensions.end(); }
	auto GetDimensionCount() const { return subDimensions.size(); }

private:
	SubDimensionType subDimensions;
};

}
