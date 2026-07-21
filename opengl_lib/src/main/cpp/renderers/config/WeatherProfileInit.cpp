#include "ProfileBuilderRegistry.h"
#include "builders/ClearDayBuilder.h"
#include "builders/ClearNightBuilder.h"
#include "builders/PartlyCloudyBuilder.h"
#include "builders/CloudyBuilder.h"
#include "builders/HazeBuilder.h"
#include "builders/RainBuilder.h"
#include "builders/FogBuilder.h"
#include "builders/SnowBuilder.h"
#include "builders/DustBuilder.h"
#include "builders/SandBuilder.h"
#include "builders/WindBuilder.h"

void RegisterWeatherProfileBuilders() {
    ProfileBuilderRegistry* registry = ProfileBuilderRegistry::GetInstance();

    registry->RegisterBuilder("CLEAR_DAY", new ClearDayBuilder());
    registry->RegisterBuilder("CLEAR_NIGHT", new ClearNightBuilder());
    registry->RegisterBuilder("PARTLY_CLOUDY_DAY", new PartlyCloudyBuilder("PARTLY_CLOUDY_DAY"));
    registry->RegisterBuilder("PARTLY_CLOUDY_NIGHT", new PartlyCloudyBuilder("PARTLY_CLOUDY_NIGHT"));
    registry->RegisterBuilder("CLOUDY", new CloudyBuilder());
    registry->RegisterBuilder("LIGHT_HAZE", new HazeBuilder("LIGHT_HAZE", 0));
    registry->RegisterBuilder("MODERATE_HAZE", new HazeBuilder("MODERATE_HAZE", 1));
    registry->RegisterBuilder("HEAVY_HAZE", new HazeBuilder("HEAVY_HAZE", 2));
    registry->RegisterBuilder("LIGHT_RAIN", new RainBuilder("LIGHT_RAIN", 0));
    registry->RegisterBuilder("MODERATE_RAIN", new RainBuilder("MODERATE_RAIN", 1));
    registry->RegisterBuilder("HEAVY_RAIN", new RainBuilder("HEAVY_RAIN", 2));
    registry->RegisterBuilder("STORM_RAIN", new RainBuilder("STORM_RAIN", 3));
    registry->RegisterBuilder("FOG", new FogBuilder());
    registry->RegisterBuilder("LIGHT_SNOW", new SnowBuilder("LIGHT_SNOW", 0));
    registry->RegisterBuilder("MODERATE_SNOW", new SnowBuilder("MODERATE_SNOW", 1));
    registry->RegisterBuilder("HEAVY_SNOW", new SnowBuilder("HEAVY_SNOW", 2));
    registry->RegisterBuilder("STORM_SNOW", new SnowBuilder("STORM_SNOW", 3));
    registry->RegisterBuilder("DUST", new DustBuilder());
    registry->RegisterBuilder("SAND", new SandBuilder());
    registry->RegisterBuilder("WIND", new WindBuilder());
}
