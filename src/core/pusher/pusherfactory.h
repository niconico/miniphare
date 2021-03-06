#ifndef PUSHERFACTORY_H
#define PUSHERFACTORY_H

#include <memory>
#include <string>

#include "core/pusher/modifiedboris.h"
#include "core/pusher/pusher.h"
#include "data/grid/gridlayout.h"



class PusherFactory
{
public:
    static std::unique_ptr<Pusher> createPusher(const GridLayout& layout,
                                                const std::string& pusherType, double dt)
    {
        std::unique_ptr<Pusher> pusher;


        if (pusherType == "modifiedBoris")
        {
            //    impl = std::unique_ptr<ModifiedBoris>( new ModifiedBoris( pusherType, layout ) ) ;
            pusher = std::unique_ptr<Pusher>(new ModifiedBoris(layout, pusherType, dt));
        }
        else
        {
            throw std::runtime_error("Error : PusherFactory - available pushers are :\
                                     - modifiedBoris ");
        }

        return pusher;
    }
};



#endif // PUSHERFACTORY_H
