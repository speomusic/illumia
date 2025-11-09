#include <metal_stdlib>
#include "metal-types.h"

kernel void bloomExtract(metal::texture2d<half, metal::access::read> in [[texture(0)]],
                         metal::texture2d<half, metal::access::write> out [[texture(1)]],
                         uint2 gid [[thread_position_in_grid]],
                         constant BloomParams* bloomParams [[buffer(0)]])
{
    half4 color = in.read(gid);
    half gray = metal::dot(color.rgb, half3(0.2126, 0.7152, 0.0722));
    out.write(gray >= bloomParams->threshold ? color : half4(0, 0, 0, 1), gid);
}

kernel void bloomOverlay(metal::texture2d<half, metal::access::read> in [[texture(0)]],
                         metal::texture2d<half, metal::access::read> overlay [[texture(1)]],
                         metal::texture2d<half, metal::access::write> out [[texture(2)]],
                         uint2 gid [[thread_position_in_grid]],
                         constant BloomParams* bloomParams [[buffer(0)]])
{
    half4 color = in.read(gid);
    half4 overlayColor = overlay.read(gid);
    out.write(half4(color.rgb + bloomParams->intensity * overlayColor.rgb, color.a), gid);
}
