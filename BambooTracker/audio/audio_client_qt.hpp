#pragma once
#include "audio_client.hpp"

class AudioClient_Qt final : public AudioClient
{
public:
	explicit AudioClient_Qt(QObject *parent = nullptr);
	~AudioClient_Qt();

	void initialize(uint32_t rate, uint32_t duration, uint32_t intrRate, const QString &device) override;
	void shutdown() override;

private:
	class AudioThread;
	std::unique_ptr<AudioThread> thread_;
};
