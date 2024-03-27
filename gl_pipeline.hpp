#pragma once

#include "gl_pipeline.h"

float CorePipeline::screenW = 2560;
float CorePipeline::screenH = 1400;

bool CorePipeline::isDebugMode = false;
bool CorePipeline::isEditorMode = true;

bool CorePipeline::isGameExit = false;
bool CorePipeline::targetFrameLock = true;
float CorePipeline::targetFrame = 165;
float CorePipeline::targetFrameBetween = ((float)1000) / CorePipeline::targetFrame;
float CorePipeline::totalTime = 0;
float CorePipeline::deltaTime = 1 / CorePipeline::targetFrame;
std::chrono::steady_clock::time_point CorePipeline::updatePrevClock;
std::chrono::steady_clock::time_point CorePipeline::updateNowClock;

