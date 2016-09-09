#include "vn/math_utilities.h"
#include <random>

// TODO -- evaluate this implementation later on for both speed and randomness..

namespace vn {
namespace math {

    namespace {
        std::seed_seq const& gimme_seed_seq()
        {
            static std::random_device rd{};
            static std::seed_seq seq{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
            return seq;
        }
    }

    float rand_zero_one()
    {
        thread_local std::uniform_real_distribution<float> dist_{ 0.0f, 1.0f };
        thread_local std::mt19937 rng_{ gimme_seed_seq() };
        return dist_(rng_);
    }

    float rand_neg_one_one()
    {
        thread_local std::uniform_real_distribution<float> dist_{ -1.0f, 1.0f };
        thread_local std::mt19937 rng_{ gimme_seed_seq() };
        return dist_(rng_);
    }
}
} // namespace