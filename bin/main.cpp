#include "config.h"

#ifdef ALSA
#include <send/alsa.h>
#else
#include <send/pulse.h>
#endif
#include <read/wav.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct TContext {
    std::deque<std::pair<std::chrono::time_point<std::chrono::steady_clock>, std::vector<std::uint8_t>>> queue;
    std::mutex mutex;
    std::condition_variable sendCv;
    std::condition_variable readCv;

    bool end = false;
};

using TContextPtr = std::shared_ptr<TContext>;

void Send(TContextPtr ctx) noexcept {
    NSend::TSendPtr send = std::make_unique<NSend::TSend>(BITS_PER_SAMPLE, CHANNELS, RATE, DEVICE);

    if (auto ec = send->Init(); ec) {
        std::cerr << "send init error: " << ec.message() << std::endl;
        return;
    }

    while (true) {
        std::unique_lock<std::mutex> ulock{ctx->mutex};
        ctx->sendCv.wait(ulock, [ctx] { return !ctx->queue.empty() || ctx->end; });

        if (ctx->end) {
            return;
        }

        auto data = std::move(ctx->queue.front());
        ctx->queue.pop_front();

        ulock.unlock();
        ctx->readCv.notify_one();

        auto delta = std::chrono::microseconds(100);
        auto now = std::chrono::steady_clock::now();

        if (now - data.first > delta) {
            continue;
        } else if (data.first - now > delta) {
            std::this_thread::sleep_until(data.first);
        }

        send->Send(std::move(data.second));
    }
}

void Read(TContextPtr ctx, std::vector<std::filesystem::path> files) noexcept {    
    for (const auto& file: files) {
        std::cerr << file.filename().string() << std::endl;

        auto time = std::chrono::steady_clock::now() + std::chrono::milliseconds(DELAY_MS * MAX_SIZE_QUEUE);

        NRead::TReadPtr read = std::make_unique<NRead::TWav>(
            file.string(), BITS_PER_SAMPLE, CHANNELS, RATE, DATA_SIZE);

        if (auto ec = read->Init(); ec) {
            std::cerr << "read init error: " << ec.message() << std::endl;
            return; 
        }

        while (true) {
            std::unique_lock<std::mutex> ulock{ctx->mutex};
            ctx->readCv.wait(ulock, [ctx] { return ctx->queue.size() < MAX_SIZE_QUEUE; });

            if (auto result = read->Rcv(); result) {
                ctx->queue.emplace_back(std::make_pair(time, std::move(result).value()));
                time += std::chrono::milliseconds(DELAY_MS);
            } else {
                break;
            }

            ulock.unlock();
            ctx->sendCv.notify_one();
        }
    }

    ctx->end = true;
    ctx->sendCv.notify_one();
} 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "run: play dir" << std::endl;
        return 1;
    }

    std::filesystem::path path = std::string{argv[1]};
    std::vector<std::filesystem::path> files;

    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() ==  ".wav") {
                files.push_back(entry.path());
            }
        }
    } else {
        if (path.extension() == ".wav") {
            files.push_back(path);
        }
    }

    std::sort(files.begin(), files.end());

    auto ctx = std::make_shared<TContext>();
    
    std::thread tSend(Send, ctx);
    std::thread tRead(Read, ctx, std::move(files));

    tRead.join();
    tSend.join();

    return 0;
}
