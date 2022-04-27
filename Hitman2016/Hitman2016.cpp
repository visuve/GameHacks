#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	DWORD exitCode = 0;

	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infammo", typeid(std::nullopt), L"Increasing ammunition" },
			{ L"dummyai", typeid(std::nullopt), L"AI cannot shoot"  }
		});

		DWORD pid = System::WaitForWindow(L"Hitman");

		Process process(pid);

		if (!process.Verify("31c33c1937b1ddc3cad21bd7d761f12f962286d55c48d3582a704ca35e2830f9"))
		{
			LogError << "Expected Hitman v1.15.0 with DX 12 (Steam)";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		process.WaitForIdle();
		System::BeepUp();

		if (args.Contains(L"infammo"))
		{
			process.ChangeByte(0x103047, 0xCA, 0xC2);
		}

		if (args.Contains(L"dummyai"))
		{
			// New code
			ByteStream code;
			code << "4C 8D A1 C8 03 00 00"; // lea r12,[rcx+000003C8]
			code << "4D 0F B6 84 24 10 03 00 00"; // movzx r8,byte ptr [r12+00000310]
			code << "66 41 81 F8 01 00"; // cmp r8w,0001
			code << "74 0E"; // je (14 steps forward)
			code << process.JumpAbsolute(0x1030B4); // jump to shooting function end

			// Stolen code
			code << "88 54 24 10"; // mov [rsp+10], dl
			code << "55"; // push rbp
			code << "56"; // push rsi

			code << "41 54"; // push r12
			code << "41 55"; // push r13
			code << "41 57"; // push r15
			code << "48 8D AC 24 E0 FC FF FF"; // lea rbp,[rsp-00000320]
			
			process.InjectX64(0x1028A0, 6, code);

			exitCode = process.WairForExit();
		}

		System::BeepDown();
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << e.what() << "\n";
		std::wcerr << e.Usage();
		System::BeepBurst();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return ERROR_PROCESS_ABORTED;
	}

	return exitCode;
}
