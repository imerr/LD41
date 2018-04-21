//
// Created by iMer on 21.04.2018.
//

#include "SoundInfo.hpp"


SoundInfo::SoundInfo() : Interval(1), Volume(1), Offset(0), Sound(nullptr),
						 m_beatTime(std::numeric_limits<float>::infinity()), Played(false), m_cycles(0), m_hitCycle(0), IsHit(false) {

}

void SoundInfo::Update(float interval) {
	m_beatTime += interval;
	Played = false;
	// TODO: pitch
	if (m_beatTime >= m_playIn) {
		m_prevPlayTime = m_playIn;
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