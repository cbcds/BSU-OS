#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) {
        buffer_size_ = size;
        can_send_ = true;
        can_recv_ = false;
        is_closed_ = false;
    }

    void Send(T value) {
        if (is_closed_) {
            throw std::runtime_error("channel is closed");
        }

        std::unique_lock<std::mutex> locker(lock_);
        if (q_.size() == buffer_size_) {
            can_send_ = false;
        }
        continue_send_.wait(locker, [this]() { return (can_send_ || is_closed_); });
        if (is_closed_) {
            throw std::runtime_error("channel is closed");
        }
        q_.push(std::move(value));
        can_recv_ = true;
        continue_recv_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> locker(lock_);
        if (q_.empty()) {
            if (!is_closed_) {
                can_recv_ = false;
            } else {
                return std::make_pair(T(), false);
            }
        }
        continue_recv_.wait(locker, [this]() { return can_recv_ || is_closed_; });
        if (q_.empty()) {
            return std::make_pair(T(), false);
        }
        std::pair<T, bool> elem = std::make_pair(q_.front(), true);
        q_.pop();
        if (!is_closed_) {
            can_send_ = true;
            continue_send_.notify_one();
        }
        return elem;
    }

    void Close() {
        is_closed_ = true;
        can_send_ = false;
        can_recv_ = true;
        continue_send_.notify_all();
        continue_recv_.notify_all();
    }

private:
    std::queue<T> q_;
    int buffer_size_;

    std::mutex lock_;
    std::condition_variable continue_send_;
    std::condition_variable continue_recv_;

    bool can_send_;
    bool can_recv_;
    bool is_closed_;
};

#endif // BUFFERED_CHANNEL_H_