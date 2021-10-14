#pragma once

#include <Windows.h>

class RegistryService
{

public:
    static RegistryService          &get();

    int                             getMouseSensivity() const;
    int                             getMouseSpeed() const;

private:
    explicit RegistryService() = default;
    RegistryService(const RegistryService &) = delete;
    void operator =(const RegistryService &) = delete;
    virtual ~RegistryService() = default;
};
