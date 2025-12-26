//
// Created by hstasonis on 12/25/25.
//

#pragma once

/// @brief Base class for elevatable programs
class WildcatElevatable
{
public:
    WildcatElevatable() = default;
    virtual ~WildcatElevatable() = default;

    /// @brief Called when elevation fails
    virtual void onElevationFailure() {}

    /// @brief Called when elevation to the requested level succeeds
    virtual void onElevationComplete() = 0;

    /**
     * Makes sure that the current process is running as UID 0 then calls the proper functions
     * @param pElevatable Elevatable class
     */
    static void elevate(WildcatElevatable *pElevatable);
};