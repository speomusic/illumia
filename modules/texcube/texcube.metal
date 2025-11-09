#include <metal_stdlib>
#include "metal-types.h"

using namespace metal;

struct RasterizerData
{
    float4 position [[position]];
    float4 color;
};

vertex RasterizerData vertexShader(uint vertexID [[vertex_id]],
                                   constant Vertex* vertices [[buffer(0)]])
{
    return { float4(vertices[vertexID].position, 0, 1), vertices[vertexID].color };
}

fragment float4 fragmentShader(RasterizerData in [[stage_in]])
{
    return in.color;
}
