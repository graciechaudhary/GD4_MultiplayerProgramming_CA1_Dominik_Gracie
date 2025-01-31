//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#pragma once
#include "Command.hpp"
#include <queue>

class CommandQueue
{
public:
	void Push(const Command& command);
	Command Pop();
	bool IsEmpty() const;

private:
	std::queue<Command> m_queue;
};

