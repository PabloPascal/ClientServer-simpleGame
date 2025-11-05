#include "protocol.hpp"

#include <SFML/Network.hpp>

sf::Packet& operator << (sf::Packet& packet, const PlayerState& state)
{
    return packet 
    << (uint8_t)state.type
    << state.id 
    << state.name
    << state.positionX
    << state.positionY
    << state.speed
    << static_cast<uint8_t>(state.action)
    << state.lastTimeUpdate
    << state.ammoCount
    << state.health
    << state.dirX
    << state.dirY;
}

sf::Packet& operator >> (sf::Packet& packet, PlayerState& state)
{
    
    packet >> state.id >> state.name >> state.positionX >> state.positionY >> state.speed;

    uint8_t action;
    packet >> action;
    state.action = static_cast<PlayerAction>(action);

    return packet   
    >> state.lastTimeUpdate
    >> state.ammoCount
    >> state.health
    >> state.dirX
    >> state.dirY;
}

sf::Packet& operator << (sf::Packet& packet, const ProjectileState& state)
{
    return packet 
    << static_cast<uint8_t>(state.type) 
    << state.id 
    << state.owner_id 
    << state.live
    << state.positionX
    << state.positionY
    << state.velocityX
    << state.velocityY
    << state.lastTimeUpdate;
}

sf::Packet& operator >> (sf::Packet& packet, ProjectileState& state)
{
    return packet 
    >> state.id
    >> state.owner_id 
    >> state.live
    >> state.positionX
    >> state.positionY
    >> state.velocityX
    >> state.velocityY
    >> state.lastTimeUpdate;
}
