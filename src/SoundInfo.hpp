//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_SOUNDINFO_HPP
#define LD41_SOUNDINFO_HPP
#include <vector>
#include <memory>
#include <SFML/Audio/Sound.hpp>

enum class BeatType {
	Left,
	Down,
	Up,
	Right,
	Max
};
class Level;
class SoundInfo {
	float m_beatTime;
	float m_playIn;
	size_t m_cycles;
	size_t m_hitCycle;
	Level* m_level;
public:
	SoundInfo(Level* level);
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
	bool Played;
	bool IsHit;
	int TowerRequirement[4][2];

	void Reset();

	bool IsCurrentHit();

	bool IsPrevHit();

	bool IsPlaying();

	void Hit(bool b);
};

#endif //LD41_SOUNDINFO_HPP
