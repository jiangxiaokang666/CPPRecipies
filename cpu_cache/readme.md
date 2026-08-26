# cpu指令支持
- AVX2
- FMA
- AoS可能被编译器自动向量化

# OOD和DOD
- OOD(Object-Oriented Design)
- DOD(Data-Oriented Design)
# AoSoA
```c++
constexpr std::size_t SIMD_WIDTH = 8;
struct ParticleBlock
{
    float px[SIMD_WIDTH];
    float py[SIMD_WIDTH];
    float pz[SIMD_WIDTH];
    
    float vx[SIMD_WIDTH];
    float vy[SIMD_WIDTH];
    float vz[SIMD_WIDTH];

    float life[SIMD_WIDTH];
};

std::vector<ParticleBlock> blocks;//1个block8个particle
```
> 好处：
> 固定SIMD batch
> 局部性好
> chunk管理方便
> 容易并行