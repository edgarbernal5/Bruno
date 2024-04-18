#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

namespace Bruno
{
    using EventHandlerId = size_t;

    template <typename... Args>
    class Event
    {
    public:
        using Handler = std::function<void(const Args &...)>;
        
        Event() : data(std::make_shared<Data>()) {}
        Event(Event&& other) : Event() { *this = std::move(other); }
        
        Event& operator=(Event&& other)
        {
            std::swap(data, other.data);
            return *this;
        }

        Event(const Event&) = default;
        Event& operator=(const Event&) = default;

    private:

        struct StoredHandler
        {
            EventHandlerId id;
            std::shared_ptr<Handler> callback;
        };

        using HandlerList = std::vector<StoredHandler>;

        struct Data
        {
            EventHandlerId IdCounter = 0;
            HandlerList observers;
            std::mutex observerMutex;
        };

        std::shared_ptr<Data> data;

        EventHandlerId AddHandler(Handler h) const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            data->observers.emplace_back(StoredHandler{ data->IdCounter, std::make_shared<Handler>(h) });
            return data->IdCounter++;
        }

    public:

        EventHandlerId connect(const Handler& h) const
        { 
            return AddHandler(h);
        }

        void disconnect(EventHandlerId id) const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            auto it = std::find_if(data->observers.begin(), data->observers.end(),
                [&](auto& o)
                { 
                    return o.id == id; 
                });
            
            if (it != data->observers.end())
            {
                data->observers.erase(it);
            }
        }

        void reset() const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            data->observers.clear();
        }

        void emit(Args... args) const
        {
            std::vector<std::weak_ptr<Handler>> handlers;
            {
                std::lock_guard<std::mutex> lock(data->observerMutex);
                handlers.resize(data->observers.size());
                std::transform(data->observers.begin(), data->observers.end(), handlers.begin(),
                    [](auto& h) { return h.callback; });
            }

            for (auto& weakCallback : handlers)
            {
                if (auto callback = weakCallback.lock())
                {
                    (*callback)(args...);
                }
            }
        }
    };
}