//
// Created by iMer on 21.04.2018.
//

#include "SoundInfo.hpp"
#include "Level.hpp"


SoundInfo::SoundInfo(Level* level) : Interval(1), Volume(1), Offset(0), Sound(nullptr),
						 m_beatTime(std::numeric_limits<float>::infinity()), Played(false), m_cycles(0), m_hitCycle(0),
						 IsHit(false), TowerRequirement{{0, 999},
														{0, 999},
														{0, 999},
														{0, 999}}, m_level(level) {

}

void SoundInfo::Update(float interval) {
	Played = false;
	if (!IsPlaying()) {
		return;
	}
	m_beatTime += interval;
	if (m_beatTime >= m_playIn) {
		m_beatTime -= m_playIn;
		m_playIn = Interval;
		Sound->play();
		Played = true;
		m_cycles++;
	}
	if (m_cycles > m_hitCycle && m_beatTime > m_playIn / 2) {
		IsHit = false;
	}
}

void SoundInfo::Reset() {
	m_beatTime = -Offset;
	m_playIn = Interval;
	m_cycles = 0;
	m_hitCycle = 0;
	IsHit = false;
	Played = false;
	Sound->setVolume(Volume * 100);
}

void SoundInfo::Hit(bool b) {
	IsHit = true;
	m_hitCycle = m_cycles - (b ? 1 : 0);
}

bool SoundInfo::IsCurrentHit() {
	return IsHit && m_hitCycle == m_cycles;
}

bool SoundInfo::IsPrevHit() {
	return IsHit && m_cycles > m_hitCycle;
}

bool SoundInfo::IsPlaying() {
	for (int i = 0; i < 4; i++) {
		int count = m_level->TowerCount[i];
		if (count < TowerRequirement[i][0] || count > TowerRequirement[i][1]) {
			return false;
		}
	}
	return true;
}
