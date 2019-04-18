#include "audio_client_qt.hpp"
#include <QThread>
#include <QSysInfo>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QDebug>
#include <stdexcept>
#include <atomic>

class AudioClient_Qt::AudioThread : public QThread
{
public:
	AudioThread(AudioClient_Qt &client, uint32_t rate, uint32_t duration, const QString &device);
	~AudioThread();
	bool isReady() const;

protected:
	void run() override;

private:
	void setDeviceFromString(const QString &device);

private:
	class IODevice : public QIODevice {
	public:
		explicit IODevice(AudioClient_Qt &client);
		qint64 readData(char *data, qint64 maxlen) override;
		qint64 writeData(const char *, qint64) override { return 0; }
	private:
		AudioClient_Qt &client_;
	};

private:
	AudioClient_Qt &client_;
	std::atomic_bool readyFlag_;
	QAudioDeviceInfo info_;
	QAudioFormat format_;
	uint32_t duration_;
	QString device_;
};

AudioClient_Qt::AudioClient_Qt(QObject *parent)
	: AudioClient(parent),
	  thread_(nullptr)
{
}

AudioClient_Qt::~AudioClient_Qt()
{
	shutdown();
}

void AudioClient_Qt::initialize(uint32_t rate, uint32_t duration, uint32_t intrRate, const QString &device)
{
	shutdown();

	AudioClient::initialize(rate, duration, intrRate, device);

	thread_.reset(new AudioThread(*this, rate, duration, device));
	thread_->start(QThread::HighPriority);

	while (!thread_->isReady()) {
		if (!thread_->isRunning()) {
			thread_.reset();
			throw std::runtime_error("Error initializing the audio thread.");
		}
		QThread::yieldCurrentThread();
	}
}

void AudioClient_Qt::shutdown()
{
	thread_.reset();
}

AudioClient_Qt::AudioThread::AudioThread(AudioClient_Qt &client, uint32_t rate, uint32_t duration, const QString &device)
	: client_(client),
	  duration_(duration),
	  device_(device)
{
	format_.setByteOrder(QAudioFormat::Endian(QSysInfo::ByteOrder));
	format_.setChannelCount(2); // Stereo
	format_.setCodec("audio/pcm");
	format_.setSampleRate(static_cast<int>(rate));
	format_.setSampleSize(16);   // int16
	format_.setSampleType(QAudioFormat::SignedInt);
}

AudioClient_Qt::AudioThread::~AudioThread()
{
	if (isRunning()) {
		quit();
		wait();
	}
}

bool AudioClient_Qt::AudioThread::isReady() const
{
	return readyFlag_.load();
}

void AudioClient_Qt::AudioThread::run()
{
	setDeviceFromString(device_);

	QAudioOutput audio(info_, format_);
	audio.setBufferSize(format_.sampleRate() * duration_ / 1000);
	IODevice ioDevice(client_);
	ioDevice.open(QIODevice::ReadOnly);
	audio.start(&ioDevice);
	readyFlag_.store(true);

	exec();
}

void AudioClient_Qt::AudioThread::setDeviceFromString(const QString &device)
{
	info_ = QAudioDeviceInfo::defaultOutputDevice();
	if (device == info_.deviceName())
		return;

	for (auto& i : QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
		if (device == i.deviceName()) {
			info_ = i;
			break;
		}
	}
}

AudioClient_Qt::AudioThread::IODevice::IODevice(AudioClient_Qt &client)
	: client_(client)
{
}

qint64 AudioClient_Qt::AudioThread::IODevice::readData(char *data, qint64 maxlen)
{
	size_t frameSize = (2 * sizeof(int16_t));
	size_t nFrames = maxlen / frameSize;
	client_.generate(reinterpret_cast<int16_t *>(data), nFrames);
	return nFrames * frameSize;
}
