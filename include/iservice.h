//                                    .-""-.
//                                   / _  _ \
//                                   |(_)(_)|
// .------------------------------ooO(_ /\ _)Ooo-----------------------------.
// |                                  |====|                                 |
// |                                  '-..-'                                 |
// | Desc:     Service interface                                             |
// | By:       Nuroferatu                                                    |
// '-------------------------------------------------------------------------'
// ----= Change log =---------------------------------------------------------
//   1. 2017.11.05, 15:00    [+] Initial
// ---------------------------------------------------------------------------
#pragma once
#ifndef __ISRVICE_H__
#define __ISRVICE_H__

#include <memory>

// ---------------------------------------------------------------------------
// IService
// ---------------------------------------------------------------------------
class IService {
public:
    typedef std::unique_ptr<IService>   Ref;
    static const unsigned int   INTERFACE_VERSION = 0x01;

    virtual ~IService() = default;

    virtual const char* getName( void ) const = 0;
    virtual const char* getVersion( void ) const = 0;

    virtual void onInit( void ) = 0;
    virtual void onShutdown( void ) = 0;
};

#endif /* ndef __ISRVICE_H__ */
/* EOF */