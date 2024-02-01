#pragma once

#include <spdlog/sinks/dist_sink.h>

template<typename Mutex>
class NewlineSplitterDistSink final : public spdlog::sinks::dist_sink<Mutex>
{
public:
	NewlineSplitterDistSink() = default;
	NewlineSplitterDistSink(spdlog::sinks_init_list Sinks) : spdlog::sinks::dist_sink<Mutex>(Sinks) {}
	NewlineSplitterDistSink(const NewlineSplitterDistSink&) = delete;
	NewlineSplitterDistSink& operator=(const NewlineSplitterDistSink&) = delete;

protected:
	void sink_it_(const spdlog::details::log_msg& Message) override
	{
		// Split the original payload into separate string views based on a newline delimiter
		std::vector<std::string_view> payloadViews;
		std::string_view v(Message.payload);

		for (size_t next = 0; (next = v.find('\n')) != std::string_view::npos;)
		{
			payloadViews.push_back(v.substr(0, next));
			v.remove_prefix(next + 1);
		}

		if (!v.empty())
			payloadViews.push_back(v);

		// Broadcast to the sinks
		auto messageCopy = Message;

		for (auto& sink : spdlog::sinks::dist_sink<Mutex>::sinks_)
		{
			if (!sink->should_log(messageCopy.level))
				continue;

			for (auto& payload : payloadViews)
			{
				messageCopy.payload = payload;
				sink->log(messageCopy);
			}
		}
	}
};

using NewlineSplitterDistSinkMT = NewlineSplitterDistSink<std::mutex>;
using NewlineSplitterDistSinkST = NewlineSplitterDistSink<spdlog::details::null_mutex>;