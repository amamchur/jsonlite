#include <atomic>
#include <condition_variable>
#include <iostream>
#include <jsonlite.h>
#include <mutex>
#include <string.h>
#include <thread>

std::atomic<size_t> threads_waiting;
std::condition_variable wait_variable;
std::mutex mutex;

typedef struct collision_struct {
    const char *value;
    uint32_t hash;
} collision_struct;

collision_struct collisions[] = {{"collision_a", 0},
                                 {"collision_b", 0},
                                 {"collision_c", 0},
                                 {"collision_d", 0},
                                 {"collision_e", 0},
                                 {"collision_f", 0},
                                 {"collision_g", 0},
                                 {"collision_h", 0}};

auto collision_n = sizeof(collisions) / sizeof(collisions[0]);

void thread_done() {
    threads_waiting--;
    if (threads_waiting == 0) {
        std::unique_lock<std::mutex> lock(mutex);
        wait_variable.notify_all();
    }
}

void fixed_length_collision(uint64_t offset, uint64_t step) {
    char text[256];

    for (uint64_t i = offset; i < (1ul << 63); i += step) {
        strcpy(text, "collision_");
        auto ptr = text + strlen(text);
        auto value = i;
        for (int q = 0; q < sizeof(i) * 2; q++) {
            auto hex = value & 0xF;
            *ptr++ = (hex < 9 ? '0' : 'A') + hex;
            value >>= 4;
        }

        *ptr++ = 0;
        auto count = strlen(text);
        auto hash = jsonlite_hash(text, static_cast<size_t>(count));

        for (int j = 0; j < collision_n; j++) {
            if (hash == collisions[j].hash) {
                std::cout << "Collision found \"" << collisions[j].value << "\" & \"" << text << "\"" << std::endl;
            }
        }
    }

    thread_done();
}

void variable_length_collision(uint64_t offset, uint64_t step) {
    char text[256];

    for (uint64_t i = offset; i < (1ul << 63); i += step) {
        strcpy(text, "collision_");
        auto ptr = text + strlen(text);
        auto value = i;
        for (int q = 0; value != 0 && q < sizeof(i) * 2; q++) {
            auto hex = value & 0x3F;
            *ptr++ = '!' + hex;
            value >>= 6;
        }

        *ptr++ = 0;
        auto count = strlen(text);
        auto hash = jsonlite_hash(text, static_cast<size_t>(count));

        for (int j = 0; j < collision_n; j++) {
            if (hash == collisions[j].hash) {
                std::cout << "Collision found \"" << collisions[j].value << "\" & \"" << text << "\"" << std::endl;
            }
        }
    }

    thread_done();
}

int main() {
    auto threads_count = std::thread::hardware_concurrency();
    threads_waiting = threads_count;

    std::cout << threads_count << " concurrent threads are supported." << std::endl;

    for (int i = 0; i < collision_n; i++) {
        auto value = collisions[i].value;
        collisions[i].hash = jsonlite_hash(value, strlen(value));
        std::cout << "Value: " << value << " Hash: 0x" << std::hex << collisions[i].hash << std::endl;
    }

    for (int i = 0; i < threads_count; i++) {
        std::thread t(variable_length_collision, i, threads_count);
        t.detach();
    }

    std::unique_lock<std::mutex> lock(mutex);
    wait_variable.wait(lock);

    return 0;
}
