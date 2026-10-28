// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "elogico/core/AudioCallback.h"

#include <memory>
#include <string>
#include <vector>

namespace elogico
{

struct AudioDeviceInfo
{
    std::string id;   // identificatore stabile del backend (endpoint id WASAPI, CLSID ASIO)
    std::string name; // nome leggibile
    int numInputs = 0;
    int numOutputs = 0;
    bool isDefault = false;
};

struct AudioDeviceConfig
{
    std::string deviceId;    // vuoto = device di default
    double sampleRate = 0.0; // 0 = quello del device
    int bufferSize = 0;      // 0 = quello del device
    bool exclusive = false;  // WASAPI: modalità esclusiva
};

/// Un device aperto. Il backend chiama AudioCallback dal proprio thread ad alta priorità.
class AudioDevice
{
public:
    virtual ~AudioDevice() = default;

    virtual bool start(AudioCallback& callback) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    virtual double sampleRate() const = 0;
    virtual int bufferSize() const = 0;
    virtual int numInputs() const = 0;
    virtual int numOutputs() const = 0;
    /// Latenze riportate dal driver, per allineare le registrazioni ("Recording Delay").
    virtual int inputLatency() const = 0;
    virtual int outputLatency() const = 0;
    virtual std::string lastError() const = 0;
};

/// Un backend di driver (WASAPI, ASIO...).
class AudioBackend
{
public:
    virtual ~AudioBackend() = default;

    virtual std::string name() const = 0;
    virtual std::vector<AudioDeviceInfo> listDevices() = 0;
    /// Ritorna nullptr in caso di errore; errorOut riceve il motivo.
    virtual std::unique_ptr<AudioDevice> open(const AudioDeviceConfig& config, std::string& errorOut) = 0;
};

} // namespace elogico
