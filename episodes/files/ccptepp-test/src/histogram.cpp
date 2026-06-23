#include "histogram.hpp"

Histogram::Histogram(int n_bins, float x_min, float x_max)
    : n_bins_(n_bins), x_min_(x_min), x_max_(x_max)
{
    if (n_bins <= 0)
        throw std::invalid_argument("n_bins must be > 0");
    if (x_min >= x_max)
        throw std::invalid_argument("x_min must be < x_max");

    bin_width_ = (x_max - x_min) / n_bins;
    counts_.assign(n_bins, 0.0f);
}

void Histogram::fill(float x, float weight) {
    ++n_entries_;
    if (x < x_min_) {
        ++n_underflow_;
        return;
    }
    if (x >= x_max_) {
        ++n_overflow_;
        return;
    }
    int bin = static_cast<int>((x - x_min_) / bin_width_);
    counts_[bin] += weight;
    value_sum_ += x;
    ++in_range_;
}

std::vector<float> Histogram::bin_counts() const {
    return counts_;
}

std::vector<float> Histogram::bin_edges() const {
    std::vector<float> edges(n_bins_ + 1);
    for (int i = 0; i <= n_bins_; ++i)
        edges[i] = x_min_ + i * bin_width_;
    return edges;
}

int Histogram::n_entries()   const { return n_entries_;   }
int Histogram::n_overflow()  const { return n_overflow_;  }
int Histogram::n_underflow() const { return n_underflow_; }

float Histogram::mean() const {
    if (in_range_ == 0)
        throw std::runtime_error("mean() called on empty histogram");
    return value_sum_ / in_range_;
}
