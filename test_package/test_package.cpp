/**
 * KFR (http://kfrlib.com)
 * Copyright (C) 2016  D Levin
 * See LICENSE.txt for details
 */

#include <kfr/base.hpp>
#ifdef HAVE_DFT
#include <kfr/dft.hpp>
#endif
#include <kfr/dsp.hpp>
#include <kfr/io.hpp>

using namespace kfr;

int main()
{
    // Show KFR version
    println(library_version());

    // --------------------------------------------------------------------------------------
    // --------------------- FIR filter design (using window functions) ---------------------
    // --------------------------------------------------------------------------------------

    // Options for dspplot
    const std::string options = "phaseresp=True";

    univector<fbase, 15> taps15;
    univector<fbase, 127> taps127;
    univector<fbase, 8191> taps8191;

    // Prepare window functions (only expression saved here, not data)
    expression_pointer<fbase> hann = to_pointer(window_hann(taps15.size()));

    expression_pointer<fbase> kaiser = to_pointer(window_kaiser(taps127.size(), 3.0));

    expression_pointer<fbase> blackman_harris = to_pointer(window_blackman_harris(taps8191.size()));

    // Fill taps15 with the low pass FIR filter coefficients using hann window and cutoff=0.15
    fir_lowpass(taps15, 0.15, hann, true);

    // Fill taps127 with the low pass FIR filter coefficients using kaiser window and cutoff=0.2
    fir_lowpass(taps127, 0.2, kaiser, true);

    // Fill taps127 with the high pass FIR filter coefficients using kaiser window and cutoff=0.2
    fir_highpass(taps127, 0.2, kaiser, true);

    // Fill taps127 with the band pass FIR filter coefficients using kaiser window and cutoff=0.2 and 0.4
    fir_bandpass(taps127, 0.2, 0.4, kaiser, true);

    // Fill taps127 with the band stop FIR filter coefficients using kaiser window and cutoff=0.2 and 0.4
    fir_bandstop(taps127, 0.2, 0.4, kaiser, true);

    // Fill taps8191 with the low pass FIR filter coefficients using blackman harris window and cutoff=0.15
    fir_lowpass(taps8191, 0.15, blackman_harris, true);
    univector<fbase, 8191 + 150> taps8191_150 = scalar(0);

    // Shift by 150 samples
    taps8191_150.slice(150) = taps8191;

    // --------------------------------------------------------------------------------------
    // -------------------------- Using FIR filter as an expression -------------------------
    // --------------------------------------------------------------------------------------

    // Prepare 10000 samples of white noise
    univector<float> noise = truncate(gen_random_range(random_bit_generator{1, 2, 3, 4}, -1.f, +1.f), 10000);

    // Apply band stop filter
    univector<float> filtered_noise = fir(noise, taps127);

    // --------------------------------------------------------------------------------------
    // ------------------------------- FIR filter as a class --------------------------------
    // --------------------------------------------------------------------------------------

    fir_bandpass(taps127, 0.2, 0.4, kaiser, true);
    // Initialize FIR filter with float input/output and fbase taps
    filter_fir<fbase, float> fir_filter(taps127);

    // Apply to univector, static array, data by pointer or anything
    univector<float> filtered_noise2;
    fir_filter.apply(filtered_noise2, noise);

#ifdef HAVE_DFT
    // --------------------------------------------------------------------------------------
    // ---------------------- Convolution filter (optimized using DFT) ----------------------
    // --------------------------------------------------------------------------------------

    // Initialize FIR filter with float input/output and fbase taps
    convolve_filter<float> conv_filter(taps127);

    // Apply to univector, static array, data by pointer or anything
    univector<float> filtered_noise3;
    conv_filter.apply(filtered_noise3, noise);

#if PYTHON_IS_INSTALLED
    // Plot results, same as filtered_noise2
    plot_save("filtered_noise3", filtered_noise3, "title='Filtered noise 3', div_by_N=True");
#endif
#endif

    println("SVG plots have been saved to svg directory");

    return 0;
}
