#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>
#include <immintrin.h>

#include "./benchmark.hpp"

volatile float g_sink = 0.0f;

struct Particle
{
    float x;
    float y;
    float z;

    float vx;
    float vy;
    float vz;
};

struct ParticleSoA
{
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> z;

    std::vector<float> vx;
    std::vector<float> vy;
    std::vector<float> vz;

    explicit ParticleSoA(std::size_t count)
        : x(count),
          y(count),
          z(count),
          vx(count),
          vy(count),
          vz(count)
    {
    }
};

void UpdateAoS(std::vector<Particle>& particles,  float dt)
{
    for (auto& p: particles)
    {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

void UpdateAVX2(ParticleSoA &particles, std::size_t count, float dt)
{
    const __m256 dt8 = _mm256_set1_ps(dt);
    std::size_t i = 0;
    for (; i + 8 <= count; i += 8)
    {
        __m256 x = _mm256_loadu_ps(&particles.x[i]);
        __m256 vx = _mm256_loadu_ps(&particles.vx[i]);
        x = _mm256_add_ps(x, _mm256_mul_ps(vx, dt8));
        _mm256_storeu_ps(&particles.x[i], x);

        __m256 y = _mm256_loadu_ps(&particles.y[i]);
        __m256 vy = _mm256_loadu_ps(&particles.vy[i]);
        y = _mm256_add_ps(y, _mm256_mul_ps(vy, dt8));
        _mm256_storeu_ps(&particles.y[i], y);

        __m256 z = _mm256_loadu_ps(&particles.z[i]);
        __m256 vz = _mm256_loadu_ps(&particles.vz[i]);
        z = _mm256_add_ps(z, _mm256_mul_ps(vz, dt8));
        _mm256_storeu_ps(&particles.z[i], z);
    }
    for (; i < count; ++i)
    {
        particles.x[i] += particles.vx[i] * dt;
        particles.y[i] += particles.vy[i] * dt;
        particles.z[i] += particles.vz[i] * dt;
    }
}

void UpdateSoA(ParticleSoA &particles, std::size_t count, float dt)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        particles.x[i] += particles.vx[i] * dt;
        particles.y[i] += particles.vy[i] * dt;
        particles.z[i] += particles.vz[i] * dt;
    }
}

struct Statistics
{
    double mean;
    double median;
    double minimum;
    double maximum;
    double stddev;
    double p95;
};

Statistics Analyze(std::vector<double> samples)
{
    std::sort(samples.begin(), samples.end());

    double sum = 0.0;
    for (double value : samples)
    {
        sum += value;
    }
    const double mean = sum / static_cast<double>(samples.size());

    double squaredDifferenceSum = 0.0;
    for (double value : samples)
    {
        const double difference = value - mean;
        squaredDifferenceSum += difference * difference;
    }

    const std::size_t middle = samples.size() / 2;
    const double median = samples.size() % 2 == 0
        ? (samples[middle - 1] + samples[middle]) / 2.0
        : samples[middle];
    const std::size_t p95Index =
        static_cast<std::size_t>(std::ceil(samples.size() * 0.95)) - 1;

    return {
        mean,
        median,
        samples.front(),
        samples.back(),
        std::sqrt(squaredDifferenceSum / static_cast<double>(samples.size())),
        samples[p95Index]
    };
}

void PrintStatistics(std::string_view name, const std::vector<double>& samples)
{
    const Statistics stats = Analyze(samples);
    std::cout << std::left << std::setw(6) << name
              << std::right
              << std::setw(11) << stats.mean
              << std::setw(11) << stats.median
              << std::setw(11) << stats.minimum
              << std::setw(11) << stats.maximum
              << std::setw(11) << stats.stddev
              << std::setw(11) << stats.p95 << '\n';
}

int ParseRepeatCount(int argc, char* argv[])
{
    constexpr int DefaultRepeatCount = 50;
    if (argc == 1)
    {
        return DefaultRepeatCount;
    }
    if (argc != 2)
    {
        return 0;
    }

    int repeatCount = 0;
    const std::string_view argument(argv[1]);
    const auto result = std::from_chars(
        argument.data(), argument.data() + argument.size(), repeatCount);
    if (result.ec != std::errc{} ||
        result.ptr != argument.data() + argument.size() ||
        repeatCount <= 0)
    {
        return 0;
    }
    return repeatCount;
}

int main(int argc, char* argv[])
{
    const int repeatCount = ParseRepeatCount(argc, argv);
    if (repeatCount == 0)
    {
        std::cerr << "Usage: " << argv[0] << " [positive repeat count]\n";
        return 1;
    }

    // prepare
    constexpr std::size_t N = 4'000'000;
    constexpr float dt = 0.016f;

    std::vector<Particle> aos(N);
    ParticleSoA soa(N);
    ParticleSoA avx(N);

    for (std::size_t i = 0; i < N; ++i)
    {
        float f = static_cast<float>(i % 1000);
        aos[i] = {f, f + 1, f + 2, 0.1f, 0.2f, 0.3f};
        soa.x[i] = avx.x[i] = f;
        soa.y[i] = avx.y[i] = f + 1;
        soa.z[i] = avx.z[i] = f + 2;

        soa.vx[i] = avx.vx[i] = 0.1f;
        soa.vy[i] = avx.vy[i] = 0.2f;
        soa.vz[i] = avx.vz[i] = 0.3f;
    }
    //warmup
    for(std::size_t i = 0; i < 5; ++i)
    {
        UpdateAoS(aos, dt);
        UpdateSoA(soa, N, dt);
        UpdateAVX2(avx, N, dt);
    }
    //test
    constexpr int Iterations = 100;

    std::vector<double> aosSamples;
    std::vector<double> soaSamples;
    std::vector<double> avxSamples;
    aosSamples.reserve(repeatCount);
    soaSamples.reserve(repeatCount);
    avxSamples.reserve(repeatCount);

    for (int repeat = 0; repeat < repeatCount; ++repeat)
    {
        aosSamples.push_back(BenchMark(
            [&]()
            {
                UpdateAoS(aos, dt);
            },
            Iterations) / Iterations);

        soaSamples.push_back(BenchMark(
            [&]()
            {
                UpdateSoA(soa, N, dt);
            },
            Iterations) / Iterations);

        avxSamples.push_back(BenchMark(
            [&]()
            {
                UpdateAVX2(avx, N, dt);
            },
            Iterations) / Iterations);
    }

    float checksum = 0.0f;

    for (const auto& p : aos)
    {
        checksum += p.x + p.y + p.z;
    }

    for (size_t i = 0; i < N; ++i)
    {
        checksum +=
            soa.x[i] +
            soa.y[i] +
            soa.z[i];

        checksum +=
            avx.x[i] +
            avx.y[i] +
            avx.z[i];
    }

    g_sink = checksum;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "repeat: " << repeatCount
              << ", iterations/repeat: " << Iterations
              << ", unit: ms/frame\n\n";
    std::cout << std::left << std::setw(6) << "Type"
              << std::right
              << std::setw(11) << "Mean"
              << std::setw(11) << "Median"
              << std::setw(11) << "Min"
              << std::setw(11) << "Max"
              << std::setw(11) << "StdDev"
              << std::setw(11) << "P95" << '\n';
    PrintStatistics("AoS", aosSamples);
    PrintStatistics("SoA", soaSamples);
    PrintStatistics("AVX2", avxSamples);

    std::cout << "\nchecksum: " << g_sink << '\n';
    
    return 0;
}
