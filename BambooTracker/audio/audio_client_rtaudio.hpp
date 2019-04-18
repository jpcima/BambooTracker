#pragma once
#include "audio_client.hpp"
#include <memory>

class RtAudio;

class AudioClient_RtAudio final : public AudioClient
{
public:
	explicit AudioClient_RtAudio(QObject *parent = nullptr);
	~AudioClient_RtAudio();

	void initialize(uint32_t rate, uint32_t duration, uint32_t intrRate, const QString &device) override;
	void shutdown() override;

	std::vector<std::string> getAvailableDevices();

private:
	std::unique_ptr<RtAudio> audio_;
};
