//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Component.hpp"

namespace gui
{
	class Container : public Component
	{
	public:
		typedef std::shared_ptr<Container> Ptr;

	public:
		Container(bool is_player_one);
		void Pack(Component::Ptr component);
		virtual bool IsSelectable() const override;
		virtual void HandleEvent(const sf::Event& event) override;

		sf::Keyboard::Key GetUpKey(bool is_player_one) const;
		sf::Keyboard::Key GetDownKey(bool is_player_one) const;
		sf::Keyboard::Key GetReturnKey(bool is_player_one) const;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		bool HasSelection() const;
		void Select(std::size_t index);
		void SelectNext();
		void SelectPrevious();

	private:
		std::vector<Component::Ptr> m_children;
		int m_selected_child;
		bool m_is_player_one;
	};
}

