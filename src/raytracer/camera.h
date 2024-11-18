#ifndef CAMERA_H
#define CAMERA_H

// openEXR
#include <ImfRgbaFile.h>
#include <ImfArray.h>

// TBB
#include <tbb/parallel_for.h>

#include "hittable.h"
#include "material.h"

class camera
{
    public:
        double aspectRatio = 1.0;
        int imagePlaneWidth = 100;
        int samplesPerPixel = 10;
        int maxDepth = 10;

        double viewFov = 90;
        point3 lookFrom = point3(0,0,0);
        point3 lookAt = point3(0,0,-1);
        vec3 vUp = vec3(0,1,0);

        double defocusAngle = 0;
        double focusDist = 10;

        Imf::Array2D<Imf::Rgba> frame;
        Imf::Array2D<Imf::Rgba> debugFrame;

        void parallelRender(const hittable& world)
        {
            initialize();
            
            Imf::Array2D<Imf::Rgba> frame(imagePlaneHeight, imagePlaneWidth);
            Imf::Array2D<Imf::Rgba> debugFrame(imagePlaneHeight, imagePlaneWidth);
            
            int finishedRows = 0;
            tbb::parallel_for(0, imagePlaneHeight, [&](int y){
                finishedRows++;
                std::clog << "Rows Left: " << (imagePlaneHeight - finishedRows) << std::endl;
                std::clog.flush();
                tbb::parallel_for(0, imagePlaneWidth, [&](int x){
                    calPixelColor(x, y, world, frame, debugFrame);
                });
            });

            std::clog << "Writing to frame with width: " << frame.width() << std::endl;
            std::clog << "Writing to frame with height: " << frame.height() << std::endl;
            writeToOpenEXR(frame, imagePlaneWidth, imagePlaneHeight, "output.exr");
            writeToOpenEXR(debugFrame, imagePlaneWidth, imagePlaneHeight, "test.exr");
            std::clog << "\rDone.                 \n";
        };

        void render(const hittable& world)
        {
            initialize();
            //std::cout << "P3\n" << imagePlaneWidth << ' ' << imagePlaneHeight << "\n255\n";
            Imf::Array2D<Imf::Rgba> frame(imagePlaneWidth, imagePlaneHeight);
            Imf::Array2D<Imf::Rgba> debugFrame(imagePlaneWidth, imagePlaneHeight);

            for (int y = 0; y < imagePlaneHeight; y++)
            {
                std::clog << "\rScanlines Left: " << (imagePlaneHeight - y) << ' ' << std::flush;
                for (int x = 0; x < imagePlaneWidth; x++)
                {
                    calPixelColor(x, y, world, frame, debugFrame);
                };
            };

            writeToOpenEXR(frame, imagePlaneWidth, imagePlaneHeight, "output.exr");
            writeToOpenEXR(debugFrame, imagePlaneWidth, imagePlaneHeight, "test.exr");
            std::clog << "\rDone.                 \n";
        }
    private:
        int imagePlaneHeight;
        double pixelSampleScale;
        point3 cameraCenter;
        point3 pixel_00_loc;
        vec3 pixelDeltaU;
        vec3 pixelDeltaV;
        vec3 u, v, w;
        vec3 defocusDiskU;
        vec3 defocusDiskV;

        void initialize()
        {
            imagePlaneHeight = int(imagePlaneWidth / aspectRatio);
            imagePlaneHeight = (imagePlaneHeight < 1) ? 1 : imagePlaneHeight;
            Imf::Array2D<Imf::Rgba> frame(imagePlaneWidth, imagePlaneHeight);
            Imf::Array2D<Imf::Rgba> debugFrame(imagePlaneWidth, imagePlaneHeight);

            std::cout << "Height: " << imagePlaneHeight<< std::endl;
            std::cout << "Width: " << imagePlaneWidth << std::endl;
            std::cout << "aspectRatio: " << aspectRatio << std::endl;

            pixelSampleScale = 1.0 / samplesPerPixel;

            cameraCenter = lookFrom;

            auto theta = degreesToRadians(viewFov);
            auto h = std::tan(theta/2);
            auto viewportHeight = 2 * h * focusDist;
            auto viewportWidth = viewportHeight * (float(imagePlaneWidth)/imagePlaneHeight);

            w = unitVector(lookFrom - lookAt);
            u = unitVector(cross(vUp, w));
            v = cross(w, u);

            auto viewportU = viewportWidth * u;
            auto viewportV = viewportHeight * -v;

            pixelDeltaU = viewportU / imagePlaneWidth;
            pixelDeltaV = viewportV / imagePlaneHeight;

            auto viewportUpperLeft = cameraCenter - (focusDist * w) - viewportU/2 - viewportV/2;
            pixel_00_loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

            auto defocusRadius = focusDist * std::tan(degreesToRadians(defocusAngle / 2));
            defocusDiskU = u * defocusRadius;
            defocusDiskV = v * defocusRadius;
        };

        ray getRay(int x, int y) const
        {
            auto offset = sampleSquare();
            auto pixelSample = pixel_00_loc + ((x + offset.x()) * pixelDeltaU) + ((y + offset.y()) * pixelDeltaV);
            auto rayOrigin = (defocusAngle <= 0) ? cameraCenter : defocusDiskSample();
            auto rayDirection = pixelSample - rayOrigin;

            return ray(rayOrigin, rayDirection);
        }

        vec3 sampleSquare() const
        {
            return vec3(randomDouble() - 0.5, randomDouble() - 0.5, 0);
        }

        point3 defocusDiskSample() const
        {
            auto p = randomInUnitDisk();
            return cameraCenter + (p[0] * defocusDiskU) + (p[1] * defocusDiskV);
        }

        color rayColor(const ray& r, int maxDepth, const hittable& world)
        {
            if (maxDepth <= 0)
            {
                return color(0,0,0);
            }
            hitRecord rec;
            if(world.hit(r, interval(0, infinity), rec))
            {
                ray scattered;
                color attenuation;
                if(rec.mat->scatter(r, rec, attenuation, scattered))
                {
                    return attenuation * rayColor(scattered, maxDepth - 1, world);
                }
                return color(0,0,0);
            }

            vec3 unitDirection = unitVector(r.direction());
            auto a = 0.5*(unitDirection.y() + 1.0);
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
        };

        void calPixelColor(int x, int y, const hittable& world, Imf::Array2D<Imf::Rgba>& frame, Imf::Array2D<Imf::Rgba>& debugFrame)
        {
            color pixelColor (0,0,0);
            for (int sampleID = 0; sampleID < samplesPerPixel; sampleID++)
            {
                ray r = getRay(x, y);
                pixelColor += rayColor(r, maxDepth, world);
            }
            auto finalPixel = pixelSampleScale * pixelColor;
            frame[y][x] = Imf::Rgba(half(finalPixel.x()), half(finalPixel.y()), half(finalPixel.z()), 0.0);
            debugFrame[y][x] = Imf::Rgba(x / (imagePlaneWidth-1.0f), y / (imagePlaneHeight-1.0f), 0.0);
        };
};

#endif