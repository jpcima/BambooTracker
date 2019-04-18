#include "audio_client.hpp"
#include <algorithm>

AudioClient::AudioClient(QObject *parent)
	: QObject(parent),
	  rate_(0),
	  intrRate_(0),
	  intrCount_(0),
	  intrCountRest_(0),
	  cb_(nullptr),
	  cbPtr_(nullptr),
	  started_(false),
	  tickNotifierQuit_(false),
	  tickNotifier_([this]() { tickNotifierRun(); })
{
}

AudioClient::~AudioClient()
{
	tickNotifierQuit_ = true;
	tickNotifierSem_.release();
	tickNotifier_.join();
}

void AudioClient::setCallback(Callback *cb, void *cbPtr)
{
	std::lock_guard<std::mutex> lock(generateMutex_);
	cb_ = cb;
	cbPtr_ = cbPtr;
}

void AudioClient::initialize(uint32_t rate, uint32_t duration, uint32_t intrRate, const QString &device)
{
	Q_UNUSED(duration);
	Q_UNUSED(device);

	started_ = false;
	rate_ = rate;
	setInterruption(intrRate);
}

void AudioClient::setInterruption(uint32_t intrRate)
{
	std::lock_guard<std::mutex> lock(generateMutex_);
	intrRate_ = intrRate;
	updateInterruptCount();
}

void AudioClient::start()
{
	std::lock_guard<std::mutex> lock(generateMutex_);
	started_ = true;
}

void AudioClient::stop()
{
	std::lock_guard<std::mutex> lock(generateMutex_);
	started_ = false;
}

void AudioClient::generate(int16_t *container, uint32_t nSamples)
{
	Callback *cb = nullptr;
	void *cbPtr = nullptr;
	bool started = false;

	std::unique_lock<std::mutex> lock(generateMutex_, std::try_to_lock);
	if (lock.owns_lock()) {
		cb = cb_;
		cbPtr = cbPtr_;
		started = started_;
	}

	if (!cb || !started) {
		std::fill(container, container + 2 * nSamples, 0);
		return;
	}

	int16_t *destPtr = container;

	while (nSamples) {
		if (!intrCountRest_) {	// Interruption
			intrCountRest_ = intrCount_;    // Set counts to next interruption
			generateTick();
		}

		size_t count = std::min(intrCountRest_, nSamples);
		nSamples -= count;
		intrCountRest_ -= count;

		cb(destPtr, count, cbPtr);

		destPtr += (count << 1);	// Move head
	}
}

void AudioClient::generateTick()
{
	tickNotifierSem_.release();
}

void AudioClient::updateInterruptCount()
{
	intrCount_ = rate_ / intrRate_;
}

void AudioClient::tickNotifierRun()
{
	volatile bool *quit = &tickNotifierQuit_;

	for (;;) {
		tickNotifierSem_.acquire();

		if (*quit)
			return;

		emit streamInterrupted();
	};
}
