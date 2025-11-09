import("stdfaust.lib");
process = par(i, 2, *((os.lf_squarewave(5 + os.lf_squarewave(1.25)) * 0.5 + 0.5) : fi.lowpass(2, 80))) : re.dattorro_rev_default;
