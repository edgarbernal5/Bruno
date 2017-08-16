#pragma once

#include <vector>
#include <functional>
#include <map>
#include <stdint.h>


namespace Cuado
{
	template <typename... EventArgs>
	class Event
	{
	public:
		typedef std::function<void(EventArgs...)> EventHandler;
		
		Event() = default;
		~Event() = default;
		Event(const Event &) = delete;
		Event & operator=(const Event &) = delete;

		void operator()(EventArgs... eventArgs)
		{
			for (auto eventHandler : m_funcs)
				eventHandler(eventArgs...);
		}

		Event & operator+=(const EventHandler &eventHandler)
		{
			long long dirrL = (long long)(&eventHandler);
			std::map<long long, uint32_t>::iterator iter = m_map.find(dirrL);

			if (iter == m_map.end())
			{
				m_map[dirrL] = m_funcs.size();
				m_funcs.push_back(eventHandler);
			}
			return *this;
		}

		Event & operator-=(const EventHandler &eventHandler)
		{
			long long dirrL = (long long)(&eventHandler);

			auto iter = m_map.find(dirrL);

			if (iter != m_map.end())
			{
				uint32_t index = iter->second;
				m_map.erase(iter);
				iter = m_map.begin();
				for (; iter != m_map.end(); iter++)
				{
					if (iter->second > index)
					{
						--iter->second;
					}
				}
				m_funcs.erase(m_funcs.begin() + index);
			}
			return *this;
		}
	private:
		std::vector<EventHandler> m_funcs;
		std::map<long long, uint32_t> m_map;

	};
}
