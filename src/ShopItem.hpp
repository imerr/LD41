//
// Created by iMer on 21.04.2018.
//

#ifndef LD41_SHOPITEM_HPP
#define LD41_SHOPITEM_HPP


#include <Engine/Button.hpp>

class ShopItem: public engine::Button {
protected:
	int m_price;
	std::string m_description;
	std::string m_icon;
	std::string m_script;
public:
	ShopItem(engine::Scene* scene);

	virtual void OnInitializeDone();

	virtual bool initialize(Json::Value& root);

protected:
	virtual void OnUpdate(sf::Time interval);
public:

};


#endif //LD41_SHOPITEM_HPP
