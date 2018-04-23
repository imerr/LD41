//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_MENU_HPP
#define LD41_MENU_HPP


#include <Engine/SpriteNode.hpp>
#include <Engine/Button.hpp>

class Menu: public engine::SpriteNode {
protected:
	bool m_collapsed;
	engine::Node* m_descOwner;
	std::string m_defaultDescription;
	engine::Button* m_playButton;
	std::unique_ptr<engine::BaseEventHandler> m_playHandler;
	std::unique_ptr<engine::BaseEventHandler> m_clickBlocker;
public:
	Menu(engine::Scene* scene);
	virtual ~Menu();
	void ToggleCollapse();
	virtual void OnInitializeDone();

	void ClearDescription(engine::Node* node);

	void SetDescription(engine::Node* owner, std::string desc);
	bool IsCollapsed();
	void SetDefaultDescription(const std::string& desc);

protected:
	virtual void OnUpdate(sf::Time interval);

};


#endif //LD41_MENU_HPP
