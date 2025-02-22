#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t pixelDataOffset;
};

struct DIBHeader {
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)

// thanks ChatGPT
void parseBMP(const uint8_t* bmpData, size_t dataSize, uint8_t (&output)[48 * 48 * 4]) {
    if (!bmpData || dataSize < 54)
        throw std::runtime_error("Invalid BMP data");

    BMPHeader bmpHeader;
    std::memcpy(&bmpHeader, bmpData, sizeof(BMPHeader));

    if (bmpHeader.fileType != 0x4D42)
        throw std::runtime_error("Not a BMP file");

    DIBHeader dibHeader;
    std::memcpy(&dibHeader, bmpData + sizeof(BMPHeader), sizeof(DIBHeader));

    if (dibHeader.width != 48 || std::abs(dibHeader.height) != 48)
        throw std::runtime_error("Unsupported BMP dimensions (must be 64x64)");

    const uint8_t* pixelData = bmpData + bmpHeader.pixelDataOffset;
    int bytesPerPixel = dibHeader.bitCount / 8;

    if (dibHeader.bitCount != 24 && dibHeader.bitCount != 32)
        throw std::runtime_error("Unsupported BMP bit count (only 24 or 32-bit allowed)");

    int rowStride = ((dibHeader.width * dibHeader.bitCount + 31) / 32) * 4;
    bool isBottomUp = dibHeader.height > 0;

    for (int y = 0; y < 48; ++y) {
        int srcY = isBottomUp ? (47 - y) : y;
        const uint8_t* srcRow = pixelData + srcY * rowStride;
        uint8_t* dstRow = &output[y * 48 * 4];

        for (int x = 0; x < 48; ++x) {
            uint8_t b = srcRow[x * bytesPerPixel + 0];
            uint8_t g = srcRow[x * bytesPerPixel + 1];
            uint8_t r = srcRow[x * bytesPerPixel + 2];
            uint8_t a = (dibHeader.bitCount == 32) ? srcRow[x * bytesPerPixel + 3] : 255; // Default alpha to 255 if 24-bit

            dstRow[x * 4 + 0] = r;
            dstRow[x * 4 + 1] = g;
            dstRow[x * 4 + 2] = b;
            dstRow[x * 4 + 3] = a;
        }
    }
}