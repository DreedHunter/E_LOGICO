// SPDX-License-Identifier: AGPL-3.0-or-later
#include "WasapiDevice.h"

#include "elogico/platform/ThreadPriority.h"

#include <windows.h>
#include <initguid.h>
#include <audioclient.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>
#include <mmreg.h>

#include <atomic>
#include <cstdio>
#include <thread>
#include <vector>

namespace elogico
{

namespace
{

template <typename T>
class ComPtr
{
public:
    ComPtr() = default;
    ~ComPtr() { reset(); }
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    T** put() noexcept { reset(); return &ptr_; }
    T* get() const noexcept { return ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }
    void reset() noexcept
    {
        if (ptr_ != nullptr)
            ptr_->Release();
        ptr_ = nullptr;
    }

private:
    T* ptr_ = nullptr;
};

/// Inizializza COM per lo scope corrente (multi-thread apartment).
struct ComScope
{
    ComScope() : hr(CoInitializeEx(nullptr, COINIT_MULTITHREADED)) {}
    ~ComScope()
    {
        if (SUCCEEDED(hr))
            CoUninitialize();
    }
    HRESULT hr;
};

std::string toUtf8(const wchar_t* text)
{
    if (text == nullptr)
        return {};
    const int size = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
    std::string out(std::size_t(size > 0 ? size - 1 : 0), '\0');
    if (size > 1)
        WideCharToMultiByte(CP_UTF8, 0, text, -1, out.data(), size, nullptr, nullptr);
    return out;
}

std::wstring toWide(const std::string& text)
{
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    std::wstring out(std::size_t(size > 0 ? size - 1 : 0), L'\0');
    if (size > 1)
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, out.data(), size);
    return out;
}

std::string hrMessage(const char* what, HRESULT hr)
{
    char buffer[128];
    std::snprintf(buffer, sizeof(buffer), "%s (HRESULT 0x%08lX)", what, static_cast<unsigned long>(hr));
    return buffer;
}

bool isFloat32(const WAVEFORMATEX* format)
{
    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
        return format->wBitsPerSample == 32;
    if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        const auto* ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(format);
        return ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT && format->wBitsPerSample == 32;
    }
    return false;
}

class WasapiDevice final : public AudioDevice
{
public:
    ~WasapiDevice() override
    {
        stop();
        if (event_ != nullptr)
            CloseHandle(event_);
        if (format_ != nullptr)
            CoTaskMemFree(format_);
    }

    bool open(const AudioDeviceConfig& config, std::string& error)
    {
        if (config.exclusive)
        {
            error = "WASAPI esclusivo non ancora implementato";
            return false;
        }

        ComPtr<IMMDeviceEnumerator> enumerator;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(enumerator.put()));
        if (FAILED(hr))
            return error = hrMessage("MMDeviceEnumerator non disponibile", hr), false;

        if (config.deviceId.empty())
            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, device_.put());
        else
            hr = enumerator->GetDevice(toWide(config.deviceId).c_str(), device_.put());
        if (FAILED(hr))
            return error = hrMessage("device di uscita non trovato", hr), false;

        hr = device_->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                               reinterpret_cast<void**>(client_.put()));
        if (FAILED(hr))
            return error = hrMessage("IAudioClient non attivabile", hr), false;

        hr = client_->GetMixFormat(&format_);
        if (FAILED(hr))
            return error = hrMessage("formato del mixer non leggibile", hr), false;
        if (!isFloat32(format_))
            return error = "il mixer di sistema non usa float 32 bit: formato non ancora supportato", false;

        // Il periodo è scelto dal mixer di sistema in modalità condivisa: chiediamo il minimo.
        hr = client_->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, format_,
                                 nullptr);
        if (FAILED(hr))
            return error = hrMessage("inizializzazione WASAPI fallita", hr), false;

        event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        client_->SetEventHandle(event_);
        client_->GetBufferSize(&bufferFrames_);
        hr = client_->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(render_.put()));
        if (FAILED(hr))
            return error = hrMessage("IAudioRenderClient non disponibile", hr), false;

        REFERENCE_TIME latency = 0;
        client_->GetStreamLatency(&latency);
        outputLatency_ = int(double(latency) * 1e-7 * format_->nSamplesPerSec) + int(bufferFrames_);

        numOutputs_ = format_->nChannels;
        planar_.assign(std::size_t(numOutputs_), std::vector<float>(bufferFrames_, 0.0f));
        for (auto& ch : planar_)
            pointers_.push_back(ch.data());
        return true;
    }

    bool start(AudioCallback& callback) override
    {
        if (running_)
            return true;
        callback_ = &callback;
        callback.audioDeviceAboutToStart(sampleRate(), bufferSize(), 0, numOutputs_);
        running_ = true;
        thread_ = std::thread([this] { run(); });
        return true;
    }

    void stop() override
    {
        if (!running_)
            return;
        running_ = false;
        SetEvent(event_);
        if (thread_.joinable())
            thread_.join();
        if (callback_ != nullptr)
            callback_->audioDeviceStopped();
    }

    bool isRunning() const override { return running_; }
    double sampleRate() const override { return format_ != nullptr ? double(format_->nSamplesPerSec) : 0.0; }
    int bufferSize() const override { return int(bufferFrames_); }
    int numInputs() const override { return 0; }
    int numOutputs() const override { return numOutputs_; }
    int inputLatency() const override { return 0; }
    int outputLatency() const override { return outputLatency_; }
    std::string lastError() const override { return lastError_; }

private:
    void run()
    {
        ComScope com;
        ScopedProAudioThread priority;

        client_->Start();
        while (running_)
        {
            if (WaitForSingleObject(event_, 2000) != WAIT_OBJECT_0)
                continue;
            if (!running_)
                break;

            UINT32 padding = 0;
            if (FAILED(client_->GetCurrentPadding(&padding)))
            {
                lastError_ = "device perso"; // TODO(MVP): riapertura automatica
                break;
            }
            const UINT32 frames = bufferFrames_ - padding;
            if (frames == 0)
                continue;

            BYTE* data = nullptr;
            if (FAILED(render_->GetBuffer(frames, &data)))
                continue;

            callback_->audioCallback(nullptr, 0, pointers_.data(), numOutputs_, int(frames));

            auto* out = reinterpret_cast<float*>(data);
            for (UINT32 i = 0; i < frames; ++i)
                for (int c = 0; c < numOutputs_; ++c)
                    *out++ = planar_[std::size_t(c)][i];

            render_->ReleaseBuffer(frames, 0);
        }
        client_->Stop();
    }

    ComPtr<IMMDevice> device_;
    ComPtr<IAudioClient> client_;
    ComPtr<IAudioRenderClient> render_;
    WAVEFORMATEX* format_ = nullptr;
    HANDLE event_ = nullptr;
    UINT32 bufferFrames_ = 0;
    int numOutputs_ = 0;
    int outputLatency_ = 0;
    std::vector<std::vector<float>> planar_;
    std::vector<float*> pointers_;
    AudioCallback* callback_ = nullptr;
    std::atomic<bool> running_{false};
    std::thread thread_;
    std::string lastError_;
};

} // namespace

std::vector<AudioDeviceInfo> WasapiBackend::listDevices()
{
    ComScope com;
    std::vector<AudioDeviceInfo> result;

    ComPtr<IMMDeviceEnumerator> enumerator;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                reinterpret_cast<void**>(enumerator.put()))))
        return result;

    std::wstring defaultId;
    {
        ComPtr<IMMDevice> def;
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, def.put())))
        {
            LPWSTR id = nullptr;
            if (SUCCEEDED(def->GetId(&id)))
            {
                defaultId = id;
                CoTaskMemFree(id);
            }
        }
    }

    ComPtr<IMMDeviceCollection> collection;
    if (FAILED(enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, collection.put())))
        return result;

    UINT count = 0;
    collection->GetCount(&count);
    for (UINT i = 0; i < count; ++i)
    {
        ComPtr<IMMDevice> device;
        if (FAILED(collection->Item(i, device.put())))
            continue;

        AudioDeviceInfo info;
        LPWSTR id = nullptr;
        if (SUCCEEDED(device->GetId(&id)))
        {
            info.id = toUtf8(id);
            info.isDefault = defaultId == id;
            CoTaskMemFree(id);
        }

        ComPtr<IPropertyStore> props;
        if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, props.put())))
        {
            PROPVARIANT name;
            PropVariantInit(&name);
            if (SUCCEEDED(props->GetValue(PKEY_Device_FriendlyName, &name)) && name.vt == VT_LPWSTR)
                info.name = toUtf8(name.pwszVal);
            PropVariantClear(&name);
        }

        ComPtr<IAudioClient> client;
        if (SUCCEEDED(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                       reinterpret_cast<void**>(client.put()))))
        {
            WAVEFORMATEX* format = nullptr;
            if (SUCCEEDED(client->GetMixFormat(&format)))
            {
                info.numOutputs = format->nChannels;
                CoTaskMemFree(format);
            }
        }
        result.push_back(std::move(info));
    }
    return result;
}

std::unique_ptr<AudioDevice> WasapiBackend::open(const AudioDeviceConfig& config, std::string& errorOut)
{
    // COM deve restare inizializzato sul thread che usa i puntatori: il thread audio lo
    // inizializza per sé, qui basta per l'apertura (MTA, gli oggetti sono free-threaded).
    ComScope com;
    auto device = std::make_unique<WasapiDevice>();
    if (!device->open(config, errorOut))
        return nullptr;
    return device;
}

} // namespace elogico
