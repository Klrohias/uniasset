#include "PlayingHandler.hpp"
#include "uniasset/common/Errors.hpp"

namespace uniasset {
	PlayingHandler::PlayingHandler() {
	}

	std::error_code PlayingHandler::pause() {
		state_ = State::Paused;
		return err_ok();
	}

	std::error_code PlayingHandler::play() {
		state_ = State::Playing;
		return err_ok();
	}

	std::error_code PlayingHandler::stop() {
		state_ = State::Stopped;
		return err_ok();
	}

	PlayingHandler::State PlayingHandler::getState() const noexcept {
		return state_;
	}

	float PlayingHandler::getVolume() const noexcept
	{
		return volume_;
	}

	std::error_code PlayingHandler::setVolume(float volume)
	{
		volume_ = volume;
		return err_ok();
	}

	float PlayingHandler::getTime() const noexcept {
		return time_;
	}

	std::error_code PlayingHandler::setTime(float time) {
		time_ = time;
		return err_ok();
	}
}