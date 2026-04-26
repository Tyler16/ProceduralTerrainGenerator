#pragma once

#include <FastSIMD/Utility/ArchDetect.h>
#include <FastSIMD/Utility/FeatureSetList.h>

namespace FastSIMD
{
namespace FastSIMD_FastNoise
{
using CompiledFeatureSets = FeatureSetList<0
,FastSIMD::FeatureSet::NEON
,FastSIMD::FeatureSet::AARCH64
>;
}
}
