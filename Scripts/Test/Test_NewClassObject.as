

class FooDerived : UObject
{
    void CallMe()
    {
        Log("ASDASDASDASD");
    }
}

class TestBase
{
    int a;
}

class TestB : TestBase
{
    int b;
}

int RunNewObject()
{
    FooDerived asdsd = FooDerived();
    

    UDUMMYUOBJECT Value = NewObject<UDUMMYUOBJECT>(null, "DummyTestUObject");
    if (Value is null)
    {
        return -1;
    }
	if (Value.GetName() != "DummyTestUObject")
		return -2;

    return 1;
}