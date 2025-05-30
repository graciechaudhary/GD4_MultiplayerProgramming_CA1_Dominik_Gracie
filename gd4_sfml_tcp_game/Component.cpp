//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "Component.hpp"

namespace gui
{

    Component::Component()
        : m_is_selected(false)
        , m_is_active(false)
    {
    }

    Component::~Component()
    {
    }

    bool Component::IsSelected() const
    {
        return m_is_selected;
    }

    void Component::Select()
    {
        m_is_selected = true;
    }

    void Component::Deselect()
    {
        m_is_selected = false;
    }

    bool Component::IsActive() const
    {
        return m_is_active;
    }

    void Component::Activate()
    {
        m_is_active = true;
    }

    void Component::Deactivate()
    {
        m_is_active = false;
    }
}