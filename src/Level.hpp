//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_LEVEL_HPP
#define LD41_LEVEL_HPP


#include <Engine/Scene.hpp>
#include <SFML/Audio/Sound.hpp>
#include <Engine/SpriteNode.hpp>

class Level : public engine::Scene {
protected:
	class SoundInfo {
		float m_beatTime;
		float m_playIn;
		float m_prevPlayTime;
	public:
		enum class SoundType {
			None = 0,
			Beat = 1,
			Other = 2
		};
		SoundInfo();
		void Update(float interval);

		inline float LastPlayTime() {
			return -m_beatTime;
		}
		inline float NextPlayTime() {
			// TODO: add more than 1 in future
			return m_playIn - m_beatTime;
		}
		SoundType Type;
		float Interval;
		float Volume;
		float Offset;
		float Deviation;
		std::unique_ptr<sf::Sound> Sound;
		std::vector<float> Pitches;
		bool Played;

		void Reset();

	};
	std::vector<SoundInfo> m_sounds;
	int m_bpm;
	engine::SpriteNode* m_beatIndicator;
	engine::SpriteNode* m_inputIndicator;
	engine::EventHandler<bool, const sf::Event::KeyEvent&, bool>* m_keyHandler;
	bool m_pressedInput;
public:
	explicit Level(engine::Game* game);
	virtual ~Level();

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual bool initialize(Json::Value& root);


};


#endif //LD41_LEVEL_HPP
