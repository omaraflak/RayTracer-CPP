#include <fstream>
#include <vector>
#include <stdio.h>
#include <limits>
#include "vect3.h"

typedef struct sphere_t {
    Vect3 center;
    float radius;
    Vect3 ambient;
    Vect3 diffuse;
    Vect3 specular;
    float shininess;
    float reflectivity;

    sphere_t(
        const Vect3& center,
        const float& radius,
        const Vect3& diffuse,
        const float& shininess,
        const float& reflectivity
    ) {
        this->center.set(center);
        this->radius = radius;
        this->ambient.set(0.1 * diffuse);
        this->diffuse.set(diffuse);
        this->specular.set(1, 1, 1);
        this->shininess = shininess;
        this->reflectivity = reflectivity;
    }

    float intersect(const Vect3& origin, const Vect3& direction) {
        float b = direction.dot(2 * (origin - center));
        float c = pow((origin - center).length(), 2) - pow(radius, 2);
        float delta = b * b - 4 * c;

        if (delta > 0) {
            float t1 = (-b + sqrt(delta)) / 2;
            float t2 = (-b - sqrt(delta)) / 2;
            float t = std::min(std::max(0.f, t1), std::max(0.f, t2));
            if (t > 0) {
                return t;
            }
        }

        return -1;
    }

    Vect3 normal(const Vect3& point) {
        return (point - center).normalized();
    }
} sphere_t;

typedef struct light_t {
    Vect3 position;
    Vect3 ambient;
    Vect3 diffuse;
    Vect3 specular;

    light_t(const Vect3& position) {
        this->position.set(position);
        this->ambient.set(1, 1, 1);
        this->diffuse.set(1, 1, 1);
        this->specular.set(1, 1, 1);
    }
} light_t;

Vect3 camera(0, 0, 1);
light_t light(Vect3(5, 5, 5));
std::vector<sphere_t> objects = {
    sphere_t(Vect3(-0.2, 0, -1), 0.7, Vect3(0.7, 0, 0), 100, 0.5),
    sphere_t(Vect3(0.1, -0.3, 0), 0.1, Vect3(0.7, 0, 0.7), 100, 0.5),
    sphere_t(Vect3(-0.3, 0, 0), 0.15, Vect3(0, 0.6, 0), 100, 0.5),
    sphere_t(Vect3(0, -9000, 0), 9000 - 0.7, Vect3(0.6, 0.6, 0.6), 100, 0.5),
};

void clampF(float& x, const float& min, const float& max) {
    if (x < min)
        x = min;
    else if (x > max)
        x = max;
}

void traceRay(Vect3& color, Vect3 origin, Vect3 direction, int maxDepth) {
    Vect3 tmpColor;
    float reflection = 1;

    for (int k=0 ; k<maxDepth ; k++) {
        float minDistance = std::numeric_limits<float>::infinity();
        sphere_t* nearest = NULL;
        float distance;
        for (sphere_t& obj : objects) {
            distance = obj.intersect(origin, direction);
            if (distance > -1 && distance < minDistance) {
                minDistance = distance;
                nearest = &obj;
            }
        }

        if (nearest == NULL) {
            break;
        }

        Vect3 intersection = origin + minDistance * direction;
        Vect3 normal = nearest->normal(intersection);
        Vect3 shiftedPoint = intersection + 0.001 * normal;

        Vect3 toLight = (light.position - shiftedPoint).normalized();
        bool isShadowed = false;
        for (sphere_t& obj : objects) {
            if (obj.intersect(shiftedPoint, toLight) > -1) {
                isShadowed = true;
                break;
            }
        }

        if (isShadowed) {
            break;
        }

        tmpColor.set(0, 0, 0);

        // ambient
        tmpColor.set(tmpColor + nearest->ambient * light.ambient);

        // diffuse
        float diffuseFactor = toLight.dot(normal);
        tmpColor.set(tmpColor + diffuseFactor * nearest->diffuse * light.diffuse);

        // specular
        Vect3 toCamera = (camera - shiftedPoint).normalized();
        Vect3 h = (toCamera + toLight).normalized();
        tmpColor.set(tmpColor + pow(h.dot(normal), nearest->shininess / 4) * nearest->specular * light.specular);

        // reflection
        color.set(color + reflection * tmpColor);
        reflection *= nearest->reflectivity;

        origin.set(shiftedPoint);
        direction.set(direction.reflected(normal));
    }

    clampF(color.x, 0, 1);
    clampF(color.y, 0, 1);
    clampF(color.z, 0, 1);
}

void writeToPixmap(const std::vector<std::vector<Vect3> >& image, std::string filepath) {
    int height = image.size();
    int width = image[0].size();
    char red, green, blue;
    Vect3 pixel;

    std::ofstream file(filepath.c_str());
    file << "P6" << std::endl;
    file << width << " " << height << std::endl;
    file << 255 << std::endl;
    for (int i=0 ; i<height ; i++) {
        for (int j=0 ; j<width ; j++) {
            pixel = image[i][j];
            red = pixel.x * 255;
            green = pixel.y * 255;
            blue = pixel.z * 255;
            file << red << green << blue;
        }
    }
    file.close();
}

int main(int argc, char const *argv[]) {
    float width = 1920;
    float height = 1080;

    std::vector<std::vector<Vect3> > image(height, std::vector<Vect3>(width, Vect3()));
    float ratio = width / height;

    Vect3 pixel, origin, direction;
    Vect3 upperLeft(-1, 1 / ratio, 0);
    Vect3 xVector(2 / width, 0, 0);
    Vect3 yVector(0, -2 / ratio / height, 0);

    for (int i=0 ; i<width ; i++) {
        for (int j=0 ; j<height ; j++) {
            pixel.set(upperLeft + i * xVector + j * yVector);
            traceRay(image[j][i], camera, (pixel - camera).normalized(), 5);
        }
        printf("\r%3d%c", int(100 * (i + 1) / width), '%');
    }
    printf("\nwriting image...");
    fflush(stdout);
    writeToPixmap(image, "out.ppm");
    printf("done.\n");
}
