#ifndef BUFFER_H_
#define BUFFER_H_

#include <queue>
#include <memory>
#include "User.h"


class Buffer
{

public:
	enum class ChannelState
	{
		BUSY,
		FREE,
		NOT_CHECKED
	};

    size_t get_buffer_size(){ return buffer_.size(); }
    std::queue<std::shared_ptr<User>> get_buffer(){ return buffer_; }
    void add_user_to_buffer(std::shared_ptr<User> user){ buffer_.push(user); }


private:
	std::queue<std::shared_ptr<User>> buffer_;
	ChannelState channel_state_;
};


#endif // !BUFFER_H_



