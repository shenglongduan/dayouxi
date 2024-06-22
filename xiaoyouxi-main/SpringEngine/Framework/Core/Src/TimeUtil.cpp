#include "Precompiled.h"
#include "TimeUtil.h"

namespace SpringEngine::Core::TimeUtil
{
    float Frame = 0.0f;
    float GlobalTime = 0.0f;
}
float SpringEngine::Core::TimeUtil::GetTime()
{
    static const auto startTime = std::chrono::high_resolution_clock::now();
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    return milliseconds / 1000.0f;
}

float SpringEngine::Core::TimeUtil::GetDeltaTime()
{
    static auto lastCallTime = std::chrono::high_resolution_clock::now();
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTime).count();
    lastCallTime = currentTime;
    return milliseconds / 1000.0f;
}


void SpringEngine::Core::TimeUtil::UpdateFrame(float deltaTime)
{
    Frame += deltaTime * 30.0f;
}

int SpringEngine::Core::TimeUtil::GetFrame()
{
    return (int)Frame;
}

void SpringEngine::Core::TimeUtil::UpdateGlobalTime(float deltaTime)
{
    GlobalTime += deltaTime;
}
float SpringEngine::Core::TimeUtil::GetGlobalTime()
{
    return GlobalTime;
}
