#version 130

in vec2 fragPosition;
in vec4 fragColor;
out vec4 color;

int maxIterations = 200;

int isInMandelbrotSet(vec2 point);
vec2 cartesianToPolar(vec2 cartesianVector);
vec2 polarToCartesian(vec2 polarVector);
vec2 complexMultiply(vec2 a, vec2 b);
int iterationsBeforeRepeat(vec2 point);
vec2 complexPow(vec2 cartesianNumber, float power);
uniform float zPower;

void main() {
    vec2 position = fragPosition;
    //position.x -= 0.5;
    int iterations = iterationsBeforeRepeat(position);
    if(iterations != -1) {
        color.r = 0.25 * (iterations) / float(maxIterations);
        color.g = 0.5 * (iterations) / float(maxIterations);
        color.b = 0.75 * (iterations) / float(maxIterations);
    } else {
        color.rgb = vec3(0, 0, 0);
    }
}

vec2 cartesianToPolar(vec2 cartesianVector) {
    float r = sqrt(float(dot(cartesianVector, cartesianVector)));
    float theta = atan(cartesianVector.y / float(cartesianVector.x));
    return vec2(r, theta);
}

vec2 polarToCartesian(vec2 polarVector) {
    return vec2(polarVector[0] * cos(polarVector[1]), polarVector[0] * sin(polarVector[1]));
}

vec2 complexMultiply(vec2 a, vec2 b) {
    vec2 result = vec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
    return result;
}

int iterationsBeforeRepeat(vec2 point) {
    vec2 Z = point;
    int i;
    for(i = 0; i < maxIterations; i++) {
        Z = 2 * complexPow(Z, zPower) + point;
        if(dot(Z, point) > 100*100) {
            return i;
        }
    }
    return -1;
}

vec2 complexPow(vec2 cartesianNumber, float power) {
    vec2 polar = cartesianToPolar(cartesianNumber);
    polar[0] = pow(polar[0], power);
    polar[1] *= power;
    return polarToCartesian(polar);
}
