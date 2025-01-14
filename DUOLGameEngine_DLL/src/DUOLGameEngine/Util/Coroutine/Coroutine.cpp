#include "DUOLGameEngine/Util/Coroutine/Coroutine.h"

namespace DUOLGameEngine
{
	Coroutine::Coroutine(const CoroutineHandler& coroutineHandler) :
		YieldInstructionBase(YieldInstructionType::COROUTINE)
		, _coroutineHandler(coroutineHandler)
	{

	}

	Coroutine::~Coroutine()
	{

	}

	void Coroutine::UpdateInstruction(float deltaTime)
	{
		// 딱히 여기서는 할 일이 없고 그냥 해당 코루틴 객체가 물고 있는 코루틴 핸들러가
		// 잘 끝났는지 확인하고 다음 흐름으로 넘어가면 됩니다.
		// 왜냐하면, 스타트 코루틴으로 일드 리턴하면 .. 어차피 들어가기 때문이지 ..!
	}

	bool Coroutine::CanResume()
	{
		return _coroutineHandler.IsDone();
	}
}