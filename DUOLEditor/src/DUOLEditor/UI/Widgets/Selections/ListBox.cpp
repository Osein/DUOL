#include "DUOLEditor/UI/Widgets/Selections/ListBox.h"

namespace DUOLEditor
{
	ListBox::ListBox(int currentChoice) :
		DataWidgetBase<int>(&_currentChoices)
		, _currentChoices(currentChoice)
		, _nextChoice(0)
	{
		_viewChoices.insert({ _nextChoice, TEXT("None") });

		_choices.insert({ _nextChoice++, TEXT("None")});
	}
	
	void ListBox::Draw_Impl()
	{
		// 만약 현재 초이스에 대해서 선택 목록에 없다면 그냥 선택 목록에 맞게 초이스를 되돌린다.
		if (!_viewChoices.contains(_currentChoices))
			_currentChoices = _viewChoices.begin()->first;

		if (ImGui::BeginListBox(DUOLCommon::StringHelper::ToString(_tstringID).c_str(), ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (const auto& [key, value] : _viewChoices)
			{
				bool selected = (key == _currentChoices);

				if (ImGui::Selectable(DUOLCommon::StringHelper::ToString(value).c_str(), selected))
				{
					// 현재 선택과 다른, 지금 키에 대해서 선택이 되었습니다. 즉, 유저의 선택이 바뀌었습니다.
					if (!selected)
					{
						ImGui::SetItemDefaultFocus();
						_currentChoices = key;
						_valueChangedEvent.Invoke(_currentChoices);
						_choiceChangedEvent.Invoke(_viewChoices.at(_currentChoices));
						Notify();
					}
				}
			}

			ImGui::EndListBox();
		}
	}

	void ListBox::AddChoice(const DUOLCommon::tstring& choice)
	{
		_viewChoices.insert({ _nextChoice, choice });

		_choices.insert({ _nextChoice++, choice });
	}

	void ListBox::DeleteChoice()
	{
		_choices.erase(--_nextChoice);
		_viewChoices.erase(_nextChoice);
	}
}
