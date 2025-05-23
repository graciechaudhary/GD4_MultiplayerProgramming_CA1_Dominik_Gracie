//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "State.hpp"
#include "StateID.hpp"
#include "StateStack.hpp"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, PlayersController& player, MusicPlayer& music, SoundPlayer& sounds) : window(&window), textures(&textures), fonts(&fonts), players_controller(&player), music(&music), sounds(&sounds)
{
}

State::State(StateStack& stack, Context context) : m_stack(&stack), m_context(context)
{
}

State::~State()
{
}

void State::RequestStackPush(StateID state_id)
{
    m_stack->PushState(state_id);
}

void State::RequestStackPop()
{
    m_stack->PopState();
}

void State::RequestStackClear()
{
    m_stack->ClearStack();
}

State::Context State::GetContext() const
{
    return m_context;
}
