#pragma once


static constexpr unsigned int sendReceiveSamplingTime = 1; // in ms
static constexpr float sendReceiveSamplingTime_seconds = sendReceiveSamplingTime *0.001f; // in seconds

static constexpr unsigned int crazyflieUpdateSamplingTime = 5; // in ms
static constexpr float crazyflieUpdateSamplingTime_seconds = crazyflieUpdateSamplingTime *0.001f; // in seconds

static constexpr unsigned int cameraUpdateSamplingTime = 30; // in ms
static constexpr float cameraUpdateSamplingTime_seconds = cameraUpdateSamplingTime *0.001f; // in seconds

static constexpr unsigned int guiUpdateSamplingTime = 100; // in ms
static constexpr float guiUpdateSamplingTime_seconds = guiUpdateSamplingTime *0.001f; // in seconds

