#include "order_list_editor.hpp"
#include "ui_order_list_editor.h"

OrderListEditor::OrderListEditor(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::OrderListEditor)
{
	ui->setupUi(this);

	QObject::connect(ui->panel, &OrderListPanel::currentTrackChangedForSlider,
					 ui->horizontalScrollBar, &QScrollBar::setValue);
	QObject::connect(ui->panel, &OrderListPanel::currentOrderChangedForSlider,
					 this, [&](int num, int max) {
		ui->verticalScrollBar->setMaximum(max);
		ui->verticalScrollBar->setValue(num);
	});
	QObject::connect(ui->panel, &OrderListPanel::currentTrackChanged,
					 this, [&](int num) { emit currentTrackChanged(num); });
	QObject::connect(ui->panel, &OrderListPanel::currentOrderChanged,
					 this, [&](int num) { emit currentOrderChanged(num); });
	QObject::connect(ui->panel, &OrderListPanel::orderEdited,
					 this, [&]() { emit orderEdited(); });

	auto focusSlot = [&]() { ui->panel->setFocus(); };

	QObject::connect(ui->horizontalScrollBar, &QScrollBar::valueChanged,
					 ui->panel, &OrderListPanel::setCurrentTrackForSlider);
	QObject::connect(ui->horizontalScrollBar, &QScrollBar::sliderPressed, this, focusSlot);

	QObject::connect(ui->verticalScrollBar, &QScrollBar::valueChanged,
					 ui->panel, &OrderListPanel::setCurrentOrderForSlider);
	QObject::connect(ui->verticalScrollBar, &QScrollBar::sliderPressed, this, focusSlot);
}

OrderListEditor::~OrderListEditor()
{
	delete ui;
}

void OrderListEditor::setCore(std::shared_ptr<BambooTracker> core)
{
	bt_ = core;
	ui->panel->setCore(core);
}

void OrderListEditor::setCommandStack(std::weak_ptr<QUndoStack> stack)
{
	ui->panel->setCommandStack(stack);
}

void OrderListEditor::changeEditable()
{
	ui->panel->changeEditable();
}

/********** Slots **********/
void OrderListEditor::setCurrentTrack(int num)
{
	ui->panel->setCurrentTrack(num);
}

void OrderListEditor::setCurrentOrder(int num, int max)
{
	ui->panel->setCurrentOrder(num);
	ui->verticalScrollBar->setMaximum(max);
	ui->verticalScrollBar->setValue(num);
}

void OrderListEditor::onSongLoaded()
{
	ui->panel->onSongLoaded();
	int song = bt_->getCurrentSongNumber();
	ui->horizontalScrollBar->setMaximum(bt_->getSongStyle(song).trackAttribs.size() - 1);
	ui->verticalScrollBar->setMaximum(bt_->getOrderSize(song) - 1);
}
