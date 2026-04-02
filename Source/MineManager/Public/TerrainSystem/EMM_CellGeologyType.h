#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMM_CellGeologyType : uint8
{
    Empty,
    CoverSoil,
    Overburden,
    Rock,
    Coal,
};
