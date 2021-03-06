#include "increase_note_key_in_pattern_command.hpp"

IncreaseNoteKeyInPatternCommand::IncreaseNoteKeyInPatternCommand(std::weak_ptr<Module> mod,
																 int songNum, int beginTrack,
																 int beginOrder, int beginStep,
																 int endTrack, int endStep)
	: mod_(mod),
	  song_(songNum),
	  bTrack_(beginTrack),
	  order_(beginOrder),
	  bStep_(beginStep),
	  eTrack_(endTrack),
	  eStep_(endStep)
{
	auto& sng = mod.lock()->getSong(songNum);

	for (int step = beginStep; step <= endStep; ++step) {
		for (int track = beginTrack; track <= endTrack; ++track) {
			int n = sng.getTrack(track).getPatternFromOrderNumber(beginOrder)
					.getStep(step).getNoteNumber();
			if (n > -1) prevKeys_.push_back(n);
		}
	}
}

void IncreaseNoteKeyInPatternCommand::redo()
{
	auto& sng = mod_.lock()->getSong(song_);

	for (int step = bStep_; step <= eStep_; ++step) {
		for (int track = bTrack_; track <= eTrack_; ++track) {
			auto& s = sng.getTrack(track).getPatternFromOrderNumber(order_).getStep(step);
			int n = s.getNoteNumber();
			if (n > -1) {
				n = (n == 95)? 95 : (n + 1);
				s.setNoteNumber(n);
			}
		}
	}
}

void IncreaseNoteKeyInPatternCommand::undo()
{
	auto& sng = mod_.lock()->getSong(song_);

	size_t i = 0;
	for (int step = bStep_; step <= eStep_; ++step) {
		for (int track = bTrack_; track <= eTrack_; ++track) {
			auto& s = sng.getTrack(track).getPatternFromOrderNumber(order_).getStep(step);
			 if (s.getNoteNumber() > -1) s.setNoteNumber(prevKeys_.at(i++));
		}
	}
}

int IncreaseNoteKeyInPatternCommand::getID() const
{
	return 0x30;
}
