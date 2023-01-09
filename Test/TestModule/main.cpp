#include "TestObject.h"

int main()
{
	Main::Initialize();

	A a;
	B b;
	C c;
	D d;
	E e;
	bool da = d.IsDerived(a.GetType());
	bool db = d.IsDerived(b.GetType());

	Pointer<C> pc = _NEW C();
	Pointer<D> pd = _NEW D();
	Pointer<E> pe = _NEW E();
	pc->m_i = 123;
	pc->m_f = 0.987;
	pd->m_b = false;
	pd->m_vec = { 1, 2, 3 };
	pd->m_FloatData = _NEW float[5];
	pd->m_FloatDataNum = 5;
	pd->m_testStruct.m_sInt = 2;
	for (int i = 0; i < 5; i++)
	{
		pd->m_FloatData[i] = i + 3.0f;
	}
	for (int i = 0; i < 4; i++)
	{
		pd->m_FloatData[i] = i + 1113.0f;
	}
	pd->m_testStruct.m_sFloat = 0.08;
	pe->m_testVecObject = { _NEW C(1, 1.11111) };
	pe->m_testHashObject = { { "haha", _NEW C(3, 3.14) } };
	pe->m_testHash = { {3, 14} };
	pe->m_pTestObjectCloneD = pd;
	pe->m_pTestObjectClone = pc;
	pe->m_pTestObjectCopy = _NEW C(5, .20f);
	pe->m_pTestObjectClonePtr = pc;
	pe->m_objectD.m_vec = { 3, 4, 5 };

	Object* cloneE = pe->CloneCreateObject(pe);

	// 序列化
	Stream TestStream;
	TestStream.SetStreamFlag(Stream::AT_REGISTER);
	TestStream.ArchiveAll(pe);
	const char Path[] = ("test_stream");
	TestStream.Save(Path);

	// 反序列化
	Pointer<E> pTestSaveLoad = nullptr;
	Stream TestStream2;
	TestStream2.Load(Path);
	pTestSaveLoad = (E*)TestStream2.GetObjectByRtti(E::ms_Type);

	Main::Terminate();

	return 0;
}