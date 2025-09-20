#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <random>
#include <atomic>



struct Player{

    int health;
    float radius;
    sf::Vector2f pos;
    sf::Color color;
    std::chrono::steady_clock::time_point lastUpdate;

    unsigned int id;

};



void gen_player_param(Player& player){

    std::srand(time(0));

    player.health = 100;
    player.pos.x = std::rand() % 600;
    player.pos.y = std::rand() % 400;
    player.radius = 10 + std::rand() % 10;

    player.color.r = std::rand() % 255;
    player.color.g = std::rand() % 255;
    player.color.b = std::rand() % 255;

    player.id = std::rand();

}


void control(sf::Vector2f& pos){

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
        pos.y -= 5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
        pos.y += 5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        pos.x -= 5;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
        pos.x += 5;
    }

}


int main(){

    unsigned short port = 5341; 
    sf::IpAddress ip = "127.0.0.1";
    sf::UdpSocket socket;

    if(socket.bind(sf::Socket::AnyPort) != sf::Socket::Done){
        std::cerr << "bind error!\n";
        return -1;
    }


    std::map<unsigned int, Player> players; 
    std::map<unsigned int, sf::CircleShape> players_to_render;

    std::mutex mx;

    std::atomic<bool> run = true;


    //поток принимает данные
    std::thread receiver_thread(
        [&socket, &players_to_render, &players, &mx, &run]{
            socket.setBlocking(false);

            while(run){
                
                sf::Packet packet;
                unsigned short port;
                sf::IpAddress ip_address;

                sf::Socket::Status status = socket.receive(packet, ip_address, port);
                if(status == sf::Socket::Done){

                    Player player_data;

                    packet >> player_data.health >> player_data.radius >> player_data.pos.x >> player_data.pos.y 
                    >> player_data.color.r >> player_data.color.g >> player_data.color.b >> player_data.id;

                    sf::CircleShape cs;
                    cs.setFillColor(player_data.color);
                    cs.setPosition(player_data.pos);
                    cs.setRadius(player_data.radius);

                    std::lock_guard<std::mutex> lc{mx};
                    players_to_render[player_data.id] = std::move(cs);
                    players[player_data.id] = std::move(player_data);

                }

            }


        }
    );


    sf::RenderWindow window(sf::VideoMode(640, 480), "CLIENT");

    Player player;
    gen_player_param(player);
    sf::CircleShape circle;
    circle.setFillColor(player.color);
    circle.setPosition(player.pos);
    circle.setRadius(player.radius);


    while(window.isOpen()){

        sf::Event event;
        while(window.pollEvent(event)){

            if(event.type == sf::Event::Closed){
                window.close();
            }

        }


        sf::Packet packet;
        packet << player.health << player.radius << player.pos.x << player.pos.y 
                    << player.color.r << player.color.g << player.color.b << player.id;

        sf::Socket::Status status = socket.send(packet, ip, port);

        if(window.hasFocus()){
            control(player.pos);
        }
        circle.setPosition(player.pos);

        window.clear();

        window.draw(circle);
        {
            std::lock_guard<std::mutex> lk{mx};
            for(auto &it: players_to_render){
                if(it.first != player.id)
                    window.draw(it.second);
            }
        }
        
        window.display();
        sf::sleep(sf::milliseconds(10));    
    }   

    run = false;
    receiver_thread.join();


    return 0;
}