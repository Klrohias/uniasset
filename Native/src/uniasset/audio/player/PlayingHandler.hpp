#pragma once
#ifndef UNIASSET_PLAYINGHANDLER_H
#define UNIASSET_PLAYINGHANDLER_H

#include <memory>
#include <system_error>
#include "uniasset/common/Result.hpp"
#include "IWaveProvider.hpp"

namespace uniasset {
	class PlayingHandler : public std::enable_shared_from_this<PlayingHandler> {
	public:
		enum State {
			Paused = 0,
			Playing = 1,
			Stopped = 2
		};

		explicit PlayingHandler();

		std::error_code pause();

		std::error_code play();

		std::error_code stop();

		State getState() const noexcept;

		float getTime() const noexcept;

		std::error_code setTime(float time);

	private:
		float time_{ 0.0f };
		State state_{ Paused };
	};
}

#endif // UNIASSET_PLAYINGHANDLER_H
