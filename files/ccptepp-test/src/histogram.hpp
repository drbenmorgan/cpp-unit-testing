#pragma once
#include <stdexcept>
#include <vector>

/**
 * @brief A one-dimensional histogram with fixed-width bins.
 *
 * Bins are defined over the half-open interval [@p x_min, @p x_max).
 * Values outside this range are counted separately as underflow or overflow
 * and do not contribute to bin counts or the mean.
 *
 * All bin widths are equal: (@p x_max - @p x_min) / @p n_bins.
 */
class Histogram {
public:

    /**
     * @brief Construct a histogram with uniform binning.
     *
     * @param n_bins  Number of bins. Must be greater than zero.
     * @param x_min   Lower edge of the first bin (inclusive).
     * @param x_max   Upper edge of the last bin (exclusive).
     *
     * @throws std::invalid_argument if @p n_bins <= 0.
     * @throws std::invalid_argument if @p x_min >= @p x_max.
     */
    Histogram(int n_bins, float x_min, float x_max);

    /**
     * @brief Fill the histogram with a value.
     *
     * If @p x is in [@p x_min, @p x_max), the corresponding bin count is
     * incremented by @p weight. If @p x is outside this range, the underflow
     * or overflow counter is incremented instead; @p weight is ignored for
     * out-of-range values. The total entry count is always incremented.
     *
     * @param x       The value to fill.
     * @param weight  The weight to add to the bin count. Defaults to 1.0.
     */
    void fill(float x, float weight = 1.0f);

    /**
     * @brief Return the bin counts as a vector of length n_bins.
     *
     * Element @c i contains the sum of weights of all in-range values that
     * fell into bin @c i. Underflow and overflow are not included.
     */
    std::vector<float> bin_counts() const;

    /**
     * @brief Return the bin edges as a vector of length n_bins + 1.
     *
     * Element @c i is the lower edge of bin @c i; element @c n_bins is the
     * upper edge of the last bin, equal to @p x_max.
     */
    std::vector<float> bin_edges() const;

    /**
     * @brief Return the total number of fill() calls, including out-of-range values.
     */
    int n_entries() const;

    /**
     * @brief Return the number of fill() calls where x >= x_max.
     */
    int n_overflow() const;

    /**
     * @brief Return the number of fill() calls where x < x_min.
     */
    int n_underflow() const;

    /**
     * @brief Return the unweighted mean of all in-range filled values.
     *
     * Computed as the arithmetic mean of the @p x values passed to fill(),
     * excluding out-of-range values. The @p weight parameter of fill() does
     * not affect this calculation.
     *
     * @throws std::runtime_error if no in-range values have been filled.
     */
    float mean() const;

private:
    int   n_bins_;
    float x_min_, x_max_, bin_width_;
    std::vector<float> counts_;
    int   n_entries_   = 0;
    int   n_overflow_  = 0;
    int   n_underflow_ = 0;
    float value_sum_   = 0.0f;  // sum of x values, unweighted
    int   in_range_    = 0;
};
