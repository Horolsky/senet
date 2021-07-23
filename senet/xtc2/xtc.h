#include "lib/bitf.hpp"
#include <stdint.h>

template class bitf::data<uint64_t>;
template class bitf::constructor<uint64_t>;
template class bitf::accessor<uint64_t>;

template class bitf::data<uint32_t>;
template class bitf::constructor<uint32_t>;
template class bitf::accessor<uint32_t>;


//bitfield wrapper
class Board :
public virtual bitf::data<uint64_t>,
public virtual bitf::constructor<uint64_t>, 
public virtual bitf::accessor<uint64_t>
{
    public:
    using constructor<uint64_t>::constructor;
};
//bitfield wrapper
class Moves :
public virtual bitf::data<uint64_t>,
public virtual bitf::constructor<uint64_t>, 
public virtual bitf::accessor<uint64_t>
{
    public:
    using constructor<uint64_t>::constructor;
};

/**
 * senet ply
 * 
 */
class Ply
{
    Board _board;
    Moves _moves;
    double _utility;
    
};