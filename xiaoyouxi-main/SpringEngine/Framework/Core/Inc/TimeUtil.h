#pragma once


namespace SpringEngine::Core::TimeUtil
{
    float GetTime();
    float GetDeltaTime();

    void UpdateFrame(float deltaTime);
    int GetFrame();

    void UpdateGlobalTime(float deltaTime);
    float GetGlobalTime();
}