#pragma once
#include "DUOLCommon/Event/Event.h"
#include "DUOLEditor/UI/Widgets/Layout/Container.h"

namespace DUOLEditor
{
	/**
	 * \brief Widget that can contains other widgets and is collapsable
	 */
	class ContainerCollapsable : public Container
	{
	public:
		ContainerCollapsable(const DUOLCommon::tstring& name = TEXT(""), bool isFirstOpen = false);

	protected:
		virtual void Draw_Impl() override;

	private:
		bool _isFirstOpen;

	public:
		DUOLCommon::tstring _name;

		bool _closable;

		bool _isOpened;

		DUOLCommon::Event<void> _closeEvent;

		DUOLCommon::Event<void> _openEvent;
	};
}