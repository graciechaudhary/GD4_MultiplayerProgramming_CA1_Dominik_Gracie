//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Component.hpp"
#include "ResourceIdentifiers.hpp"

namespace gui
{
	class Label : public Component
	{
	public:
		typedef std::shared_ptr<Label> Ptr;

	public:
		Label(const std::string& text, const FontHolder& fonts);
		virtual bool IsSelectable() const override;
		void SetText(const std::string& text);
		void HandleEvent(const sf::Event& event) override;
		//sf::Text GetText() const;
		void SetOutlineDesign(sf::Color color, float thickness);

		void SetColor(sf::Color color) { m_text.setFillColor(color); }

		void SetSize(int size) { m_text.setCharacterSize(size); }

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		sf::Text m_text;

	};
}

