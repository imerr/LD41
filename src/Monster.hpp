//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_MONSTER_HPP
#define LD41_MONSTER_HPP


#include <Engine/SpriteNode.hpp>

class Monster : engine::SpriteNode {
protected:
	size_t m_pathSegment;
public:
	Monster(engine::Scene* scene);

private:
	virtual void OnUpdate(sf::Time interval);

	virtual bool initialize(Json::Value& root);

};


#endif //LD41_MONSTER_HPP
