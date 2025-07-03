#pragma once
#include <cstdint>
using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

    void sha256(long iterations);
    void p3np1E(unsigned long a, unsigned long * steps);
    void primes(unsigned long a, unsigned long * steps);
    void avx(float * a, float * b, float * c);
    void floodL1L2(void* buffer, unsigned long * iterations_ptr, size_t buffer1_size);
    void floodMemory(void* buffer, unsigned long * iterations_ptr, size_t buffer_size);
    void rowhammerAttack(void* buffer, unsigned long * iterations_ptr, size_t buffer_size);
    void floodNt(void * buffer, unsigned long * iterations_ptr, size_t buffer_size);
    void aes128EncryptBlock(void * out, const void * in, const void * key);
    void aes256Keygen(void* expanded_key);
    void aesXtsEncrypt(void * out, const void * in, const void* key, const void * tweak, size_t blocks);
    void aes128DecryptBlock(void * out, const void * in, const void * key);
    void aesXtsDecrypt(void * out, const void * in, const void* key, const void * tweak, size_t blocks);
    void diskWrite(const char * name);
    void startLZMA(int duration);
    void spawn_system_monitor();
    void stop_system_monitor();
    void renderPixel(int iterations, int thread_id, float* output);
    void branchTorture(unsigned long iterations, int pattern_type);
    void cacheL1Test(unsigned long iterations, void* buffer);
    void cacheL2Test(unsigned long iterations, void* buffer);
    void cacheL3Test(unsigned long iterations, void* buffer);
    void memoryLatencyTest(unsigned long iterations, void* buffer, size_t buffer_size);
//
#ifdef __cplusplus
}
#endif
