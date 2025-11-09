import("stdfaust.lib");
process = 0.02*os.osc(vslider("freq", 0.2, 0.1, 6, 0.01));
