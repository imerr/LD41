//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_LEVEL_HPP
#define LD41_LEVEL_HPP


#include <Engine/Scene.hpp>
#include <SFML/Audio/Sound.hpp>
#include <Engine/SpriteNode.hpp>
#include "SoundInfo.hpp"

class Level : public engine::Scene {
protected:
	std::vector<SoundInfo> m_sounds;
	int m_bpm;
	engine::SpriteNode* m_beatIndicator;
	engine::SpriteNode* m_inputIndicator;
	engine::EventHandler<bool, const sf::Event::KeyEvent&, bool>* m_keyHandler;
	bool m_pressedInput;
	std::vector<sf::Vector2f> m_path;
public:
	explicit Level(engine::Game* game);
	virtual ~Level();

	virtual void OnInitializeDone();
	int GetBPM() {
		return m_bpm;
	}

	const std::vector<sf::Vector2f>& GetPath() {
		return m_path;
	}

protected:
	virtual void OnUpdate(sf::Time interval);

public:
	virtual bool initialize(Json::Value& root);


	std::vector<SoundInfo>& GetSounds();

	static constexpr float HitLimit = 0.5f;
};


#endif //LD41_LEVEL_HPP
