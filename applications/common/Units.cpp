#include <Units.h>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

namespace Units
{

double GetLengthFactor(UnitSystem& fromUnitSystem, UnitSystem& toUnitSystem)
{
    if(LengthUnit::Unknown == fromUnitSystem.lengthUnit)
    {
        std::cerr << "Unknown length unit!!!" << std::endl;
        return 1.0;
    }

    if(LengthUnit::Unknown == toUnitSystem.lengthUnit)
    {
        std::cerr << "Unknown length unit!!!" << std::endl;
        return 1.0;
    }

    if(fromUnitSystem.lengthUnit == toUnitSystem.lengthUnit)
        return 1.0;


    //Converting from fromUnitSystem to Millimeter
    double toMillimeter = 1.0;

    if(LengthUnit::Meter == fromUnitSystem.lengthUnit)
        toMillimeter = 1000.0;

    else if(LengthUnit::Centimeter == fromUnitSystem.lengthUnit)
        toMillimeter = 10.0;

    else if(LengthUnit::Millimeter == fromUnitSystem.lengthUnit)
        toMillimeter = 1.0;

    else if(LengthUnit::Inch == fromUnitSystem.lengthUnit)
        toMillimeter = 25.4;

    else if(LengthUnit::Foot == fromUnitSystem.lengthUnit)
        toMillimeter = 304.8;

    //Converting from Millimeter to toUnitSystem
    double fromMillimeter = 1.0;

    if(LengthUnit::Meter == toUnitSystem.lengthUnit)
        fromMillimeter = 0.001;

    else if(LengthUnit::Centimeter == toUnitSystem.lengthUnit)
        fromMillimeter = 0.1;

    else if(LengthUnit::Millimeter == toUnitSystem.lengthUnit)
        fromMillimeter = 1.0;

    else if(LengthUnit::Inch == toUnitSystem.lengthUnit)
        fromMillimeter = 1.0/25.4;

    else if(LengthUnit::Foot == toUnitSystem.lengthUnit)
        fromMillimeter = 1.0/304.8;

    return toMillimeter * fromMillimeter;
}

double GetTimeFactor(UnitSystem& fromUnitSystem, UnitSystem& toUnitsystem)
{
    if(TimeUnit::Unknown == fromUnitSystem.timeUnit)
    {
        std::cerr << "Unknown time unit!!!" << std::endl;
        return 1.0;
    }

    if(TimeUnit::Unknown == toUnitsystem.timeUnit)
    {
        std::cerr << "Unknown time unit!!!" << std::endl;
        return 1.0;
    }

    if(fromUnitSystem.timeUnit == toUnitsystem.timeUnit)
        return 1.0;

    //converting to second
    double toSecond = 1.0;
    if(TimeUnit::Hour == fromUnitSystem.timeUnit)
        toSecond = 3600.0;

    else if(TimeUnit::Minute == fromUnitSystem.timeUnit)
        toSecond = 60.0;

    //converting from second
    double fromSecond = 1.0;
    if(TimeUnit::Hour == toUnitsystem.timeUnit)
        fromSecond = 1.0/3600.0;

    else if(TimeUnit::Minute == toUnitsystem.timeUnit)
        fromSecond = 1.0/60.0;

    return toSecond * fromSecond;
}

double GetAccelerationFactor(UnitSystem& fromUnitSystem, UnitSystem& toUnitSystem)
{
    double timeFactor = GetTimeFactor(fromUnitSystem, toUnitSystem);
    double lengthFactor = GetLengthFactor(fromUnitSystem, toUnitSystem);

    return lengthFactor / (timeFactor*timeFactor);
}

LengthUnit ParseLengthUnit(const char* lengthUnit)
{
    std::string lengthUnitString(lengthUnit);
    std::transform(lengthUnitString.begin(), lengthUnitString.end(), lengthUnitString.begin(), ::tolower);

    static std::map<std::string, LengthUnit> lengthUnitMap
    {
        {"m", LengthUnit::Meter},
        {"meter", LengthUnit::Meter},
        {"meters", LengthUnit::Meter},
        {"cm", LengthUnit::Centimeter},

        {"centimeter", LengthUnit::Centimeter},
        {"centimeters", LengthUnit::Centimeter},

        {"mm", LengthUnit::Millimeter},
        {"millimeter", LengthUnit::Millimeter},
        {"millimeters", LengthUnit::Millimeter},

        {"in", LengthUnit::Inch},
        {"inch", LengthUnit::Inch},
        {"inches", LengthUnit::Inch},

        {"ft", LengthUnit::Foot},
        {"foot", LengthUnit::Foot},
        {"feet", LengthUnit::Foot}
    };

    if(lengthUnitMap.end() != lengthUnitMap.find(lengthUnitString))
        return lengthUnitMap[lengthUnitString];

    std::cerr << "Failed to parse length unit: " << lengthUnitString  << "!!!" << std::endl;
    return LengthUnit::Unknown;
}

TimeUnit ParseTimeUnit(const char* timeUnit)
{
    std::string timeUnitString(timeUnit);
    std::transform(timeUnitString.begin(), timeUnitString.end(), timeUnitString.begin(), ::tolower);

    static std::map<std::string, TimeUnit> timeUnitMap
    {
        {"s", TimeUnit::Second},
        {"sec", TimeUnit::Second},
        {"second", TimeUnit::Second},
        {"seconds", TimeUnit::Second},

        {"min", TimeUnit::Minute},
        {"minute", TimeUnit::Minute},
        {"minutes", TimeUnit::Minute},

        {"hr", TimeUnit::Hour},
        {"hour", TimeUnit::Hour},
        {"hours", TimeUnit::Hour},

    };

    if(timeUnitMap.end() != timeUnitMap.find(timeUnitString))
        return timeUnitMap[timeUnitString];

    std::cerr << "Failed to parse time unit: " << timeUnitString  << "!!!" << std::endl;

    return TimeUnit::Unknown;
}

double GetGravity(UnitSystem &unitSystem)
{
    double gravity = 9.80665; //in SI Units (m/sec^2)

    UnitSystem SIUnits;
    SIUnits.lengthUnit = LengthUnit::Meter;
    SIUnits.timeUnit = TimeUnit::Second;

    double conversionFactor = GetAccelerationFactor(SIUnits, unitSystem);

    return gravity * conversionFactor;
}


}
