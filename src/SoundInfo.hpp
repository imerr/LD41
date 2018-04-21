//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_SOUNDINFO_HPP
#define LD41_SOUNDINFO_HPP
#include <vector>
#include <memory>
#include <SFML/Audio/Sound.hpp>
enum class BeatType {
	None,
	Beat,
	Max
};

class SoundInfo {
	float m_beatTime;
	float m_playIn;
	float m_prevPlayTime;
	size_t m_cycles;
	size_t m_hitCycle;
public:
	SoundInfo();
	void Update(float interval);

	inline float LastPlayTime() {
		return -m_beatTime;
	}
	inline float NextPlayTime(int future = 0) {
		return m_playIn * (future + 1) - m_beatTime;
	}
	BeatType Type;
	float Interval;
	float Volume;
	float Offset;
	std::unique_ptr<sf::Sound> Sound;
	std::vector<float> Pitches;
	bool Played;
	bool IsHit;

	void Reset();

	bool IsCurrentHit();

	bool IsPrevHit();

	void Hit(bool b);
};

#endif //LD41_SOUNDINFO_HPP
