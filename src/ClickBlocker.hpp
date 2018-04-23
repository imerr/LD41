//
// Created by iMer on 23.04.2018.
//

#ifndef LD41_CLICKBLOCKER_HPP
#define LD41_CLICKBLOCKER_HPP


#include <Engine/SpriteNode.hpp>

class ClickBlocker: public engine::SpriteNode {
	std::unique_ptr<engine::BaseEventHandler> m_clickBlocker;
public:
	ClickBlocker(engine::Scene* scene);
	~ClickBlocker();


};


#endif //LD41_CLICKBLOCKER_HPP
