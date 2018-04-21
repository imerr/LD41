//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_LEVEL_HPP
#define LD41_LEVEL_HPP


#include <Engine/Scene.hpp>
#include <SFML/Audio/Sound.hpp>
#include <Engine/SpriteNode.hpp>

class SoundInfo {
	float m_beatTime;
	float m_playIn;
	float m_prevPlayTime;
public:
	enum class BeatType {
		None,
		Beat,
		Max
	};
	SoundInfo();
	void Update(float interval);

	inline float LastPlayTime() {
		return -m_beatTime;
	}
	inline float NextPlayTime(int future = 0) {
		// TODO: this won't work with any randomness
		return m_playIn * (future + 1) - m_beatTime;
	}
	BeatType Type;
	float Interval;
	float Volume;
	float Offset;
	float Deviation;
	std::unique_ptr<sf::Sound> Sound;
	std::vector<float> Pitches;
	bool Played;

	void Reset();
};

class Level : public engine::Scene {
protected:
	std::vector<SoundInfo> m_sounds;
	int m_bpm;
	engine::SpriteNode* m_beatIndicator;
	engine::SpriteNode* m_inputIndicator;
	engine::EventHandler<bool, const sf::Event::KeyEvent&, bool>* m_keyHandler;
	bool m_pressedInput;
public:
	explicit Level(engine::Game* game);
	virtual ~Level();

	virtual void OnInitializeDone();
	int GetBPM() {
		return m_bpm;
	}

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual bool initialize(Json::Value& root);


	std::vector<SoundInfo>& GetSounds();
};


#endif //LD41_LEVEL_HPP
