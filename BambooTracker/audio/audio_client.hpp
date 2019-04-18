#pragma once
#include <QObject>
#include <QSemaphore>
#include <QString>
#include <thread>
#include <mutex>
#include <memory>
#include <cstdint>

class AudioClient : public QObject
{
	Q_OBJECT

public:
	explicit AudioClient(QObject *parent = nullptr);
	virtual ~AudioClient();

	typedef void (Callback)(int16_t *, size_t, void *);
	void setCallback(Callback *cb, void *cbPtr);

	virtual void initialize(uint32_t rate, uint32_t duration, uint32_t intrRate, const QString &device);
	virtual void shutdown() = 0;

	virtual std::vector<std::string> getAvailableDevices() = 0;

	void setInterruption(uint32_t intrRate);

	void start();
	void stop();

signals:
	void streamInterrupted();

protected:
	void generate(int16_t *container, uint32_t nSamples);

private:
	void generateTick();
	void updateInterruptCount();

private:
	void tickNotifierRun();

private:
	uint32_t rate_;
	uint32_t intrRate_;
	uint32_t intrCount_;
	uint32_t intrCountRest_;

	std::mutex generateMutex_;
	Callback *cb_;
	void *cbPtr_;
	bool started_;

	bool tickNotifierQuit_;
	QSemaphore tickNotifierSem_;
	std::thread tickNotifier_;
};
