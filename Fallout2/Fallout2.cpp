#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "charpoints", typeid(int), "Set character points" }
	});

	DWORD pid = System::WaitForExe(L"fallout2HR.exe");

	Process process(pid);

	if (!process.Verify("048684f802b05859e2b160fa358be1b99075bf31b49a312a536645581aa16b5f"))
	{
		LogError << "Expected Fallout 2 \"HR\" (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("charpoints"))
	{
		int charpoints = args.Value<int>("charpoints");
		process.Write<int32_t>(0x118538, charpoints);
	}

	return 0;
}
