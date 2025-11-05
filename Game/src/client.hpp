#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../../protocol.hpp"

#include <iostream>
#include <unordered_map>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <mutex>
#include <memory>


constexpr int width = 1080;
constexpr int height = 720; 

class Client
{
public:
    Client(PORT port, sf::IpAddress ip) : mPort(port), mAddressIP(ip) 
    {
        std::srand(time(0));
        m_window.create(sf::VideoMode(width, height), "multiplayer");
        line.setPrimitiveType(sf::Lines);
        line.resize(2);

        PlayerState player_state;
        
        player_state.id = std::rand() % 100;
        mId = player_state.id;
        player_state.name = "GIGACHAD";
        player_state.action = PlayerAction::noMove;
        player_state.ammoCount = 100;
        player_state.lastTimeUpdate = 0.5;
        player_state.health = 100;
        player_state.positionX = 500 + rand() % 100;
        player_state.positionY = 400 + rand() % 100;
        player_state.speed = 150;
        player_state.dirX = 0;
        player_state.dirY = 0;

        mPlayers.insert(std::make_pair(player_state.id, player_state));

        std::cout << "mID = " << mId << std::endl;

    }

    ~Client()
    {
        mReceiver_thread->join();
    }

    void run();

private:
    void receive();
    void handlePlayerState(sf::Packet& packet);
    void handleProjectileState(sf::Packet& packet);
    void handleTextMessage(sf::Packet& packet);
    void send();
    void inputProcess(PlayerState& state);
    void playerUpdate(float dt);
private:

    PORT                                    mPort;
    sf::IpAddress                           mAddressIP;
    ID                                      mId;

    std::unordered_map<ID, PlayerState>     mPlayers;
    std::unordered_map<ID, sf::CircleShape> mSprites; 
    std::unordered_map<ID, ProjectileState> mAllProjectiles;
    sf::UdpSocket                           mSocket;

    std::mutex mx;
    std::mutex projectile_mutex;
    std::unique_ptr<std::thread> mReceiver_thread;



    sf::RenderWindow m_window;
    sf::VertexArray line;

    bool connected;
    float dt;
};



#endif 