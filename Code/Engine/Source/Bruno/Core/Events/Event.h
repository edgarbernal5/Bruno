#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <mutex>

namespace Bruno
{
    template <typename... Args>
    class Event
    {
    public:
        using Handler = std::function<void(const Args &...)>;
        
        Event() = default;

    protected:
        Event(const Event&) = default;
        Event& operator=(const Event&) = default;

    private:
        using HandlerId = size_t;

        struct StoredHandler
        {
            HandlerId id;
            std::shared_ptr<Handler> callback;
        };

        using HandlerList = std::vector<StoredHandler>;

        struct Data
        {
            HandlerId IDCounter = 0;
            HandlerList observers;
            std::mutex observerMutex;
        };

        std::shared_ptr<Data> data;

        HandlerId AddHandler(Handler h) const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            data->observers.emplace_back(StoredHandler{ data->IDCounter, std::make_shared<Handler>(h) });
            return data->IDCounter++;
        }

    public:

        HandlerId connect(const Handler& h) const { return AddHandler(h); }

        void disconnect(HandlerId id) const { Observer(data, id).reset(); }

        void reset() const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            data->observers.clear();
        }
    };
}