#ifndef COLOR_H
#define COLOR_H

#include "rtweekend.h"

// openEXR
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <iostream>

#include "interval.h"
#include "vec3.h"

using color = vec3;

void writeColor(std::ostream& out, const color& pixelColor)
{
    auto red = pixelColor.x();
    auto green = pixelColor.y();
    auto blue = pixelColor.z();

    static const interval intensity(0.000, 0.999);
    int redByte = int(256 * intensity.clamp(red));
    int greenByte = int(256 * intensity.clamp(green));
    int blueByte = int(256 * intensity.clamp(blue));

    out << redByte << ' ' << greenByte << ' ' << blueByte << '\n';
}

void writeToOpenEXR(Imf::Array2D<Imf::Rgba> &inputFrame, int width, int height, char* filename)
{
    try
    {   
        
        Imf::RgbaOutputFile file (filename, width, height, Imf::WRITE_RGBA);
        file.setFrameBuffer(&inputFrame[0][0], 1, width);
        file.writePixels(height);
    } catch (const std::exception &e) {
        std::cerr << "Fails to Write Image: " << e.what() << std::endl;
    }
}

#endif