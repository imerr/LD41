//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_BEATDISPLAY_HPP
#define LD41_BEATDISPLAY_HPP

#include <unordered_map>
#include <Engine/Node.hpp>
#include <Engine/SpriteNode.hpp>
#include "Level.hpp"

class BeatDisplay : public engine::Node {
public:
	static constexpr float DisplayTime = 4;
protected:
	std::unordered_map<BeatType, engine::Tween<sf::Color>*> m_colorTweens;
	std::unordered_map<BeatType, Json::Value> m_beats;
	std::unordered_map<BeatType, std::vector<engine::Node*>> m_bars;
public:
	BeatDisplay(engine::Scene* scene);
	virtual ~BeatDisplay();

	virtual void SetActive(bool active);

	virtual bool initialize(Json::Value& root);

protected:
	virtual void OnUpdate(sf::Time interval);

	void AddBar(BeatType type);
	virtual void OnDraw(sf::RenderTarget& target, sf::RenderStates states, float delta);

};


#endif //LD41_BEATDISPLAY_HPP
