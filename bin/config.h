#pragma once

#include <cstdint>
#include <string>

//wave
static const std::uint16_t BITS_PER_SAMPLE = 24;
static const std::uint8_t CHANNELS = 2;
static const std::uint32_t RATE = 48000;

//common
static const std::size_t DATA_SIZE = 2880;
static const std::size_t DELAY_MS = 10; // DELAY_MS = DATA_SIZE * 1000 * 8 / (RATE * CHANNELS * BITS_PER_SAMPLE)
static const std::size_t MAX_SIZE_QUEUE = 100;
