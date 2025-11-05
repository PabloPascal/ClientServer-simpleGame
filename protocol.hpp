#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP


#include <cstdint>
#include <string>

using ID = unsigned int;
using PORT = unsigned int;


enum class MessageType : uint8_t
{
    player_action,
    player_state,
    projectile_state,
    text
};


enum class PlayerAction : uint8_t 
{
    noMove      = 0,
    moveUp      = 1 << 0,
    moveDown    = 1 << 1,
    moveLeft    = 1 << 2,
    moveRight   = 1 << 3,
    shoot       = 1 << 4,
};



inline PlayerAction operator |(PlayerAction act1, PlayerAction act2) 
{
    return static_cast<PlayerAction>(static_cast<uint8_t>(act1) | static_cast<uint8_t>(act2));
}
inline PlayerAction operator &(PlayerAction act1, PlayerAction act2) 
{
    return static_cast<PlayerAction>(static_cast<uint8_t>(act1) & static_cast<uint8_t>(act2));
}
inline PlayerAction& operator |=(PlayerAction& act1, PlayerAction act2) 
{
    act1 = act1 | act2;
    return act1;
}
inline PlayerAction& operator &=(PlayerAction& act1, PlayerAction act2) 
{
    act1 = act1 & act2;
    return act1;
}





struct PlayerState
{
    MessageType type = MessageType::player_state;
    ID id;
    std::string name;
    float positionX;
    float positionY;
    float speed;
    PlayerAction action;
    float lastTimeUpdate;
    uint8_t ammoCount;
    uint8_t health;
    float dirX;
    float dirY;
};


struct ProjectileState
{
    MessageType type = MessageType::projectile_state;
    ID id;
    ID owner_id;
    bool live = false;
    float positionX;
    float positionY;
    float velocityX = 0.1f;
    float velocityY = 0.1f;
    float lastTimeUpdate;
};



namespace sf
{
    class Packet;
}

sf::Packet& operator << (sf::Packet& packet, const PlayerState& state);
sf::Packet& operator >> (sf::Packet& packet, PlayerState& state);
sf::Packet& operator << (sf::Packet& packet, const ProjectileState& state);
sf::Packet& operator >> (sf::Packet& packet, ProjectileState& state);


#endif