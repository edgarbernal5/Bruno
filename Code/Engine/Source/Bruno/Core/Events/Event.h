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
        
        Event() : data(std::make_shared<Data>()) {}
        Event(Event&& other) : Event() { *this = std::move(other); }
        
        Event& operator=(Event&& other)
        {
            std::swap(data, other.data);
            return *this;
        }

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
            HandlerId IdCounter = 0;
            HandlerList observers;
            std::mutex observerMutex;
        };

        std::shared_ptr<Data> data;

        HandlerId AddHandler(Handler h) const
        {
            std::lock_guard<std::mutex> lock(data->observerMutex);
            data->observers.emplace_back(StoredHandler{ data->IdCounter, std::make_shared<Handler>(h) });
            return data->IdCounter++;
        }

    public:

        HandlerId connect(const Handler& h) const
        { 
            return AddHandler(h);
        }

        void disconnect(HandlerId id) const
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