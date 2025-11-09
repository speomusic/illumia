import("stdfaust.lib");
gate = ba.pulsen(ba.sec2samp(0.1), ba.sec2samp(0.15));
index = ba.counter(gate) - 1;
semitones = waveform{0, 7, 14, 16, 23}, index % 5 : rdtable;
process = gate, 62 + semitones;
