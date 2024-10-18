#ifndef BOT_HPP
#define BOT_HPP

class Bot {
public:

    virtual void initialize() = 0; 
    virtual void run() = 0;
    virtual void stop() = 0;

};

#endif 
