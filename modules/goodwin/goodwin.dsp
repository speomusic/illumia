import("stdfaust.lib");
// process(g, n, o) = 0.1 * en.ar(0.01, 1, g), os.square(ba.midikey2hz(n) * 2^o) + no.noise : * <: _,_;
process = 0.1 * en.asr(0.1, 1, 1, 1) * os.osc(440) <: _,_;
