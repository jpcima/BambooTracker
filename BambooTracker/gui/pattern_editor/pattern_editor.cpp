#include "pattern_editor.hpp"
#include "ui_pattern_editor.h"

PatternEditor::PatternEditor(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::PatternEditor)
{
	ui->setupUi(this);

	ui->panel->setFocus();
	QObject::connect(ui->panel, &PatternEditorPanel::currentCellInRowChanged,
					 ui->horizontalScrollBar, &QScrollBar::setValue);
	QObject::connect(ui->panel, &PatternEditorPanel::currentStepChanged,
					 this, [&](int num, int max) {
		ui->verticalScrollBar->setValue(num);
		ui->verticalScrollBar->setMaximum(max);
	});
	QObject::connect(ui->panel, &PatternEditorPanel::currentTrackChanged,
					 this, [&](int num) { emit currentTrackChanged(num); });
	QObject::connect(ui->panel, &PatternEditorPanel::currentOrderChanged,
					 this, [&](int num, int max) { emit currentOrderChanged(num, max); });

	auto focusSlot = [&]() { ui->panel->setFocus(); };

	QObject::connect(ui->horizontalScrollBar, &QScrollBar::valueChanged,
					 ui->panel, &PatternEditorPanel::setCurrentCellInRow);
	QObject::connect(ui->horizontalScrollBar, &QScrollBar::sliderPressed, this, focusSlot);

	QObject::connect(ui->verticalScrollBar, &QScrollBar::valueChanged,
					 ui->panel, &PatternEditorPanel::setCurrentStep);
	QObject::connect(ui->verticalScrollBar, &QScrollBar::sliderPressed, this, focusSlot);
}

PatternEditor::~PatternEditor()
{
	delete ui;
}

void PatternEditor::setCore(std::shared_ptr<BambooTracker> core)
{
	bt_ = core;
	ui->panel->setCore(core);
}

void PatternEditor::setCommandStack(std::weak_ptr<QUndoStack> stack)
{
	ui->panel->setCommandStack(stack);
}

void PatternEditor::changeEditable()
{
	ui->panel->changeEditable();
}

void PatternEditor::updatePosition()
{
	ui->panel->updatePosition();
}

/********** Slots **********/
void PatternEditor::setCurrentTrack(int num)
{
	ui->panel->setCurrentTrack(num);
}

void PatternEditor::setCurrentOrder(int num)
{
	ui->panel->setCurrentOrder(num);
}

void PatternEditor::onOrderListEdited()
{
	ui->panel->onOrderListEdited();
}

void PatternEditor::onDefaultPatternSizeChanged()
{
	ui->panel->onDefaultPatternSizeChanged();
}

void PatternEditor::onSongLoaded()
{
	ui->panel->onSongLoaded();
	ui->horizontalScrollBar->setMaximum(ui->panel->getFullColmunSize());
	ui->horizontalScrollBar->setValue(0);
	ui->verticalScrollBar->setMaximum(bt_->getPatternSizeFromOrderNumber(
										  bt_->getCurrentSongNumber(),
										  bt_->getCurrentOrderNumber()) - 1);
	ui->verticalScrollBar->setValue(0);
}
