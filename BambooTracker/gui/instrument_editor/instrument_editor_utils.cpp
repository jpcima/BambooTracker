/*
 * Copyright (C) 2020 Rerrah
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "instrument_editor_utils.hpp"
#include <unordered_map>
#include <algorithm>
#include <QStringList>
#include "enum_hash.hpp"

namespace inst_edit_utils
{
namespace
{
// TODO: DELETE
const std::unordered_map<VisualizedInstrumentMacroEditor::PermittedReleaseFlag, ReleaseType> REL_TYPE_MAP = {
	{ VisualizedInstrumentMacroEditor::PermittedReleaseFlag::NO_RELEASE, ReleaseType::NoRelease },
	{ VisualizedInstrumentMacroEditor::PermittedReleaseFlag::FIXED_RELEASE, ReleaseType::FixedRelease },
	{ VisualizedInstrumentMacroEditor::PermittedReleaseFlag::ABSOLUTE_RELEASE, ReleaseType::AbsoluteRelease },
	{ VisualizedInstrumentMacroEditor::PermittedReleaseFlag::RELATIVE_RELEASE, ReleaseType::RelativeRelease }
};
}

ReleaseType convertReleaseTypeForData(VisualizedInstrumentMacroEditor::PermittedReleaseFlag type)
{
	return REL_TYPE_MAP.at(type);
}

VisualizedInstrumentMacroEditor::PermittedReleaseFlag convertReleaseTypeForUI(ReleaseType type)
{
	return std::find_if(REL_TYPE_MAP.begin(), REL_TYPE_MAP.end(), [type](const auto& pair) {
		return (pair.second == type);
	})->first;
}

QString generateUsersString(const std::multiset<int>& users)
{
	QStringList l;
	std::transform(users.begin(), users.end(), std::back_inserter(l), [](int n) {
		return QString("%1").arg(n, 2, 16, QChar('0'));
	});
	return l.join(",").toUpper();
}
}
