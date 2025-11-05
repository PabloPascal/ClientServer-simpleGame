#include "client.hpp"
#include <cmath>



void Client::run()
{
    //mSocket.bind(mPort);
    mSocket.bind(sf::Socket::AnyPort);

    mReceiver_thread = std::make_unique<std::thread>(&Client::receive, this);


    sf::Font font;
    font.loadFromFile("../res/MomoTrustDisplay-Regular.ttf");

    sf::Clock timer;
    while(connected)
    {
        dt = timer.restart().asSeconds();
        

        sf::Event event;
        while(m_window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                m_window.close();
                connected = false;
            }
        }

        playerUpdate(dt);

        send();

        m_window.clear();

        for(const auto& [id, state] : mPlayers)
        {
            std::lock_guard<std::mutex> lk{mx};
            sf::CircleShape circle(40);
            circle.setOrigin(40,40);
            circle.setFillColor(sf::Color::Red);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(5);
            circle.setPosition(state.positionX, state.positionY);

            line[0] = sf::Vector2f(state.positionX, state.positionY);
            line[1] = sf::Vector2f(state.positionX, state.positionY) + sf::Vector2f(100 * state.dirX, 100 * state.dirY);

            sf::Text text;
            text.setFont(font);
            text.setPosition(state.positionX-20, state.positionY - 80);
            text.setString(std::to_string(state.health));
            text.setFillColor(sf::Color::Green);
            text.setCharacterSize(24);

            m_window.draw(circle);
            m_window.draw(line);
            m_window.draw(text);
        }

        for(const auto& [id, state] : mAllProjectiles)
        {
            if(state.live){
                std::lock_guard<std::mutex> lk{projectile_mutex};
                sf::CircleShape circle(5);
                circle.setFillColor(sf::Color::White);
                circle.setPosition(state.positionX, state.positionY);

                m_window.draw(circle);
            }
        }


        sf::sleep(sf::milliseconds(10.f));
        m_window.display();
    }

}



void Client::receive()
{
    mSocket.setBlocking(false);
    while(connected)
    {
        unsigned short sender_port;
        sf::IpAddress sender_ip;
        sf::Packet packet;

        sf::Socket::Status status = mSocket.receive(packet, sender_ip, sender_port);
        if(status == sf::Socket::Done)
        {
            uint8_t type;
            packet >> type;
            MessageType msg_type = static_cast<MessageType>(type);

            switch (msg_type)
            {
            case MessageType::player_state:
                handlePlayerState(packet);
                break;
            case MessageType::projectile_state:
                handleProjectileState(packet);
                break;
            case MessageType::text:
                handleTextMessage(packet);
            default:
                break;
            }

        }
    }
}




void Client::handlePlayerState(sf::Packet& packet)
{
    PlayerState state;
    packet >> state;


    auto it = mPlayers.find(state.id);
    std::lock_guard<std::mutex> lx(mx);

    if(it == mPlayers.end())
    {
        std::cout << "new client connected!\n";
        mPlayers.insert(std::make_pair(state.id, state));
    }
    else{
        mPlayers[state.id] = state;
    }


}



void Client::handleProjectileState(sf::Packet& packet)
{
    ProjectileState state;
    packet >> state;
    std::lock_guard<std::mutex> lx(projectile_mutex);

    auto it = mAllProjectiles.find(state.id);
    if(it == mAllProjectiles.end())
    {
        mAllProjectiles.insert(std::make_pair(state.id, state));
    }
    else{
        mAllProjectiles[state.id] = state;
    }

}


void Client::send()
{

    if(m_window.hasFocus()){
        inputProcess(mPlayers[mId]);
    }

    
    sf::Packet packet;
    packet << mPlayers[mId];

    mSocket.send(packet, mAddressIP, mPort);

}



void Client::inputProcess(PlayerState& state)
{
    state.action = PlayerAction::noMove;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        state.action |= PlayerAction::moveUp;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        state.action |= PlayerAction::moveDown;

    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        state.action |= PlayerAction::moveRight;

    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        state.action |= PlayerAction::moveLeft;

    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        state.action |= PlayerAction::shoot;
    }

    state.lastTimeUpdate = dt;

}


void Client::handleTextMessage(sf::Packet& packet)
{
    std::string text;
    packet >> text;
    std::cout << text;
}




void Client::playerUpdate(float dt)
{   
    float myPosX = mPlayers[mId].positionX;
    float myPosY = mPlayers[mId].positionY;
    

    float angle = std::atan2(sf::Mouse::getPosition(m_window).y - myPosY, sf::Mouse::getPosition(m_window).x - myPosX);

    mPlayers[mId].dirX = cos(angle);
    mPlayers[mId].dirY = sin(angle);

    

}